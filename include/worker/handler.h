#pragma once

#include <iostream>
#include <unistd.h>
#include <sys/times.h>

#include "../core/prime_item.h"
// #include "../core/algorithm.hpp"

class Worker {
private:
  long int lower_bound;
  long int upper_bound;

  // Number of parent
  int parent_number;
  // Number of children cmd-argument
  int children;
  // Worker number in relation to "sibling" workers
  int worker_inner_number;
  // Worker number in relation to all workers of the program
  int worker_total_number;
  // Algorithm used to check if a number is prime or not
  int algorithm;

  // Write-end of pipe between this process and its parent (INNER)
  int inner_write_fd;
  // The process id of root
  pid_t root_pid;

  // Constant to calculate seconds properly
  double tics_per_sec;

  // Variables related to errors
  bool exit;
  int exit_val;

public:
  // The implementation and usage of the following methods is documented in src/worker/handler.cpp
  Worker(int, const char**);

  void calculate_primes ();

  // Methods related to exitting and error-handling
  inline bool get_exit() { return exit; }
  int finish ();
};
