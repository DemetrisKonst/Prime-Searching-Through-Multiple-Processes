PrimeItem read_from_worker (int read_fd) {
  PrimeItem item;

  int msg_bytes = read(read_fd, &item, sizeof(PrimeItem));

  return item;
}

int write_to_main (int write_fd, PrimeItem* item_arr) {
  int batch_size = 10;
  int bytes = write(write_fd, item_arr, sizeof(item_arr)*batch_size);
}
