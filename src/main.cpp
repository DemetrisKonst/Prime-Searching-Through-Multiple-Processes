#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>

#include "../include/main/initialization.hpp"
#include "../include/utils/utils.hpp"
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
          read_buffer = read_from_moderator(moderator_read_fd[i]);
          std::cout << "MAIN: " << read_buffer << '\n';
        }else{
          close(pfd_arr[i].fd);
          children_closed++;
          child_active[i] = false;
        }
      }
    }
  }

  // while (1) {
  //   int children_closed = 0;
  //
  //   for (int i = 0; i < children; i++) {
  //     if (child_active[i] == false) {
  //       children_closed++;
  //       continue;
  //     }
  //
  //     while (1) {
  //       int status;
  //
  //       char* read_buffer = read_from_moderator(moderator_read_fd[i], &status);
  //
  //       if (status == 0) {
  //         std::cout << read_buffer << '\n';
  //       }else if (status == 1){
  //         break;
  //       }else if (status == 2){
  //         std::cerr << "Read Failure" << '\n';
  //         exit(EXIT_FAILURE);
  //       }else if (status == 3){
  //         close(moderator_read_fd[i]);
  //         child_active[i] = false;
  //         break;
  //       }
  //     }
  //   }
  //
  //   if (children_closed == children) {
  //     break;
  //   }
  // }

  for (int i = 0; i < children; i++){
    pid_t cpid;
    int status;
    cpid = wait(&status);
    if (WIFEXITED(status))
        printf("Exit status: %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        psignal(WTERMSIG(status), "Exit signal");
  }

  exit(EXIT_SUCCESS);
}
