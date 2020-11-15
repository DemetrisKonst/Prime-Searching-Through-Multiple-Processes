long int** distribute_range(long int lower_bound, long int upper_bound, int elements) {
  long int** boundaries = new long int*[elements];

  int distribution_range = (upper_bound - lower_bound) / elements;
  int range_remainder = (upper_bound - lower_bound) % elements;
  int remainders_added = 0;

  for (int i = 0; i < elements; i++) {
    boundaries[i] = new long int[2];

    long int element_lower_bound = lower_bound + remainders_added + (i*distribution_range);
    long int element_upper_bound = element_lower_bound + distribution_range - 1;

    if (remainders_added <= range_remainder) {
      element_upper_bound++;
      remainders_added++;
    }

    boundaries[i][0] = element_lower_bound;
    boundaries[i][1] = element_upper_bound;
  }

  return boundaries;
}
