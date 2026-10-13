// optimized version of MATATA operation
#include "matata.h"

////////////////////////////////////////////////////////////////////////////////
// REQUIRED: Paste a copy of your sumdiag_benchmark from an ODD grace
// node below.
//
// -------REPLACE WITH YOUR RUN + TABLE --------
// 
// grace3>> ./mata_benchmark 
/* 
==== Matrix A^T*A Benchmark Version 2.0 ====
Performing 12 runs with 2 thread_counts (max 2)
  ROWS   COLS   BASE  T   OPTM SPDUP POINT TOTAL
   128    128  0.008  1  0.006  1.24  0.62  0.62 
                      2  0.004  2.03  2.04  2.66 
   171    151  0.016  1  0.009  1.69  1.51  4.18 
                      2  0.007  2.30  2.41  6.58 
   196    180  0.025  1  0.014  1.78  1.67  8.25 
                      2  0.011  2.34  2.45 10.71 
   216    128  0.027  1  0.014  1.90  1.85 12.55 
                      2  0.008  3.16  3.32 15.87 
   256    128  0.033  1  0.017  1.89  1.84 17.71 
                      2  0.010  3.47  3.59 21.30 
   256    256  0.131  1  0.066  1.99  1.98 23.28 
                      2  0.029  4.48  4.33 27.61 
   512    256  0.139  1  0.072  1.93  1.89 29.50 
                      2  0.054  2.58  2.74 32.24 
   512    512  0.619  1  0.280  2.21  2.29 34.52 
                      2  0.212  2.91  3.09 37.61 
   640    512  0.866  1  0.347  2.50  2.64 40.25 
                      2  0.304  2.85  3.02 43.27 
   640    640  1.136  1  0.544  2.09  2.13 45.40 
                      2  0.398  2.85  3.03 48.42 
   768    640  1.365  1  0.643  2.12  2.17 50.60 
                      2  0.510  2.68  2.84 53.44 
   801    800  2.225  1  1.071  2.08  2.11 55.55 
                      2  0.774  2.88  3.05 58.60 
RAW POINTS: 58.60
 _   _                 _   _           _   _       _____ _    ____ _____ _
| \ | | _____      __ | |_| |__   __ _| |_( )___  |  ___/ \  / ___|_   _| |
|  \| |/ _ \ \ /\ / / | __| '_ \ / _` | __|// __| | |_ / _ \ \___ \ | | | |
| |\  | (_) \ V  V /  | |_| | | | (_| | |_  \__ \ |  _/ ___ \ ___) || | |_|
|_| \_|\___/ \_/\_/    \__|_| |_|\__,_|\__| |___/ |_|/_/   \_\____/ |_| (_)

                             ...,?77??!~~~~!???77?<~....
                        ..?7`                           `7!..
                    .,=`          ..~7^`   I                  ?1.
       ........  ..^            ?`  ..?7!1 .               ...??7
      .        .7`        .,777.. .I.    . .!          .,7!
      ..     .?         .^      .l   ?i. . .`       .,^
       b    .!        .= .?7???7~.     .>r .      .=
       .,.?4         , .^         1        `     4...
        J   ^         ,            5       `         ?<.
       .%.7;         .`     .,     .;                   .=.
       .+^ .,       .%      MML     F       .,             ?,
        P   ,,      J      .MMN     F        6               4.
        l    d,    ,       .MMM!   .t        ..               ,,
        ,    JMa..`         MMM`   .         .!                .;
         r   .M#            .M#   .%  .      .~                 .,
       dMMMNJ..!                 .P7!  .>    .         .         ,,
       .WMMMMMm  ?^..       ..,?! ..    ..   ,  Z7`        `?^..  ,,
          ?THB3       ?77?!        .Yr  .   .!   ?,              ?^C
            ?,                   .,^.` .%  .^      5.
              7,          .....?7     .^  ,`        ?.
                `<.                 .= .`'           1
                ....dn... ... ...,7..J=!7,           .,
             ..=     G.,7  ..,o..  .?    J.           F
           .J.  .^ ,,,t  ,^        ?^.  .^  `?~.      F
          r %J. $    5r J             ,r.1      .=.  .%
          r .77=?4.    ``,     l ., 1  .. <.       4.,
          .$..    .X..   .n..  ., J. r .`  J.       `'
        .?`  .5        `` .%   .% .' L.'    t
        ,. ..1JL          .,   J .$.?`      .
                1.          .=` ` .J7??7<.. .;
                 JS..    ..^      L        7.:
                   `> ..       J.  4.
                    +   r `t   r ~=..G.
                    =   $  ,.  J
                    2   r   t  .;
              .,7!  r   t`7~..  j..
              j   7~L...$=.?7r   r ;?1.
               8.      .=    j ..,^   ..
              r        G              .
            .,7,        j,           .>=.
         .J??,  `T....... %             ..
      ..^     <.  ~.    ,.             .D
    .?`        1   L     .7.........?Ti..l
   ,`           L  .    .%    .`!       `j,
 .^             .  ..   .`   .^  .?7!?7+. 1
.`              .  .`..`7.  .^  ,`      .i.;
.7<..........~<<3?7!`    4. r  `          G%
                          J.` .!           %
                            JiJ           .`
                              .1.         J
                                 ?1.     .'
                                     7<..%

       _  ___    ____                        _ _
   _  / |/ _ \  | __ )  ___  _ __  _   _ ___| | |
 _| |_| | | | | |  _ \ / _ \| '_ \| | | / __| | |
|_   _| | |_| | | |_) | (_) | | | | |_| \__ \_|_|
  |_| |_|\___/  |____/ \___/|_| |_|\__,_|___(_|_)

TOTAL POINTS: 45 / 35
*/
// 
// -------REPLACE WITH YOUR RUN + TABLE --------
// 
// NOTE: During grading, staff will independently evaluate your code
// performance on a grace node and take the best score from 3 runs of
// the benchmark.
////////////////////////////////////////////////////////////////////////////////


// You may add helper functions, struct definitions, thread entry
// points, and other items here. Likely you'll do all of that.

// Struct for args for worker method. Attributes: Given matrix, 
// answer matrix, start and end col index for each thread, phase to
// indicate weather to compute or normalize/mirror, chunk max, max
// between all threads.
typedef struct {
  matrix_t mat;
  matrix_t ans;
  long start_index;
  long end_index;
  int phase;
  float chunk_max;
  float global_max;
} worker_args_t;

// thread worker
static void *matata_worker(void *args){
  worker_args_t *a = (worker_args_t *)args;

  long rows = a->mat.rows;
  long cols = a->mat.cols;

  float *mat_data = a->mat.data;
  float *ans_data = a->ans.data;

  if(a->phase == 0){
    // compute upper triangle
    // each thread computes its own columns in the given matrix
    for(long r=0; r<rows; r++){
      float *mat_row = mat_data + r*cols;

      for(long i=a->start_index; i<a->end_index; i++){
        float curr_val = mat_row[i];
        float *ans_row = ans_data + i*cols;

        for(long j=i; j<cols; j++){
          ans_row[j] += curr_val * mat_row[j];
        }
      }
    }

    // compute local max of the upper triangle
    float lmax = 0.0f;
    for(long i=a->start_index; i<a->end_index; i++){
      float *ans_row = ans_data + i*cols;

      for(long j=i; j<cols; j++){
        float abs_val = fabsf(ans_row[j]);

        if(abs_val > lmax) {
          lmax = abs_val;
        }
      }
    }

    // set local max to chunk of columns computed by thread
    a->chunk_max = lmax;
  } else{
    // normalize and copy upper mat to lower
    float gmax = a->global_max;

    // iterate and mirror into lower triangle to normalized values
    for(long i=a->start_index; i<a->end_index; i++){
      float *ans_row = ans_data + i*cols;

      for(long j=i; j<cols; j++){
        float val = ans_row[j] / gmax;
        ans_row[j] = val;
        ans_data[j*cols + i] = val;
      }
    }
  }

  return NULL;
}


// Required function for MATATA operation
int matata_OPTM(matrix_t mat, matrix_t ans, int thread_count) {
  // YOUR CODE HERE
  long cols = mat.cols;

  // cler output each call for square ans matrix
  memset(ans.data, 0, sizeof(float) * cols * cols);

  // set up threads and args
  pthread_t *tids = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
  worker_args_t *args = (worker_args_t *)malloc(sizeof(worker_args_t) * thread_count);

  // number of cols each thread will process
  long chunk = (cols + thread_count - 1) / thread_count;

  // calculat uper triangle
  // iterate thro thread count
  for(int t=0; t<thread_count; t++){
    long start_index = (long)t * chunk;
    long end_index = start_index + chunk;

    if(end_index > cols) {
      end_index = cols;
    }
    
    // Set arguments for worker method
    args[t].mat = mat;
    args[t].ans = ans;
    args[t].start_index = start_index;
    args[t].end_index = end_index;
    args[t].phase = 0;
    args[t].chunk_max = 0.0f;
    args[t].global_max = 0.0f;

    if(thread_count == 1){
      // Threads not required
      matata_worker(&args[t]);
    }
    else{
      // Create the thread
      pthread_create(&tids[t], NULL, matata_worker, &args[t]);
    }
  }

  // Wait for all threads to finish
  if(thread_count != 1){
    for(int t=0; t<thread_count; t++){
      pthread_join(tids[t], NULL);
    }
  }

  // Set the global max for upper trianlge
  float gmax = 0.0f;
  for(int t=0; t<thread_count; t++){
    if(args[t].chunk_max > gmax){
      gmax = args[t].chunk_max;
    }
  }

  // Mirror the upper triangle to lower triangle using threads
  for(int t=0; t<thread_count; t++){
    args[t].phase = 1;
    args[t].global_max = gmax;

    if(thread_count == 1){
      matata_worker(&args[t]);
    }
    else{
      pthread_create(&tids[t], NULL, matata_worker, &args[t]);
    }
  }

  // Wait for threads to finish
  if(thread_count != 1){
    for(int t=0; t<thread_count; t++){
      pthread_join(tids[t], NULL);
    }
  }

  free(tids);
  free(args);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// REQUIRED: DON'T FORGET TO PASTE YOUR TIMING RESULTS FOR
// sumdiag_benchmark FROM A GRACE NODE AT THE TOP OF THIS FILE
////////////////////////////////////////////////////////////////////////////////
