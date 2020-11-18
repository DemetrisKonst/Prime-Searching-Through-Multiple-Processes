#include <cstdlib>
#include "../../include/worker/handler.h"

int main(int argc, char const *argv[]) {
  Worker handler(argc, argv);
  if (handler.get_exit())
    exit(handler.finish());

  handler.calculate_primes();

  exit(handler.finish());
}
