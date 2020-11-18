This project's aim is to calculate all prime numbers in a given range using multiple
processes.


----PROCESS STRUCTURE----


-ROOT: Is the beginning of the program. Arguments given are "lower bound" (-l), "upper bound" (-u) and
"number of children processes" (-w). This process creates 'w' child processes and distributes the
number range evenly among them. When the prime calculations are over, it combines the data and
outputs them to the user.

-INNER: Are the intermediate step of the program, these are the processes created by ROOT. Each one
of them has a different sub-range of the range given as an argument to ROOT. Once initialized, they
create 'w' child processes (each) and distribute the range among those child processes even further.
They combine the prime calculations done by their children and through a pipe, send the data to ROOT.

-WORKER: These are the final "layer" of processes in our program. They are assigned a certain range
of numbers by their parent (INNER) process and are "given" a specific prime-finding algorithm.
For each number in the range they evaluate (through the algorithm) whether that number is a prime or not.
Whenever they find a prime, they send it to their parent using a pipe.

To sum up, ROOT creates 'w' INNERs and those INNERs create 'w' WORKERs. These WORKERs find primes
in their given range and send them to their parent. The parent (once all WORKERs have finished)
sends all primes to ROOT which finally combines the results. Each WORKER also sends the total time
taken to calculate all primes in the range. In addition to that, whenever a WORKER finishes, it
sends a SIGUSR1 signal to the ROOT. The ROOT then "catches" this signal and outputs the number of
signals received.


----CODE STRUCTURE----


All processes have a similar code structure. Their "main" function serves only to invoke a "handler"
object which takes care of everything. Hence, we will not delve deep into "main" but in "handler".
Each handler shares similarities with the others but they differ due to the various requirements
for each process.

-ROOT: This handler has 4 jobs. First it needs to validate the user input. Once user-input is
validated, it creates 'w' INNER child processes and their respective communication pipes.
Afterwards, it simply awaits for any message sent through an INNER node to combine all primes.
These primes are stored in a Priority Queue to remain sorted. On a sidenote, it is also responsible
for handling signals through the usage of "sigaction".

-INNER: Similar to ROOT-handler, it creates 'w' WORKER child processes
and the pipes used for communication between them. It reads every message passed by
each children through its communication pipe and adds the prime in a Priority Queue. Once children
have finished (i.e. when they send the total time taken for all calculations), it sends the Priority
Queue in batches to ROOT.

-WORKER: This handler is the most simple of them all. All it does is
calculate primes in its given range. For every prime found it sends a message to its parent
through the communication pipe (given as argument). When the calculations end, it sends the total
time it took to calculate all these primes. Finally, once it is ready to exit, it sends a SIGUSR1
to ROOT.


----DIRECTORY STRUCTURE----


-config:
  -defn.h: Defines all macros used by the program

-include: Contains all header (.h) files used by the program
  -core:
    -algorithm.h: Contains implementations for 3 different prime-checking algorithms
    -prime_item.h: Contains a PrimeItem structure which is the core item used by the
      program to indicate primes and pass them among different processes through pipes
  -root:
    -handler.h: Contains the declaration of the ROOT-handler class
  -inner:
    -handler.h: Contains the declaration of the INNER-handler class
  -worker:
    -handler.h: Contains the declaration of the WORKER-handler class
  -utils: Contains utility functions/objects
    -structures.h: Contains the implementation of a Priority Queue of prime items
    -utils.h: Contains a function which distributes a range evenly among 'w' children

-src: Contains all source (.cpp) files used by the program
  -root: Contains handler.cpp and main.cpp of ROOT
    -handler.cpp: Implements the ROOT-handler
    -main.cpp: Uses ROOT-handler to execute the program, this is where ROOT-main resides
  -inner: Contains handler.cpp and main.cpp of INNER
    -handler.cpp: Implements the INNER-handler
    -main.cpp: Uses INNER-handler to execute the program, this is where INNER-main resides
  -worker: Contains handler.cpp and main.cpp of WORKER
    -handler.cpp: Implements the WORKER-handler
    -main.cpp: Uses WORKER-handler to execute the program, this is where WORKER-main resides

-object: Destination of all object (.o) files during compilation

-bin: Destination of all binary (executable) files during compilation


----USAGE----


-Compilation:
The compilation uses the -O3 flag, it can be modified inside the Makefile by changing CFLAGS
  -Command: make "or" make ALL

-Execution:
You must be inside the project directory (important as execution through exec uses a relative
path to find the process' executable)
  -Command: bin/myprime --arguments

  -Arguments:
    -l [lower_bound as integer] REQUIRED
    -u [upper_bound as integer] REQUIRED
    -w [amount of children processes as integer] REQUIRED


----SIGNALS----
At the first stages of development of this program, signal() was used to "catch"
signals sent by WORKERs, this resulted in almost every execution at least one signal was missed.
To somewhat correct this behavior, it was replaced by sigaction(). While using sigaction did
improve the results, there is still a high chance a signal is not caught by the ROOT.
