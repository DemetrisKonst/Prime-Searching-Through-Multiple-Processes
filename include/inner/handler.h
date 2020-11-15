#pragma once

#include "../../include/utils/structures.hpp"

class Inner {
private:
  long int lower_bound;
  long int upper_bound;

  int children;
  int inner_number;

  int root_write_fd;
  int* worker_read_fd;
  int* worker_write_fd;
  pid_t root_pid;

  bool* child_active;

  PriorityQueue prime_queue;
  double* worker_time_arr;

  bool exit;
  int exit_val;

public:
  Inner(int, const char**);

  void initialize_workers();

  void get_primes();

  void build_output();

  int finish();
};
