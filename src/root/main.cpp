#include "../../include/root/handler.h"

int main(int argc, char const *argv[]) {

  Root handler(argc, argv);

  handler.initialize_inner();

  handler.get_primes();

  handler.build_output();

  exit(handler.finish());
}
