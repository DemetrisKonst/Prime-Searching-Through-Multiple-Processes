#pragma once

#include "../../include/utils/structures.h"

class Inner {
private:
  long int lower_bound;
  long int upper_bound;

  int children;
  int inner_number;

  // Write-end of pipe between this process and its parent (ROOT)
  int root_write_fd;
  // Arrays of all read-ends and write-ends of pipes between this process and its children (WORKER)
  int* worker_read_fd;
  int* worker_write_fd;
  // The process id of root (used in WORKERs)
  pid_t root_pid;

  // Logical array indicating whether a child has finished execution or not
  bool* child_active;

  // Priority Queue containing primes found by the children of this process
  PriorityQueue prime_queue;
  // Array of times taken by WORKERs to calculate all primes
  double* worker_time_arr;

  // Variables related to errors
  bool exit;
  int exit_val;

public:
  // The implementation and usage of the following methods is documented in src/inner/handler.cpp
  Inner(int, const char**);
  ~Inner();

  void initialize_workers();

  void get_primes();

  void build_output();

  // Methods related to exitting and error-handling
  inline bool get_exit() { return exit; }
  int finish();
};
