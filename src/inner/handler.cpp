#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "../../config/defn.h"

#include "../../include/inner/handler.h"

#include "../../include/utils/utils.hpp"

Inner :: Inner (int argc, const char* argv[]) {
  if (argc != 6) {
    std::cout << "Wrong number of arguments (5 required)" << '\n';
    exit = true;
    exit_val = 1;
    return;
  }

  lower_bound = atol(argv[1]);
  upper_bound = atol(argv[2]);

  children = atoi(argv[3]);
  inner_number = atoi(argv[4]);

  root_write_fd = atoi(argv[5]);
  worker_read_fd = new int[children];
  worker_write_fd = new int[children];
  root_pid = getppid();

  child_active = new bool[children];

  worker_time_arr = new double[children];

  exit = false;
  exit_val = 0;
}

void Inner :: initialize_workers () {
  long int** children_boundaries = distribute_range(lower_bound, upper_bound, children);

  for (int i = 0; i < children; i++) {
    int fd_temp[2];

    if (pipe(fd_temp) == -1) {
      std::cerr << "Pipe Failed on child " << i+1 << '\n';
      exit = true;
      exit_val = 2;
      return;
    }

    if (fcntl(fd_temp[0], F_SETFL, O_NONBLOCK) < 0){
      std::cerr << "Fcntl Failed on child " << i+1 << '\n';
      exit = true;
      exit_val = 2;
      return;
    }

    worker_read_fd[i] = fd_temp[0];
    worker_write_fd[i] = fd_temp[1];

    pid_t pid = fork();

    if (pid == -1) {
      std::cerr << "Fork Failed on child " << i+1 << '\n';
      exit = true;
      exit_val = 3;
      return;
    }else if (pid == 0){
      close(worker_read_fd[i]);

      char lower_bound_string[32];
      sprintf(lower_bound_string, "%ld", children_boundaries[i][0]);

      char upper_bound_string[32];
      sprintf(upper_bound_string, "%ld", children_boundaries[i][1]);

      char number_string[10];
      sprintf(number_string, "%d", inner_number);

      char children_string[10];
      sprintf(children_string, "%d", children);

      char child_number_string[10];
      sprintf(child_number_string, "%d", i);

      char worker_write_fd_string[10];
      sprintf(worker_write_fd_string, "%d", worker_write_fd[i]);

      char root_pid_string[10];
      sprintf(root_pid_string, "%d", (int) root_pid);

      char* worker_arg_list[] = {"worker", lower_bound_string, upper_bound_string, number_string, children_string, child_number_string, worker_write_fd_string, root_pid_string, NULL};

      execv("bin/worker", worker_arg_list);
    }else{
      close(worker_write_fd[i]);
      child_active[i] = true;
    }
  }
}

void Inner :: get_primes () {
  struct pollfd* pfd_arr = new struct pollfd[children];
  int children_closed = 0;

  for (int i = 0; i < children; i++) {
    pfd_arr[i].fd = worker_read_fd[i];
    pfd_arr[i].events = POLLIN;
  }

  while (children_closed < children) {
    int poll_res;
    poll_res = poll(pfd_arr, children, -1);

    if (poll_res == 0) {
      std::cout << "Poll timed out" << '\n';
      exit = true;
      exit_val = 3;
      return;
    }else{
      for (int i = 0; i < children; i++) {
        if (pfd_arr[i].revents == 0 || child_active[i] == false)
          continue;

        if (pfd_arr[i].revents & POLLIN){
          PrimeItem item;
          read(worker_read_fd[i], &item, sizeof(PrimeItem));

          if (item.number != 0)
            prime_queue.push(item);
          else
            worker_time_arr[i] = item.time;

        }else{
          close(pfd_arr[i].fd);
          children_closed++;
          child_active[i] = false;
        }
      }
    }
  }
}

void Inner :: build_output () {
  PrimeItem* worker_times = new PrimeItem[PIPE_BATCH_SIZE];
  for (int i = 0; i < children; i++){
    worker_times[i].number = -(children*inner_number + i + 1);
    worker_times[i].time = worker_time_arr[i];
  }

  write(root_write_fd, worker_times, sizeof(PrimeItem)*PIPE_BATCH_SIZE);

  int matrix_size = ceil(prime_queue.size()*1.0/PIPE_BATCH_SIZE);
  PrimeItem** batch_matrix = prime_queue.split_in_batches(PIPE_BATCH_SIZE);

  for (int i = 0; i < matrix_size; i++){
    PrimeItem* batch_arr = batch_matrix[i];
    write(root_write_fd, batch_arr, sizeof(PrimeItem)*PIPE_BATCH_SIZE);
  }
}

int Inner :: finish () {
  close(root_write_fd);
  return EXIT_SUCCESS;
}
