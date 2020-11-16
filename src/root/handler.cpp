#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "../../config/defn.h"

#include "../../include/root/handler.h"

#include "../../include/utils/utils.hpp"

volatile sig_atomic_t signals_received = 0;

void signal_handler (int sig) {
  // std::cout << "Caught " << sig << '\n';
  signals_received++;
}

Root :: Root (int argc, const char* argv[]) {
  if (PIPE_BATCH_SIZE < 3) {
    std::cout << "Batch size must be larger than 2" << '\n';
    exit = true;
    exit_val = USER_ERROR;
    return;
  }

  if (argc != 7) {
    std::cout << "Wrong number of arguments (6 required)" << '\n';
    exit = true;
    exit_val = 1;
    return;
  }

  bool lower_bound_check = false;
  bool upper_bound_check = false;
  bool children_check = false;

  for (int i = 1; i < 7; i+=2) {
    if (strcmp(argv[i], "-l") == 0) {
      if (lower_bound_check) {
        std::cout << "Lower Bound already declared, please check your arguments" << '\n';
        exit = true;
        exit_val = USER_ERROR;
        return;
      }else{
        lower_bound = atol(argv[i+1]);

        if (lower_bound < 0) {
          std::cout << "Lower Bound must be a positive integer" << '\n';
          exit = true;
          exit_val = USER_ERROR;
          return;
        }

        lower_bound_check = true;
      }
    }else if (strcmp(argv[i], "-u") == 0) {
      if (upper_bound_check) {
        std::cout << "Upper Bound already declared, please check your arguments" << '\n';
        exit = true;
        exit_val = USER_ERROR;
        return;
      }else{
        upper_bound = atol(argv[i+1]);

        if (upper_bound < 0) {
          std::cout << "Upper Bound must be a positive integer" << '\n';
          exit = true;
          exit_val = USER_ERROR;
          return;
        }

        upper_bound_check = true;
      }
    }else if (strcmp(argv[i], "-w") == 0) {
      if (children_check) {
        std::cout << "Number of children already declared, please check your arguments" << '\n';
        exit = true;
        exit_val = USER_ERROR;
        return;
      }else{
        children = atoi(argv[i+1]);

        if (children < 0) {
          std::cout << "Number of children must be a positive integer" << '\n';
          exit = true;
          exit_val = USER_ERROR;
          return;
        }

        children_check = true;
      }
    }else{
      std::cout << "There is no argument named: " << argv[i] << '\n';
      exit = true;
      exit_val = USER_ERROR;
      return;
    }
  }

  if (! (lower_bound_check && upper_bound_check && children_check)) {
    std::cout << "Arguments missing (check your arguments)" << '\n';
    exit = true;
    exit_val = USER_ERROR;
    return;
  }

  if (lower_bound >= upper_bound) {
    std::cout << "Upper bound must be larger than lower bound" << '\n';
    exit = true;
    exit_val = USER_ERROR;
    return;
  }

  inner_read_fd = new int[children];
  inner_write_fd = new int[children];

  child_active = new bool[children];

  worker_time_arr = new double[children];

  exit = false;
  exit_val = EXIT_SUCCESS;

  struct sigaction signal_action;

  signal_action.sa_handler = signal_handler;
  sigemptyset(&signal_action.sa_mask);
  signal_action.sa_flags = SA_RESTART;

  if (sigaction(SIGUSR1, &signal_action, NULL) == -1){
    std::cout << "Error in sigaction()" << '\n';
    exit = true;
    exit_val = SYSTEM_ERROR;
  }
}

void Root :: initialize_inner () {
  long int** children_boundaries = distribute_range(lower_bound, upper_bound, children);

  for (int i = 0; i < children; i++) {
    int fd_temp[2];

    if (pipe(fd_temp) == -1) {
      std::cerr << "Pipe Failed on child " << i+1 << '\n';
      exit = true;
      exit_val = SYSTEM_ERROR;
      return;
    }

    if (fcntl(fd_temp[0], F_SETFL, O_NONBLOCK) < 0){
      std::cerr << "Fcntl Failed on child " << i+1 << '\n';
      exit = true;
      exit_val = SYSTEM_ERROR;
      return;
    }

    inner_read_fd[i] = fd_temp[0];
    inner_write_fd[i] = fd_temp[1];

    pid_t pid = fork();

    if (pid == -1) {
      std::cerr << "Fork Failed on child " << i+1 << '\n';
      exit = true;
      exit_val = SYSTEM_ERROR;
      return;
    }else if (pid == 0) {
      // Child process has been created, we are currently inside its execution
      close(inner_read_fd[i]);

      char lower_bound_string[LONG_STR_SIZE + 1];
      sprintf(lower_bound_string, "%ld", children_boundaries[i][0]);

      char upper_bound_string[LONG_STR_SIZE + 1];
      sprintf(upper_bound_string, "%ld", children_boundaries[i][1]);

      char children_string[INT_STR_SIZE + 1];
      sprintf(children_string, "%d", children);

      char child_number_string[INT_STR_SIZE + 1];
      sprintf(child_number_string, "%d", i);

      char inner_write_fd_string[INT_STR_SIZE + 1];
      sprintf(inner_write_fd_string, "%d", inner_write_fd[i]);

      char* inner_argv_list[] = {"inner", lower_bound_string, upper_bound_string, children_string, child_number_string, inner_write_fd_string, NULL};
      execv("bin/inner", inner_argv_list);
    }else{
      // Child process has been created, we are currently inside the parent's execution
      close(inner_write_fd[i]);
      child_active[i]=true;
    }
  }
}


void Root :: get_primes () {
  struct pollfd* pfd_arr = new struct pollfd[children];
  int children_closed = 0;

  for (int i = 0; i < children; i++) {
    pfd_arr[i].fd = inner_read_fd[i];
    pfd_arr[i].events = POLLIN;
  }

  while (children_closed < children) {
    int poll_res;

    poll_res = poll(pfd_arr, children, -1);

    if (poll_res == 0) {
      std::cout << "Poll timed out" << '\n';
      exit = true;
      exit_val = SYSTEM_ERROR;
      return;
    }else{
      for (int i = 0; i < children; i++) {
        if (pfd_arr[i].revents == 0 || child_active[i] == false)
          continue;

        if (pfd_arr[i].revents & POLLIN){
          PrimeItem* item_arr = new PrimeItem[PIPE_BATCH_SIZE];
          read(inner_read_fd[i], item_arr, sizeof(PrimeItem)*PIPE_BATCH_SIZE);

          for (int i = 0; i < PIPE_BATCH_SIZE; i++){
            if (item_arr[i].number > 0)
              prime_queue.push(item_arr[i]);
            else if (item_arr[i].number < 0)
              worker_time_arr[-(item_arr[i].number+1)] = item_arr[i].time;
          }
        }else{
          close(pfd_arr[i].fd);
          children_closed++;
          child_active[i] = false;
        }
      }
    }
  }
}


void Root :: build_output () {
  std::cout << "Primes in [" << lower_bound << ", " << upper_bound << "] are: ";
  prime_queue.print();
  std::cout << '\n';

  double time_min = worker_time_arr[0];
  double time_max = worker_time_arr[0];

  for (int i = 1; i < children*children; i++){
    if (worker_time_arr[i] > time_max)
      time_max = worker_time_arr[i];

    if (worker_time_arr[i] < time_min)
      time_min = worker_time_arr[i];
  }

  std::cout << "Min Time For Workers: " << time_min << '\n';
  std::cout << "Max Time For Workers: " << time_max << '\n';

  std::cout << "Num of USR1 Received: " << signals_received << '\n';

  for (int i = 0; i < children*children; i++)
    std::cout << "Time for W" << i+1 << ": " << worker_time_arr[i] << '\n';
}

int Root :: finish () {
  if (exit_val == USER_ERROR)
    std::cout << "ROOT-User Error: Exitting" << '\n';
  else if (exit_val == SYSTEM_ERROR)
    std::cout << "ROOT-System Error: Exitting" << '\n';
  return exit_val;
}
