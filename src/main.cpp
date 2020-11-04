#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../include/main/initialization.hpp"
#include "../include/utils/utils.hpp"

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

  for (int i = 0; i < children; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      std::cout << "Error in fork()" << '\n';
      exit(EXIT_FAILURE);
    }else if (pid == 0) {
      // Child process has been created, we are currently inside its execution

      char lower_bound_string[32];
      snprintf(lower_bound_string, sizeof(lower_bound_string), "%d", children_boundaries[i][0]);

      char upper_bound_string[32];
      snprintf(upper_bound_string, sizeof(upper_bound_string), "%d", children_boundaries[i][1]);

      char children_string[2];
      snprintf(children_string, sizeof(children_string), "%d", children);

      char number_string[2];
      snprintf(number_string, sizeof(number_string), "%d", i);

      char* moderator_argv_list[] = {"moderator", lower_bound_string, upper_bound_string, children_string, number_string, NULL};

      execv("bin/moderator", moderator_argv_list);
    }else{
      // Child process has been created, we are currently inside the parent's execution
      pid_arr[i] = pid;
    }
  }

  for (int i = 0; i < children; i++)
    wait(NULL);

  exit(EXIT_SUCCESS);
}
