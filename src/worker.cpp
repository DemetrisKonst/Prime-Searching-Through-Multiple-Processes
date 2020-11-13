#include <iostream>
#include <unistd.h>
#include <sys/times.h>

#include "../include/core/prime_item.hpp"
#include "../include/worker/initialization.hpp"
#include "../include/worker/io.hpp"
#include "../include/algorithm.hpp"

int main(int argc, char const *argv[]) {
  int lower_bound;
  int upper_bound;
  int parent_number;
  int number;
  int algorithm;
  int write_fd;

  int ca = check_arguments(argc, argv, &lower_bound, &upper_bound, &parent_number, &number, &algorithm, &write_fd);
  if (ca != 0) {
    exit(ca);
  }

  double tics_per_sec = (double) sysconf(_SC_CLK_TCK);

  struct tms tb_total1, tb_total2;
  double t_total1 = (double) times(&tb_total1);

  for (int n = lower_bound; n <= upper_bound; n++) {
    struct tms tb_single1, tb_single2;
    double t_single1 = (double) times(&tb_single1);

    bool is_prime = prime1(n);
    if (!is_prime)
      continue;

    double t_single2 = (double) times(&tb_single2);
    double single_real_time = (double) (t_single2 - t_single1) / tics_per_sec;

    PrimeItem item(n, single_real_time);
    // std::cout << "WORKER: " << '\n';
    // item.print();
    write(write_fd, &item, sizeof(PrimeItem));
    // write_to_moderator(write_fd, item);
  }

  double t_total2 = (double) times(&tb_total2);
  double total_real_time = (double) (t_total2 - t_total1) / tics_per_sec;

  PrimeItem time_item(0, number*1.0);
  std::cout << "WRK: " << number << "->Time: " << number*1.0 << '\n';
  write(write_fd, &time_item, sizeof(PrimeItem));
  // write_to_moderator(write_fd, time_item);

  close(write_fd);

  exit(EXIT_SUCCESS);
}
