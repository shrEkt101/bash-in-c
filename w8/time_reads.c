#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>


// Message to print in the signal handling function. 
#define MESSAGE "%ld reads were done in %ld seconds.\n"


/* Global variables to store number of read operations and seconds elapsed.
 * These have to be global so that signal handler to be written will have
 * access.
 */
long num_reads = 0, seconds;


void handle_sigprof(int signum){

  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGPROF);
  sigprocmask(SIG_BLOCK, &set, NULL);  // Block SIGPROF


  fprintf(stderr, MESSAGE, num_reads, seconds);
  fflush(stderr);  // Ensure immediate printing

  exit(0);
}

int main(int argc, char ** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigprof;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPROF, &sa, NULL);
    
    seconds = strtol(argv[1], NULL, 10);

    struct itimerval timer;
    timer.it_value.tv_sec = seconds;  // Set the time after which SIGPROF will be sent
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;  // No repeating interval
    timer.it_interval.tv_usec = 0;

    // Start the timer with ITIMER_PROF
    setitimer(ITIMER_PROF, &timer, NULL);

    FILE *fp;
    if ((fp = fopen(argv[2], "r+")) == NULL) {    // Read+Write for later ...
      perror("fopen");
      exit(1);
    }

    /* In an infinite loop, read an int from a random location in the file
     * and print it to stderr.
     */
    for (;;) {
      int index = rand()%100;
      int val;

      fseek(fp, index * sizeof(int), SEEK_SET);
      fread(&val,sizeof(int), 1, fp);

      fprintf(stderr, "%d\n", val);
      num_reads++;

    }

    return 1;  //something is wrong if we ever get here!
}

