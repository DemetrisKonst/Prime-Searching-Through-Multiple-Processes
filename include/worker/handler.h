#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/times.h>

#include "../core/prime_item.hpp"
// #include "../core/algorithm.hpp"

class Worker {
private:
  long int lower_bound;
  long int upper_bound;

  int parent_number;
  int children;
  int worker_inner_number;
  int worker_total_number;
  int algorithm;

  int inner_write_fd;
  pid_t root_pid;

  double tics_per_sec;

  bool exit;
  int exit_val;

public:
  Worker(int, const char**);

  void calculate_primes ();

  int finish ();
};
