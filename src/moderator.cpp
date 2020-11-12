#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>

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

  char write_string[1000];
  sprintf(write_string, "Child: %d\nLower Bound: %d\nUpper Bound: %d\nTotal: %d\n", number, lower_bound, upper_bound, upper_bound-lower_bound+1);
  write_to_main(parent_write_fd, write_string);

  PriorityQueue queue;

  while (1) {
    int children_closed = 0;

    for (int i = 0; i < children; i++) {
      if (child_active[i] == false) {
        children_closed++;
        continue;
      }

      while (1) {
        int status;

        char* read_buffer = read_from_worker(worker_read_fd[i], &status);

        if (status == 3){
          close(worker_read_fd[i]);
          child_active[i] = false;
          children_closed++;
          break;
        }else if (status == 0) {
          // std::cout << "MODERATOR: " << read_buffer << '\n';

          if (read_buffer[0] == 'T') {
            close(worker_read_fd[i]);
            child_active[i] = false;
            children_closed++;
            break;
          }else if (read_buffer[0] == 'N'){
            PrimeItem* tmp_item = new PrimeItem(read_buffer);
            // tmp_item->print();
            queue.push(tmp_item);
            // write_to_main(parent_write_fd, read_buffer);
          }

        }else if (status == 1){
          break;
        }else if (status == 2){
          std::cerr << "Read Failure" << '\n';
          exit(EXIT_FAILURE);
        }
      }
    }

    if (children_closed == children) {
      std::cout << number << "----END" << '\n';
      break;
    }
  }

  queue.print();

  char end_string[50];
  sprintf(end_string, "Bye bye from %d", number);
  write_to_main(parent_write_fd, end_string);

  close(parent_write_fd);

  exit(EXIT_SUCCESS);
}
