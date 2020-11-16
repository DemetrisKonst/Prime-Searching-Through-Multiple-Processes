#pragma once

#include <signal.h>

#include "../../include/utils/structures.hpp"

extern volatile sig_atomic_t signals_received;

void signal_handler (int);

class Root {
private:
  long int lower_bound;
  long int upper_bound;

  int children;

  int* inner_read_fd;
  int* inner_write_fd;

  bool* child_active;

  PriorityQueue prime_queue;
  double* worker_time_arr;

  bool exit;
  int exit_val;

public:
  Root(int, const char**);

  void initialize_inner ();

  void get_primes();

  void build_output();

  inline bool get_exit() { return exit; }
  int finish();
};
