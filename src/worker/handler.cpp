#include <signal.h>

#include "../../config/defn.h"

#include "../../include/worker/handler.h"

#include "../../include/core/algorithm.hpp"

Worker :: Worker (int argc, const char* argv[]) {
  if (argc != 8) {
    exit = true;
    exit_val = SYSTEM_ERROR;
    return;
  }

  lower_bound = atol(argv[1]);
  upper_bound = atol(argv[2]);

  parent_number = atoi(argv[3]);
  children = atoi(argv[4]);
  worker_inner_number = atoi(argv[5]);
  worker_total_number = (parent_number*children) + worker_inner_number;
  algorithm = (worker_inner_number%3)+1;

  inner_write_fd = atoi(argv[6]);
  root_pid = (pid_t) atoi(argv[7]);

  tics_per_sec = (double) sysconf(_SC_CLK_TCK);

  exit = false;
  exit_val = EXIT_SUCCESS;
}

void Worker :: calculate_primes () {
  struct tms tb_total1, tb_total2;
  double t_total1 = (double) times(&tb_total1);
  double t_total2;

  struct tms tb_single1, tb_single2;
  double t_single1 = (double) times(&tb_single1);
  double t_single2;

  for (long int n = lower_bound; n <= upper_bound; n++) {
    bool is_prime;

    switch (algorithm) {
      case 1:
        is_prime = prime1(n);
        break;
      case 2:
        is_prime = prime2(n);
        break;
      case 3:
        is_prime = prime3(n);
        break;
      default:
        std::cout << "Unrecognized algorithm " << algorithm << '\n';
    }

    if (is_prime) {
      t_single2 = (double) times(&tb_single2);
      double single_real_time = (double) (t_single2 - t_single1) / tics_per_sec;
      PrimeItem item(n, single_real_time);
      write(inner_write_fd, &item, sizeof(PrimeItem));
      t_single1 = (double) times(&tb_total1);
    }

  }

  t_total2 = (double) times(&tb_total2);
  double total_real_time = (double) (t_total2 - t_total1) / tics_per_sec;

  PrimeItem time_item(0, total_real_time);
  write(inner_write_fd, &time_item, sizeof(PrimeItem));
}

int Worker :: finish () {
  close(inner_write_fd);
  // std::cout << worker_total_number << "-Killing Root..." << '\n';
  if (exit_val == USER_ERROR)
    std::cout << "WORKER-User Error: Exitting" << '\n';
  else if (exit_val == SYSTEM_ERROR)
    std::cout << "WORKER-System Error: Exitting" << '\n';
  kill(root_pid, SIGUSR1);
  return EXIT_SUCCESS;
}
