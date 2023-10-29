#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <stdint.h>
#include <pthread.h>
#include "utility.h"
#include "star.h"
#include "float.h"

#define NUM_STARS 30000 
#define MAX_LINE 1024
#define DELIMITER " \t\n"
#define MAX_THREADS 4
// #define THREAD_NUM 2

pthread_mutex_t mutex; 

clock_t start, end;
double cpu_time_used;


double mean = 0;


struct Star star_array[ NUM_STARS ];
uint8_t   (*distance_calculated)[NUM_STARS];

double  min  = FLT_MAX;
double  max  = FLT_MIN;

pthread_mutex_t min_mutex;
pthread_mutex_t max_mutex;
pthread_t threads[1000];
int num_threads = 0;

void showHelp()
{
  printf("Use: findAngular [options]\n");
  printf("Where options are:\n");
  printf("-t          Number of threads to use\n");
  printf("-h          Show this help\n");
}


// Embarassingly inefficient, intentionally bad method
// to calculate all entries one another to determine the
// average angular separation between any two stars 
void* determineAverageAngularDistance( void *arg )
{
    uint32_t i, j;
    uint64_t count = 0;
    int thread_id = (int)arg;

    int chunk_size = NUM_STARS / num_threads;
    int start = thread_id * chunk_size;
    int finish = start + chunk_size;

    for (i = start; i < finish; i++)
    {
      for (j = 0; j < NUM_STARS; j++)
      {
        if( i!=j && distance_calculated[i][j] == 0 )
        {
          double distance = calculateAngularDistance( star_array[i].RightAscension, star_array[i].Declination,
                                                      star_array[j].RightAscension, star_array[j].Declination ) ;
          distance_calculated[i][j] = 1;
          distance_calculated[j][i] = 1;
          count++;
          if( min > distance )
          {
            min = distance;
          }
          
          if( max < distance )
          {
            max = distance;
          }
          mean = mean + (distance-mean)/count;

        }
      }
    }
    pthread_exit(NULL);
}

int main( int argc, char * argv[] )
{

  FILE *fp;
  uint32_t star_count = 0;

  if (argc <= 2)
  {
    perror("Incorrect usage, input -h for help");
    exit(0);
  }

  if (strcmp(argv[1], "-t") == 0)
  {
    num_threads = atoi(argv[2]);
  }


  uint32_t n;

  distance_calculated = malloc(sizeof(uint8_t[NUM_STARS][NUM_STARS]));

  if( distance_calculated == NULL )
  {
    uint64_t num_stars = NUM_STARS;
    uint64_t size = num_stars * num_stars * sizeof(uint8_t);
    printf("Could not allocate %ld bytes\n", size);
    exit( EXIT_FAILURE );
  }

  int i, j;
  // default every thing to 0 so we calculated the distance.
  // This is really inefficient and should be replace by a memset
  memset(distance_calculated, 0, sizeof(distance_calculated));

  for( n = 1; n < argc; n++ )          
  {
    if( strcmp(argv[n], "-help" ) == 0 )
    {
      showHelp();
      exit(0);
    }
  }

  fp = fopen( "data/tycho-trimmed.csv", "r" );

  if( fp == NULL )
  {
    printf("ERROR: Unable to open the file data/tycho-trimmed.csv\n");
    exit(1);
  }

  char line[MAX_LINE];
  while (fgets(line, 1024, fp))
  {
    uint32_t column = 0;

    char* tok;
    for (tok = strtok(line, " ");
            tok && *tok;
            tok = strtok(NULL, " "))
    {
       switch( column )
       {
          case 0:
              star_array[star_count].ID = atoi(tok);
              break;
       
          case 1:
              star_array[star_count].RightAscension = atof(tok);
              break;
       
          case 2:
              star_array[star_count].Declination = atof(tok);
              break;

          default: 
             printf("ERROR: line %d had more than 3 columns\n", star_count );
             exit(1);
             break;
       }
       column++;
    }
    star_count++;
  }
  printf("%d records read\n", star_count );
  pthread_mutex_init(&mutex, NULL);
  start = clock();
  for (int i = 0; i < num_threads; i++) 
  {
    pthread_create(&threads[i], NULL, determineAverageAngularDistance, (void *) i);
  }

  for (int i = 0; i < num_threads; i++)
  {
    pthread_join(threads[i], NULL);
  }
  end = clock();
  pthread_mutex_destroy(&mutex);
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Average distance found is %lf\n", mean );
  printf("Minimum distance found is %lf\n", min );
  printf("Maximum distance found is %lf\n", max );
  printf("total time: %lf\n", cpu_time_used);

  return 0;
}
