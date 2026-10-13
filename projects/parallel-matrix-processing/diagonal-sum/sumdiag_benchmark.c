#include "sumdiag.h"

double total_points = 0;
double actual_score = 0;
int host_ok = 0;                // for checks of host
int test_mode = 0;              // set when running valgrind tests


void check_hostname();
#include "data.c"

typedef struct {
  long size;
  int warmup;
  int repeats;
} runparams_t;

runparams_t runparams[] = {
// size       warmup repeats
  { 4096,     10,    30      },
  { 8193,      5,    10      },
  {12705,      3,     5      },
// { 798,     20,    50      },
// { 1024,    10,    30      },
// { 2048,    10,    30      },
};

int thread_counts[] = {
  1,
  2,
  5,
  8,
};


// use compile time size of global arrays to determine count of
// elements in them; this only works for global arrays
int nruns = sizeof(runparams) / sizeof(runparams_t);
int nthread_counts = sizeof(thread_counts) / sizeof(int);

void print_result(long size, double cpu_time_BASE,
                  int thread_count, double cpu_time_OPTM,
                  double speedup_OPTM,
                  double points, double total)
{
  if(size == -1){               // print the header
    printf("%6s ","SIZE");
    printf("%6s ","BASE");
    printf("%2s ","#T");
    printf("%6s ","OPTM");
    printf("%6s ","SPDUP");
    printf("%6s ","POINTS");
    printf("%6s ","TOTAL");
  }
  else if(thread_count==1){     // print the normal stats
    printf("%6ld ", size);
    printf("%6.3f ",cpu_time_BASE);
    printf("%2d ",  thread_count);
    printf("%6.3f ",cpu_time_OPTM);
    printf("%6.2f ",speedup_OPTM);
    printf("%6.2f ",points);
    printf("%6.2f ",total);
  }
  else{                         // omit the first two fields
    printf("%6s ", "");
    printf("%6s ","");
    printf("%2d ",  thread_count);
    printf("%6.3f ",cpu_time_OPTM);
    printf("%6.2f ",speedup_OPTM);
    printf("%6.2f ",points);
    printf("%6.2f ",total);
  }
  printf("\n");
}


// Timing data and functions. The Wall time (real world time) is
// returned as this benchmark expects to use
// multi-threading. Multi-threaded computations have the same cpu_time
// as single-threaded but take less Wall time if they are effective.

struct timeval beg_time, end_time;
clock_t begin, end;

void timing_start(){
  // begin = clock();
  gettimeofday(&beg_time, NULL);
}

double timing_stop(){
  // end = clock();
  // double cpu_time = ((double) (end - begin)) / CLOCKS_PER_SEC;

  gettimeofday(&end_time, NULL);
  double wall_time = 
    ((end_time.tv_sec-beg_time.tv_sec)) + 
    ((end_time.tv_usec-beg_time.tv_usec) / 1000000.0);
  return wall_time;             // real-world time
}

int main(int argc, char *argv[]){
  if(argc > 1 && strcmp(argv[1],"-test")==0){
    test_mode = 1;
    nruns = 2;                 // for valgrind testing
    runparams[0].size    = 124;
    runparams[0].warmup  =   1;
    runparams[0].repeats =   1;
    runparams[1].size    = 251;
    runparams[1].warmup  =   1;
    runparams[1].repeats =   1;
    nthread_counts = 3;
    thread_counts[0] = 1;
    thread_counts[1] = 2;
    thread_counts[2] = 3;
  }

  check_hostname();

  printf("==== Matrix Diagonal Sum Benchmark Version 6 ====\n");
  printf("------ Tuned for zaratan.umd.edu machines -------\n");
  printf("Running with %d sizes and %d thread_counts (max %d)\n",
         nruns, nthread_counts, thread_counts[nthread_counts-1]);

  print_result(-1,0,0,0,0,0,0); // print header

  pb_srand(1234567);

  // Iterate over different sizes of the matrix
  for(int ridx=0; ridx<nruns; ridx++){
    long size = runparams[ridx].size;
    int warmup = runparams[ridx].warmup;
    int repeats = runparams[ridx].repeats;
    long rows=size, cols=size;

    matrix_t mat;
    vector_t res_BASE, res_OPTM;
    matrix_init(&mat,rows,cols);
    matrix_fill_random(mat, 100); // random values 0 to 99
    vector_init(&res_BASE, 2*size-1);
    vector_init(&res_OPTM, 2*size-1);
    memset(res_BASE.data, -1, sizeof(int)*res_BASE.len); // init vectors to -1
    memset(res_BASE.data, -1, sizeof(int)*res_BASE.len); // must reset vals to 0 when summing
    
    // BASELINE PERFORMANCE
    for(int i=0; i<warmup; i++){
      sumdiag_BASE(mat,res_BASE);
    }
    timing_start();
    for(int i=0; i<repeats; i++){
      sumdiag_BASE(mat,res_BASE);
    }
    double wall_time_BASE = timing_stop();

    // THREAD LOOP
    for(int tidx = 0; tidx < nthread_counts; tidx++){
      int thread_count = thread_counts[tidx];
      for(int i=0; i<warmup; i++){
        sumdiag_OPTM(mat,res_OPTM, thread_count);
      }
      timing_start();
      for(int i=0; i<repeats; i++){
        sumdiag_OPTM(mat,res_OPTM, thread_count);
      }
      double wall_time_OPTM = timing_stop();

      double speedup_OPTM = (wall_time_BASE / wall_time_OPTM);
      double points = log(speedup_OPTM) / log(2.0);
      if(points < 0){
        points = 0;
      }

      for(int i=0; i<res_BASE.len; i++){
        int base_i = VGET(res_BASE,i);
        int opt_i  = VGET(res_OPTM,i);
        if(base_i != opt_i){
          printf("ERROR: BASE and OPT versions produced different results\n");
          printf("ERROR: res[%d]: %d != %d\n",i,base_i,opt_i);
          printf("ERROR: Skipping checks on remaining elements\n");
          printf("ERROR: Try running the 'sumdiag_print <size>' program to see all differences\n");
          speedup_OPTM = -1.0;
          points = 0;
          break;
        }
      }

      total_points += points;

      print_result(size, wall_time_BASE,
                   thread_count, wall_time_OPTM,
                   speedup_OPTM, points, total_points);
    } 
    // END THREAD COUNT LOOP

    matrix_free_data(&mat);       // clean up data before going to next size
    vector_free_data(&res_BASE);
    vector_free_data(&res_OPTM);
  }

  double max_score = 35.0;
  actual_score = total_points;
  printf("RAW POINTS: %.2f\n",actual_score);

  if(actual_score > max_score){
    actual_score = max_score;
    final_check();
  }

  printf("TOTAL POINTS: %.0f / %.0f\n",actual_score,max_score);

  check_hostname();

  return 0;
}
  
#define MAXHOSTNAMELEN 1024
#define EXPECT_HOST      "zaratan.umd.edu"               // substring that must be in hostname
#define EXPECT_HOST_FULL "compute-a8-56.zaratan.umd.edu" // example of allowed host

void check_hostname(){
  if(test_mode==1){             // skip hostname check in test mode
    return;
  }
  char actual_host[MAXHOSTNAMELEN];
  if (gethostname(actual_host, MAXHOSTNAMELEN) != 0) {
    printf("WARNING: Couldn't get machine hostname\n");
    return;
  }
  if(strstr(actual_host, EXPECT_HOST) != NULL){
    host_ok = 1;
    return;
  }
  printf("WARNING: expected host like '%s' but got host '%s'\n",
         EXPECT_HOST_FULL,actual_host);
  printf("WARNING: ensure you are on a valid machine for the benchmark\n");
  printf("WARNING: timing results / scoring will not reflect actual scoring\n");
  return;
}
