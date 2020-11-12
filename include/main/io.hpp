char* read_from_moderator (int read_fd, int* status_code) {
  char size_buffer[1];
  int size_bytes = read(read_fd, size_buffer, 1);

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
