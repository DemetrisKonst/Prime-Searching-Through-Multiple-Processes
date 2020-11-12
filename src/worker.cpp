#include <iostream>
#include <unistd.h>
#include <sys/times.h>

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

    double single_cpu_time = (double) (tb_single2.tms_utime + tb_single2.tms_stime) - (tb_single1.tms_utime + tb_single1.tms_stime);
    double single_real_time = (double) (t_single2 - t_single1) / tics_per_sec;

    char write_string[20];
    sprintf(write_string, "N:%d-%1f,%1f", n, single_cpu_time, single_real_time);

    std::cout << "WORKER--- " << write_string << '\n';

    write_to_moderator(write_fd, write_string);
  }

  double t_total2 = (double) times(&tb_total2);

  double total_cpu_time = (double) (tb_total2.tms_utime + tb_total2.tms_stime) - (tb_total1.tms_utime + tb_total1.tms_stime);
  double total_real_time = (double) (t_total2 - t_total1) / tics_per_sec;

  char final_write_string[255];
  sprintf(final_write_string, "T:%1f,%1f", total_cpu_time, total_real_time);

  std::cout << "WORKER--- " << final_write_string << '\n';

  write_to_moderator(write_fd, final_write_string);

  close(write_fd);

  exit(EXIT_SUCCESS);
}
