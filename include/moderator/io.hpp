char* read_from_worker (int read_fd, int* status_code) {
  char size_buffer[1];
  int size_bytes = read(read_fd, size_buffer, 1);

  // std::cout << "BYTES: " << size_bytes << '\n';

  if (size_bytes == -1) {
    if (errno == EAGAIN){
      *status_code = 1;
      return NULL;
    }else{
      *status_code = 2;
      return NULL;
    }
  }else if (size_bytes == 0){
    *status_code = 3;
    return NULL;
  }else{
    int msg_size = (int) size_buffer[0];

    char* read_buffer = new char[msg_size];

    int msg_bytes = read(read_fd, read_buffer, msg_size);

    *status_code = 0;
    return read_buffer;
  }
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
