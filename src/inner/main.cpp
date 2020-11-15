#include "../../include/inner/handler.h"

int main(int argc, char const *argv[]) {

  Inner handler(argc, argv);

  handler.initialize_workers();

  handler.get_primes();

  handler.build_output();

  exit(handler.finish());
}
