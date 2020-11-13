char* read_from_moderator (int read_fd) {
  char size_buffer[1];
  int size_bytes = read(read_fd, size_buffer, 1);

  int msg_size = (int) size_buffer[0];

  char* read_buffer = new char[msg_size];

  int msg_bytes = read(read_fd, read_buffer, msg_size);

  return read_buffer;
}
