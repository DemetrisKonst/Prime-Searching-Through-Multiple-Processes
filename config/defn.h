/*
Size in bytes for a string representing a long or an int
Used in argument passing through exec*() calls
*/
#define LONG_STR_SIZE 19
#define INT_STR_SIZE 10

// Macros used in handlers to represent error-type
#define USER_ERROR 1
#define SYSTEM_ERROR 2

/*
Macro used to determine batch size for passing primes from INNER to ROOT
Must be >= num_of_children
*/
#define PIPE_BATCH_SIZE 10

/*
Macro used for the amount of milliseconds that poll() should block waiting
for a pipe to become "ready".
Preferably set it to a negative value to wait indefinitely
(as timing of execution may vary significantly)
*/
#define POLL_TIMEOUT -1
