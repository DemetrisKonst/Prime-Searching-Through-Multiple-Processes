#include <iostream>
#include <cstdlib>
#include <cstring>

int check_arguments (int argc, const char* argv[], int* lower_bound, int* upper_bound, int* parent_number, int* number, int* algorithm, int* write_fd) {
  if (argc != 7) {
    std::cout << "Wrong number of arguments (6 required)" << '\n';
    return 1;
  }

  int l = atoi(argv[1]);
  int u = atoi(argv[2]);
  int pn = atoi(argv[3]);
  int n = atoi(argv[4]);
  int a = atoi(argv[5]);
  int f = atoi(argv[6]);


  *lower_bound = l;
  *upper_bound = u;
  *parent_number = pn;
  *number = n;
  *algorithm = a;
  *write_fd = f;

  return 0;
}
