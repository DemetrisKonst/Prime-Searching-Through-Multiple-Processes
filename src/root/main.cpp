#include <signal.h>

#include "../../include/root/handler.h"

int main(int argc, char const *argv[]) {
  Root handler(argc, argv);
  if (handler.get_exit())
    exit(handler.finish());

  handler.initialize_inner();
  if (handler.get_exit())
    exit(handler.finish());

  handler.get_primes();

  handler.build_output();

  exit(handler.finish());
}
