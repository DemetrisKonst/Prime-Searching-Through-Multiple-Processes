#include <signal.h>

#include "../../config/defn.h"

#include "../../include/worker/handler.h"

#include "../../include/core/algorithm.h"

// Constructor of WORKER handler, used for attribute initialization
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

// The following method is responsible for calculating and writing primes into its parent
void Worker :: calculate_primes () {
  // Initalize two tms structures to calculate the total time taken to calculate all primes in range
  struct tms tb_total1, tb_total2;
  double t_total1 = (double) times(&tb_total1);
  double t_total2;

  // Initalize two more tms structures to calculate the time elapsed for each prime to be calculated
  struct tms tb_single1, tb_single2;
  double t_single1 = (double) times(&tb_single1);
  double t_single2;

  // For every number in the given range
  for (long int n = lower_bound; n <= upper_bound; n++) {
    bool is_prime;

    // Use the algorithm provided to find if it is a prime
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

    // If it is a prime
    if (is_prime) {
      /*
      Create a new PrimeItem with the following information:
      number = n, time = time elapsed from previous prime found
      */
      t_single2 = (double) times(&tb_single2);
      double single_real_time = (double) (t_single2 - t_single1) / tics_per_sec;
      PrimeItem item(n, single_real_time);
      // Write that PrimeItem to parent
      write(inner_write_fd, &item, sizeof(PrimeItem));
      // Reset the clock
      t_single1 = (double) times(&tb_total1);
    }

  }

  // Once finished, calculate total time elapsed
  t_total2 = (double) times(&tb_total2);
  double total_real_time = (double) (t_total2 - t_total1) / tics_per_sec;

  /*
  Create a PrimeItem with the following information:
  number = 0, time = total time elapsed
  */
  PrimeItem time_item(0, total_real_time);
  // Write it to parent
  write(inner_write_fd, &time_item, sizeof(PrimeItem));
}

int Worker :: finish () {
  // Close write-end to parent
  close(inner_write_fd);
  if (exit_val == USER_ERROR)
    std::cout << "WORKER-User Error: Exitting" << '\n';
  else if (exit_val == SYSTEM_ERROR)
    std::cout << "WORKER-System Error: Exitting" << '\n';
  // Send SIGUSR1 signal to root
  kill(root_pid, SIGUSR1);
  return EXIT_SUCCESS;
}
