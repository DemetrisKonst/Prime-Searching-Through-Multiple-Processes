#include "../../include/worker/handler.h"

int main(int argc, char const *argv[]) {
  Worker handler(argc, argv);

  handler.calculate_primes();

  exit(handler.finish());
}
