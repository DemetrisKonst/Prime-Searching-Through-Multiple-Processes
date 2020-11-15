#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>

#include "../include/core/prime_item.hpp"
#include "../include/main/initialization.hpp"
#include "../include/utils/utils.hpp"
#include "../include/utils/structures.hpp"
#include "../include/main/io.hpp"

int main(int argc, char const *argv[]) {
  int lower_bound;
  int upper_bound;
  int children;

  int ca = check_arguments(argc, argv, &lower_bound, &upper_bound, &children);
  if (ca != 0) {
    exit(ca);
  }

  int** children_boundaries = distribute_range(lower_bound, upper_bound, children);

  pid_t* pid_arr = new pid_t[children];
  int* moderator_read_fd = new int[children];
  int* moderator_write_fd = new int[children];
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

    moderator_read_fd[i] = fd_temp[0];
    moderator_write_fd[i] = fd_temp[1];

    pid_t pid = fork();

    if (pid == -1) {
      std::cout << "Error in fork()" << '\n';
      exit(EXIT_FAILURE);
    }else if (pid == 0) {
      // Child process has been created, we are currently inside its execution
      close(moderator_read_fd[i]);

      char lower_bound_string[32];
      snprintf(lower_bound_string, sizeof(lower_bound_string), "%d", children_boundaries[i][0]);

      char upper_bound_string[32];
      snprintf(upper_bound_string, sizeof(upper_bound_string), "%d", children_boundaries[i][1]);

      char children_string[2];
      snprintf(children_string, sizeof(children_string), "%d", children);

      char number_string[2];
      snprintf(number_string, sizeof(number_string), "%d", i);

      char write_fd_string[8];
      sprintf(write_fd_string, "%d", moderator_write_fd[i]);

      char* moderator_argv_list[] = {"moderator", lower_bound_string, upper_bound_string, children_string, number_string, write_fd_string, NULL};

      execv("bin/moderator", moderator_argv_list);
    }else{
      // Child process has been created, we are currently inside the parent's execution
      close(moderator_write_fd[i]);
      child_active[i]=true;
      // pid_arr[i] = pid;
    }
  }

  PriorityQueue prime_queue;
  double* worker_times = new double[children*children];

  struct pollfd* pfd_arr = new struct pollfd[children];
  char* read_buffer;
  int children_closed = 0;

  for (int i = 0; i < children; i++) {
    pfd_arr[i].fd = moderator_read_fd[i];
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
          PrimeItem* item_arr = read_from_moderator(moderator_read_fd[i]);

          for (int i = 0; i < 10; i++){
            if (item_arr[i].number > 0)
              prime_queue.push(item_arr[i]);
            else if (item_arr[i].number < 0)
              worker_times[-(item_arr[i].number+1)] = item_arr[i].time;
          }
        }else{
          close(pfd_arr[i].fd);
          children_closed++;
          child_active[i] = false;
        }
      }
    }
  }

  std::cout << "Primes in [" << lower_bound << ", " << upper_bound << "] are :";
  prime_queue.print();
  std::cout << '\n';

  double time_min = worker_times[0];
  double time_max = worker_times[0];

  for (int i = 1; i < children*children; i++){
    if (worker_times[i] > time_max)
      time_max = worker_times[i];

    if (worker_times[i] < time_min)
      time_min = worker_times[i];
  }

  std::cout << "Min Time For Workers: " << time_min << '\n';
  std::cout << "Max Time For Workers: " << time_max << '\n';

  for (int i = 0; i < children*children; i++)
    std::cout << "Time for W" << i+1 << ": " << worker_times[i] << '\n';

  // for (int i = 0; i < children; i++){
  //   pid_t cpid;
  //   int status;
  //   cpid = wait(&status);
  //   if (WIFEXITED(status))
  //       printf("Exit status: %d\n", WEXITSTATUS(status));
  //   else if (WIFSIGNALED(status))
  //       psignal(WTERMSIG(status), "Exit signal");
  // }

  exit(EXIT_SUCCESS);
}
