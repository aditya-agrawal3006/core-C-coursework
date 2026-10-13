#include "matata.h"

#define PADSIZE (1<<20)         // 1 MB padding

double total_points = 0;
double actual_score = 0;
double max_score = 35.0;
int host_ok = 0;
void check_hostname();
#include "data.c"

// int REPEATS = 2;               // repetitions to average
// int WARMUP  = 20;               // warmup iterations to warm cache

typedef struct {
  long rows;
  long cols;
  int warmup;
  int repeats;

} runparams_t;

runparams_t runparams[] = {
// rows cols warmup repeats
  {128, 128, 20,    5      },
  {171, 151, 20,    5      },
  {196, 180, 20,    5      },
  {216, 128, 20,    5      },
  {256, 128, 20,    5      },
  {256, 256, 15,    5      },
  {512, 256, 10,    5      },
  {512, 512, 10,    5      },
  {640, 512,  5,    5      },
  {640, 640,  5,    5      },
  {768, 640,  5,    5      },
  {801, 800,  5,    5      },
};

int thread_counts[] = {
  1,
  2,
  // 3,
  // 4,
  // 8,
};

// use compile time size of global arrays to determine count of
// elements in them; this only works for global arrays
int nruns = sizeof(runparams) / sizeof(runparams_t);
int nthread_counts = sizeof(thread_counts) / sizeof(int);

void print_result(long rows, long cols,
                  double cpu_time_BASE, int thread_count,
                  double cpu_time_OPTM, double speedup_OPTM,
                  double points, double total)
{
  if(rows == -1){               // print the header
    printf("%6s ","ROWS");
    printf("%6s ","COLS");
    printf("%6s ","BASE");
    printf("%2s ","T");
    printf("%6s ","OPTM");
    printf("%5s ","SPDUP");
    printf("%5s ","POINT");
    printf("%5s ","TOTAL");
  }
  else if(thread_count==1){     // print the normal stats
    printf("%6ld ", rows);
    printf("%6ld ", cols);
    printf("%6.3f ",cpu_time_BASE);
    printf("%2d ",  thread_count);
    printf("%6.3f ",cpu_time_OPTM);
    printf("%5.2f ",speedup_OPTM);
    printf("%5.2f ",points);
    printf("%5.2f ",total);
  }
  else{                         // omit the first two fields
    printf("%6s ", "");
    printf("%6s ", "");
    printf("%6s ","");
    printf("%2d ",  thread_count);
    printf("%6.3f ",cpu_time_OPTM);
    printf("%5.2f ",speedup_OPTM);
    printf("%5.2f ",points);
    printf("%5.2f ",total);
  }
  printf("\n");
}

// Timing data and functions. The Wall time (real world time) is
// returned as this benchmark expects to use
// multi-threading. Multi-threaded computations have the same cpu_time
// as single-threaded but take less Wall time if they are effective.
struct timeval beg_time, end_time;

void timing_start(){
  gettimeofday(&beg_time, NULL);
}

double timing_stop(){
  gettimeofday(&end_time, NULL);
  double wall_time = 
    ((end_time.tv_sec-beg_time.tv_sec)) + 
    ((end_time.tv_usec-beg_time.tv_usec) / 1000000.0);
  return wall_time;             // real-world time
}


int test_mode = 0;              // set when running valgrind tests

int main(int argc, char *argv[]){
  if(argc > 1 && strcmp(argv[1],"-test")==0){
    nruns = 2;                  // for valgrind testing
    test_mode = 1;
  }

  check_hostname();

  printf("==== Matrix A^T*A Benchmark Version 2.0 ====\n");
  printf("Performing %d runs with %d thread_counts (max %d)\n",
         nruns, nthread_counts, thread_counts[nthread_counts-1]);


  print_result(-1,0,0,0,0,0,0,0);  // print header
  pb_srand(1234567);

  // Iterate over different sizes of the matrix
  for(int ridx=0; ridx<nruns; ridx++){
    long rows = runparams[ridx].rows;
    long cols = runparams[ridx].cols;
    int warmup = runparams[ridx].warmup;
    int repeats = runparams[ridx].repeats;

    matrix_t base_mat, base_ans;
    matrix_t optm_mat, optm_ans;

    int init = 0;
    init = init || matrix_init(&base_mat,rows,cols);
    init = init || matrix_init(&base_ans,cols,cols);
    init = init || matrix_init(&optm_mat,rows,cols);
    init = init || matrix_init(&optm_ans,cols,cols);

    if(init){
      printf("\nERROR: failure to initialize at rows/cols %ld/%ld\n",
             rows,cols);
      printf("ABORTING\n");
      exit(EXIT_FAILURE);
    }

    matrix_fill_random(base_mat,-10,+10);
    matrix_fill_random(base_ans,-10,+10);
    matrix_copy(&base_mat, &optm_mat);
    matrix_copy(&base_ans, &optm_ans);

    // BASELINE PERFORMANCE
    for(int i=0; i<warmup; i++){
      matata_BASE(base_mat,base_ans);
    }
    timing_start();
    for(int i=0; i<repeats; i++){
      int ret = matata_BASE(base_mat,base_ans);
      if(ret){
        printf("\nERROR: failure on matata_BASE() at rows/cols %ld/%ld\n",
               rows,cols);
        printf("ABORTING\n");
        exit(EXIT_FAILURE);
      }        
    }
    double wall_time_BASE = timing_stop();

    // OPTIM PERFORMANCE THREAD LOOP
    for(int tidx = 0; tidx < nthread_counts; tidx++){
      int thread_count = thread_counts[tidx];
      for(int i=0; i<warmup; i++){
        matata_OPTM(optm_mat,optm_ans,thread_count);
      }
      timing_start();
      for(int i=0; i<repeats; i++){
        int ret = matata_OPTM(optm_mat,optm_ans,thread_count);
        if(ret){
          printf("\nERROR: failure on matata_OPTM() at rows/cols %ld/%ld\n",
                 rows,cols);
          printf("ABORTING\n");
          exit(EXIT_FAILURE);
        }        
      }
      double wall_time_OPTM = timing_stop();

      double speedup_OPTM = (wall_time_BASE / wall_time_OPTM);
      double points = log(speedup_OPTM) / log(2.0) * 2; 
      if(points < 0){
        points = 0;
      }

      int mismatch = 0;
      for(int i=0; i<base_ans.rows; i++){
        for(int j=0; j<base_ans.cols; j++){
          float base_ij = MGET(base_ans,i,j);
          float optm_ij = MGET(optm_ans,i,j);
          float diff = fabsf(base_ij - optm_ij);
          if(diff > DIFFTOL){
            printf("ERROR: base and optm results differ\n");
            printf("ERROR: base_ans[%d][%d] = %f\n",i,j,base_ij);
            printf("ERROR: optm_ans[%d][%d] = %f\n",i,j,optm_ij);
            printf("ERROR: diff = %e\n",diff);
            printf("ERROR: Skipping checks on remaining elements\n");
            printf("ERROR: Try running the 'matata_print <rows> <cols>' program to see differences\n");
            speedup_OPTM = -1.0;
            points = 0;
            mismatch = 1;
            break;
          }
        }
        if(mismatch==1){
          break;
        }
      }

      total_points += points;

      print_result(rows,cols, wall_time_BASE,
                   thread_count, wall_time_OPTM,
                   speedup_OPTM, points, total_points);
    }
    
    matrix_free_data(&base_mat);       // clean up data before starting next loop iteration
    matrix_free_data(&base_ans);
    matrix_free_data(&optm_mat);
    matrix_free_data(&optm_ans);
  }

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
#define FULL_EXPECT_HOST "grace9.umd.edu" // full expected host name
char *allowed_hosts[] = {
  "grace3.umd.edu",
  "grace4.umd.edu",
  "grace5.umd.edu",
  "grace6.umd.edu",
  "grace7.umd.edu",
  "grace8.umd.edu",
  "grace9.umd.edu",
  "grace10.umd.edu",
  NULL
};

void check_hostname(){
  if(test_mode==1){             // skip hostname check in test mode
    return;
  }
  char actual_host[MAXHOSTNAMELEN];
  if (gethostname(actual_host, MAXHOSTNAMELEN) != 0) {
    printf("WARNING: Couldn't get machine hostname\n");
    return;
  }
  for(int i=0; allowed_hosts[i]!=NULL; i++){
    if(strcmp(allowed_hosts[i], actual_host) == 0){
      host_ok = 1;
      return;
    }
  }
  printf("WARNING: UNEXPECTED HOST '%s'\n",actual_host);
  printf("WARNING: ensure you are on an normal GRACE node\n");
  printf("WARNING: timing results / scoring will not reflect actual scoring\n");
  printf("WARNING: run on one of the following hosts for accurate results\n");
  for(int i=0; allowed_hosts[i]!=NULL; i++){
    printf("WARNING:   %s\n",allowed_hosts[i]);
  }
  // printf("WARNING: while on grace, try `ssh grace5` to log into a specifc node\n");
  return;
}

