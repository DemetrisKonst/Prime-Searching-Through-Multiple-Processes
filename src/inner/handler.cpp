#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "../../config/defn.h"

#include "../../include/inner/handler.h"

#include "../../include/utils/utils.h"

// Constructor of INNER handler, used for attribute initialization
Inner :: Inner (int argc, const char* argv[]) {
  if (argc != 6) {
    exit = true;
    exit_val = SYSTEM_ERROR;
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
  exit_val = EXIT_SUCCESS;
}

Inner :: ~Inner () {
  delete[] worker_read_fd;
  delete[] worker_write_fd;
  delete[] child_active;
  delete[] worker_time_arr;
}

// The following method is responsible to initialize 'w' WORKER nodes and pipes
void Inner :: initialize_workers () {
  /*
  boundary[x][0] == lower bound
  boundary[x][1] == upper bound
  This function is inside "include/utils/utils.h"
  */
  long int** children_boundaries = distribute_range(lower_bound, upper_bound, children);

  // 'W' times...
  for (int i = 0; i < children; i++) {
    int fd_temp[2];

    // Create a pipe
    if (pipe(fd_temp) == -1) {
      exit = true;
      exit_val = SYSTEM_ERROR;
      return;
    }

    // Set it to NON-BLOCKING
    if (fcntl(fd_temp[0], F_SETFL, O_NONBLOCK) < 0){
      exit = true;
      exit_val = SYSTEM_ERROR;
      return;
    }

    worker_read_fd[i] = fd_temp[0];
    worker_write_fd[i] = fd_temp[1];

    // Fork the process
    pid_t pid = fork();

    if (pid == -1) {
      exit = true;
      exit_val = SYSTEM_ERROR;
      return;
    }else if (pid == 0){
      // Inside child: initialize cmd arguments and call bin/worker through execv
      close(worker_read_fd[i]);

      char lower_bound_string[LONG_STR_SIZE + 1];
      sprintf(lower_bound_string, "%ld", children_boundaries[i][0]);

      char upper_bound_string[LONG_STR_SIZE + 1];
      sprintf(upper_bound_string, "%ld", children_boundaries[i][1]);

      char number_string[INT_STR_SIZE + 1];
      sprintf(number_string, "%d", inner_number);

      char children_string[INT_STR_SIZE + 1];
      sprintf(children_string, "%d", children);

      char child_number_string[INT_STR_SIZE + 1];
      sprintf(child_number_string, "%d", i);

      char worker_write_fd_string[INT_STR_SIZE + 1];
      sprintf(worker_write_fd_string, "%d", worker_write_fd[i]);

      char root_pid_string[INT_STR_SIZE + 1];
      sprintf(root_pid_string, "%d", (int) root_pid);

      char* worker_arg_list[] = {"worker", lower_bound_string, upper_bound_string, number_string, children_string, child_number_string, worker_write_fd_string, root_pid_string, NULL};

      if (execv("bin/worker", worker_arg_list) < 0) {
        std::cerr << "Exec failed on bin/inner " << i+1 << '\n';
        exit = true;
        exit_val = SYSTEM_ERROR;
        return;
      }
    }else{
      // Inside parent: close write-end of pipe and set child as "active"
      delete[] children_boundaries[i];
      close(worker_write_fd[i]);
      child_active[i] = true;
    }
  }
  delete[] children_boundaries;
}

// This is responsible for fetching the arrays of primes sent by the workers
void Inner :: get_primes () {
  // Initalize poll structure
  struct pollfd* pfd_arr = new struct pollfd[children];
  int children_closed = 0;

  for (int i = 0; i < children; i++) {
    pfd_arr[i].fd = worker_read_fd[i];
    pfd_arr[i].events = POLLIN;
  }

  // Repeat until all children are "inactive" (i.e. have closed their write-ends of their pipe)
  while (children_closed < children) {
    int poll_res;
    poll_res = poll(pfd_arr, children, POLL_TIMEOUT);

    if (poll_res == 0) {
      exit = true;
      exit_val = SYSTEM_ERROR;
      return;
    }else{
      // For every child whose write-end is ready...
      for (int i = 0; i < children; i++) {
        if (pfd_arr[i].revents == 0 || child_active[i] == false)
          continue;

        // If child has written in write-end...
        if (pfd_arr[i].revents & POLLIN){
          // Read one "PrimeItem"
          PrimeItem item;
          read(worker_read_fd[i], &item, sizeof(PrimeItem));

          // If it has a positive "number", it indicates a prime number
          if (item.number != 0)
            prime_queue.push(item);
          // If the number is 0, it represents the total time the worker spent calculating
          else
            worker_time_arr[i] = item.time;

        // If child has closed write-end, set it as "inactive" and close read-end
        }else{
          close(pfd_arr[i].fd);
          children_closed++;
          child_active[i] = false;
        }
      }
    }
  }

  delete[] pfd_arr;
}

// The following method builds the data collected into an acceptable form to pass into ROOT
void Inner :: build_output () {
  /*
  At first, create an array of PrimeItems to store total worker times.
  Set their number as: -(worker_number) so that ROOT understands the nature of this array
  */
  PrimeItem* worker_times = new PrimeItem[PIPE_BATCH_SIZE];
  for (int i = 0; i < children; i++){
    worker_times[i].number = -(children*inner_number + i + 1);
    worker_times[i].time = worker_time_arr[i];
  }

  // Write this array to root
  write(root_write_fd, worker_times, sizeof(PrimeItem)*PIPE_BATCH_SIZE);
  delete[] worker_times;
  /*
  Then split the PriorityQueue in batches of size PIPE_BATCH_SIZE using split_in_batches.
  This returns a 2D-array of PrimeItems of ceil(prime_queue.size()/PIPE_BATCH_SIZE) rows
  and PIPE_BATCH_SIZE columns
  */
  int matrix_size = ceil(prime_queue.size()*1.0/PIPE_BATCH_SIZE);
  PrimeItem** batch_matrix = prime_queue.split_in_batches(PIPE_BATCH_SIZE);

  // Finally, write each row into the parent pipe write-end
  for (int i = 0; i < matrix_size; i++){
    PrimeItem* batch_arr = batch_matrix[i];
    write(root_write_fd, batch_arr, sizeof(PrimeItem)*PIPE_BATCH_SIZE);
    delete[] batch_arr;
  }

  delete[] batch_matrix;
}

int Inner :: finish () {
  // Close the write-end to ROOT
  close(root_write_fd);

  if (exit_val == USER_ERROR)
    std::cout << "INNER-User Error: Exitting" << '\n';
  else if (exit_val == SYSTEM_ERROR)
    std::cout << "INNER-System Error: Exitting" << '\n';
  return EXIT_SUCCESS;
}
