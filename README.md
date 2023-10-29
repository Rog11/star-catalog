# star-catalog

The purpose of this program is to optimize the performance of an application
that computes angular distances of stars using multi-threading. The initial step was to obtain a .. baseline time for the unmodified code using a timing method of choice. Next, the application was multi-threaded using pthreads, and the optimal number of threads was determined by calculating the minimum, maximum, and mean angular distances for different thread counts, including 2, 4, 10, 25, 100, and 1000. The report also highlights the addition of a command line parameter, -t

The libraries used for this program apart from the ones already provided are <pthread.h>
and <time.h>

The program uses the clock() function from time.h to measure the execution time of the
program. A start time is set at the beginning of the program with start = clock() and an end
time is set at the end of the program with end = clock(). The difference between these two
times is then used to calculate the execution time with cpu_time_used = ((double) (end -
start)) / CLOCKS_PER_SEC.
