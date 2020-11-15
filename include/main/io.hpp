#include "../core/prime_item.hpp"

PrimeItem* read_from_moderator (int read_fd) {
  int batch_size = 10;

  PrimeItem* item_arr = new PrimeItem[batch_size];

  int msg_bytes = read(read_fd, item_arr, sizeof(PrimeItem)*batch_size);

  return item_arr;
}
