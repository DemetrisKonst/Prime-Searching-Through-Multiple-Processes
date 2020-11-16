#include "../../include/inner/handler.h"

int main(int argc, char const *argv[]) {
  Inner handler(argc, argv);
  if (handler.get_exit())
    exit(handler.finish());

  handler.initialize_workers();
  if (handler.get_exit())
    exit(handler.finish());
    
  handler.get_primes();

  handler.build_output();

  exit(handler.finish());
}
