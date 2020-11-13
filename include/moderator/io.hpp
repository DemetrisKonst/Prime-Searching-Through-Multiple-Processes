PrimeItem* read_from_worker (int read_fd) {
  PrimeItem* item = new PrimeItem;

  int msg_bytes = read(read_fd, item, sizeof(PrimeItem));
  
  return item;
}

int write_to_main (int write_fd, char* message) {
  int msg_size = strlen(message) + 1;
  char* f_msg = new char[msg_size+1];

  f_msg[0] = (char) msg_size;
  f_msg[1] = '\0';

  strcat(f_msg, message);

  write(write_fd, f_msg, strlen(f_msg)+1);

  return 0;
}
