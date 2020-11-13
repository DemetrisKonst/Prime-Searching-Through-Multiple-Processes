int write_to_moderator (int write_fd, PrimeItem item) {
  int bytes = write(write_fd, &item, sizeof(item));
  std::cout << "WRITTEN: " << bytes << '\n';

  return 0;
}
