#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <poll.h>
#include <cstdio>

#include "../include/moderator/initialization.hpp"
#include "../include/utils/utils.hpp"
#include "../include/utils/structures.hpp"
#include "../include/moderator/io.hpp"

int main(int argc, char const *argv[]) {
  int lower_bound;
  int upper_bound;
  int children;
  int number;
  int parent_write_fd;

  int ca = check_arguments(argc, argv, &lower_bound, &upper_bound, &children, &number, &parent_write_fd);
  if (ca != 0) {
    exit(ca);
  }

  int** children_boundaries = distribute_range(lower_bound, upper_bound, children);

  int* worker_read_fd = new int[children];
  int* worker_write_fd = new int[children];
  bool* child_active = new bool[children];

  for (int i = 0; i < children; i++) {
    int fd_temp[2];

    if (pipe(fd_temp) == -1) {
      std::cerr << "Pipe Failed on child " << i+1 << '\n';
      exit(EXIT_FAILURE);
    }

    if (fcntl(fd_temp[0], F_SETFL, O_NONBLOCK) < 0){
      std::cerr << "fcntl failed" << '\n';
      exit(EXIT_FAILURE);
    }

    worker_read_fd[i] = fd_temp[0];
    worker_write_fd[i] = fd_temp[1];

    pid_t pid = fork();

    if (pid == -1) {
      std::cout << "Error in fork()" << '\n';
      exit(EXIT_FAILURE);
    }else if (pid == 0){
      close(worker_read_fd[i]);

      char lower_bound_string[32];
      sprintf(lower_bound_string, "%d", children_boundaries[i][0]);

      char upper_bound_string[32];
      sprintf(upper_bound_string, "%d", children_boundaries[i][1]);

      char number_string[10];
      sprintf(number_string, "%d", number);

      char child_number_string[10];
      sprintf(child_number_string, "%d", number*children + i);

      int algorithm = (i%2) + 1;
      char algorithm_string[2];
      sprintf(algorithm_string, "%d", algorithm);

      char worker_write_fd_string[10];
      sprintf(worker_write_fd_string, "%d", worker_write_fd[i]);

      char* worker_arg_list[] = {"worker", lower_bound_string, upper_bound_string, number_string, child_number_string, algorithm_string, worker_write_fd_string, NULL};

      execv("bin/worker", worker_arg_list);
    }else{
      close(worker_write_fd[i]);
      child_active[i] = true;
    }
  }

  PriorityQueue prime_queue;
  double* time_arr = new double[children];

  struct pollfd* pfd_arr = new struct pollfd[children];
  char* read_buffer;
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
      exit(1);
    }else{
      int status;
      for (int i = 0; i < children; i++) {
        if (pfd_arr[i].revents == 0 || child_active[i] == false)
          continue;

        if (pfd_arr[i].revents & POLLIN){
          PrimeItem tmp_item = read_from_worker(worker_read_fd[i]);
          if (tmp_item.number != 0)
            prime_queue.push(tmp_item);
          else{
            // std::cout << "MODERATOR: " << (children*number)+i << ": " << tmp_item.time << '\n';
            time_arr[i] = tmp_item.time;
          }
        }else{
          close(pfd_arr[i].fd);
          children_closed++;
          child_active[i] = false;
        }
      }
    }
  }

  PrimeItem* worker_times = new PrimeItem[10];
  for (int i = 0; i < children; i++){
    worker_times[i].number = -(children*number + i + 1);
    worker_times[i].time = time_arr[i];
  }

  write(parent_write_fd, worker_times, sizeof(PrimeItem)*10);

  int matrix_size = ceil(prime_queue.size()*1.0/10);
  PrimeItem** batch_matrix = prime_queue.split_in_batches(10);

  for (int i = 0; i < matrix_size; i++){
    PrimeItem* batch_arr = batch_matrix[i];
    write(parent_write_fd, batch_arr, sizeof(PrimeItem)*10);
  }

    // write_to_main(parent_write_fd, batch_matrix[i]);

  // for (int i = 0; i < children; i++)
  //   std::cout << "MOD: " << (number*children)+i << "->Time: " << time_arr[i] << '\n';


  close(parent_write_fd);

  exit(EXIT_SUCCESS);
}
