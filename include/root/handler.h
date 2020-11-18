#pragma once

#include <signal.h>

#include "../../include/utils/structures.h"

// Global variable representing number of signals received by ROOT
extern volatile sig_atomic_t signals_received;

// Signal handler for usage in sigaction()
void signal_handler (int);

class Root {
private:
  long int lower_bound;
  long int upper_bound;

  int children;

  // Arrays of all read-ends and write-ends of pipes between ROOT and its children
  int* inner_read_fd;
  int* inner_write_fd;

  // Logical array indicating whether a child has finished execution or not
  bool* child_active;

  // Priority Queue containing all primes found by the program
  PriorityQueue prime_queue;
  // Array of times taken by all WORKERs to calculate all primes
  double* worker_time_arr;

  // Variables related to errors
  bool exit;
  int exit_val;

public:
  // The implementation and usage of the following methods is documented in src/root/handler.cpp
  Root(int, const char**);
  ~Root();

  void initialize_inner ();

  void get_primes();

  void build_output();

  // Methods related to exitting and error-handling
  inline bool get_exit() { return exit; }
  int finish();
};
