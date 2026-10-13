// optimized versions of matrix diagonal summing
#include "sumdiag.h"

////////////////////////////////////////////////////////////////////////////////
// REQUIRED: Paste a copy of your sumdiag_benchmark from Zaratan
// below.
//
// -------REPLACE WITH YOUR RUN + TABLE --------
//
// user@login-1 [p5-code]% make benchmark
// timeout --foreground 60s ./sumdiag_benchmark
// ==== Matrix Diagonal Sum Benchmark Version 6 ====
// ------ Tuned for zaratan.umd.edu machines -------
// Running with 3 sizes and 4 thread_counts (max 8)
//   SIZE   BASE #T   OPTM  SPDUP POINTS  TOTAL 
//   4096  5.281  1  0.414  12.76   3.67   3.67 
//                2  0.212  24.89   4.64   8.31 
//                5  0.092  57.25   5.84  14.15 
//                8  0.089  59.59   5.90  20.05 
//   8193  4.122  1  0.552   7.47   2.90  22.95 
//                2  0.280  14.71   3.88  26.83 
//                5  0.124  33.18   5.05  31.88 
//                8  0.119  34.66   5.12  36.99 
//  12705  2.777  1  0.657   4.23   2.08  39.07 
//                2  0.331   8.38   3.07  42.14 
//                5  0.135  20.57   4.36  46.50 
//                8  0.093  29.75   4.89  51.40 
// RAW POINTS: 51.40
//  _   _                 _   _           _   _       _____ _    ____ _____ _ 
// | \ | | _____      __ | |_| |__   __ _| |_( )___  |  ___/ \  / ___|_   _| |
// |  \| |/ _ \ \ /\ / / | __| '_ \ / _` | __|// __| | |_ / _ \ \___ \ | | | |
// | |\  | (_) \ V  V /  | |_| | | | (_| | |_  \__ \ |  _/ ___ \ ___) || | |_|
// |_| \_|\___/ \_/\_/    \__|_| |_|\__,_|\__| |___/ |_|/_/   \_\____/ |_| (_)

//                              ...,?77??!~~~~!???77?<~.... 
//                         ..?7`                           `7!.. 
//                     .,=`          ..~7^`   I                  ?1. 
//        ........  ..^            ?`  ..?7!1 .               ...??7 
//       .        .7`        .,777.. .I.    . .!          .,7! 
//       ..     .?         .^      .l   ?i. . .`       .,^ 
//        b    .!        .= .?7???7~.     .>r .      .= 
//        .,.?4         , .^         1        `     4... 
//         J   ^         ,            5       `         ?<. 
//        .%.7;         .`     .,     .;                   .=. 
//        .+^ .,       .%      MML     F       .,             ?, 
//         P   ,,      J      .MMN     F        6               4. 
//         l    d,    ,       .MMM!   .t        ..               ,, 
//         ,    JMa..`         MMM`   .         .!                .; 
//          r   .M#            .M#   .%  .      .~                 ., 
//        dMMMNJ..!                 .P7!  .>    .         .         ,, 
//        .WMMMMMm  ?^..       ..,?! ..    ..   ,  Z7`        `?^..  ,, 
//           ?THB3       ?77?!        .Yr  .   .!   ?,              ?^C 
//             ?,                   .,^.` .%  .^      5. 
//               7,          .....?7     .^  ,`        ?. 
//                 `<.                 .= .`'           1 
//                 ....dn... ... ...,7..J=!7,           ., 
//              ..=     G.,7  ..,o..  .?    J.           F 
//            .J.  .^ ,,,t  ,^        ?^.  .^  `?~.      F 
//           r %J. $    5r J             ,r.1      .=.  .% 
//           r .77=?4.    ``,     l ., 1  .. <.       4., 
//           .$..    .X..   .n..  ., J. r .`  J.       `' 
//         .?`  .5        `` .%   .% .' L.'    t 
//         ,. ..1JL          .,   J .$.?`      . 
//                 1.          .=` ` .J7??7<.. .; 
//                  JS..    ..^      L        7.: 
//                    `> ..       J.  4. 
//                     +   r `t   r ~=..G. 
//                     =   $  ,.  J 
//                     2   r   t  .; 
//               .,7!  r   t`7~..  j.. 
//               j   7~L...$=.?7r   r ;?1. 
//                8.      .=    j ..,^   .. 
//               r        G              . 
//             .,7,        j,           .>=. 
//          .J??,  `T....... %             .. 
//       ..^     <.  ~.    ,.             .D 
//     .?`        1   L     .7.........?Ti..l 
//    ,`           L  .    .%    .`!       `j, 
//  .^             .  ..   .`   .^  .?7!?7+. 1 
// .`              .  .`..`7.  .^  ,`      .i.; 
// .7<..........~<<3?7!`    4. r  `          G% 
//                           J.` .!           % 
//                             JiJ           .` 
//                               .1.         J 
//                                  ?1.     .'         
//                                      7<..%

//        _  ___    ____                        _ _ 
//    _  / |/ _ \  | __ )  ___  _ __  _   _ ___| | |
//  _| |_| | | | | |  _ \ / _ \| '_ \| | | / __| | |
// |_   _| | |_| | | |_) | (_) | | | | |_| \__ \_|_|
//   |_| |_|\___/  |____/ \___/|_| |_|\__,_|___(_|_)
// |
// TOTAL POINTS: 45 / 35


// You can write several different versions of your optimized function
// in this file and call one of them in the last function.

// Struct for args for thread
typedef struct {
  int tid;
  int t_count;
  matrix_t mat;
  vector_t vec;
  pthread_mutex_t *lock;
} sumdiag_t;

void *sumdiag_worker(void *arg);



int sumdiag_VER1(matrix_t mat, vector_t vec, int thread_count) {

  // Set sum vec to 0s
  for(int i = 0; i < vec.len; i++){
    vec.data[i] = 0;
  }

  // Make arrays to hold threads and their args
  pthread_t *threads = malloc(thread_count * sizeof(pthread_t));
  sumdiag_t *thread_args = malloc(thread_count * sizeof(sumdiag_t));

  // Initialize lock for vectors
  pthread_mutex_t lock;
  pthread_mutex_init(&lock, NULL);

  // Create each worker thread
  for(int t = 0; t < thread_count; t++){
    thread_args[t].tid = t;
    thread_args[t].t_count = thread_count;
    thread_args[t].mat = mat;
    thread_args[t].vec = vec;
    thread_args[t].lock = &lock;

    pthread_create(&threads[t], NULL, sumdiag_worker, &thread_args[t]);
  }

  // Wait for threads to finish
  for(int t = 0; t < thread_count; t++){
    pthread_join(threads[t], NULL);
  }

  // release lock
  pthread_mutex_destroy(&lock);

  // free memory
  free(threads);
  free(thread_args);

  return 0;
}

int sumdiag_VER2(matrix_t mat, vector_t vec, int thread_count) {
  // OPTIMIZED CODE HERE
  return 0;
}

int sumdiag_OPTM(matrix_t mat, vector_t vec, int thread_count) {
  // call your preferred version of the function
  return sumdiag_VER1(mat, vec, thread_count);
}

// Worker mehtod for each of the threads
// Each thread will do its own rows and find diagonal sum for the rows assigned to it
void *sumdiag_worker(void *arg){
  sumdiag_t *thread_arg = arg;

  matrix_t mat = thread_arg->mat;
  vector_t vec = thread_arg->vec;

  // Attributes of mat and vec
  long rows = mat.rows;
  long cols = mat.cols;
  long vlen = vec.len;

  // Divide rows using chunks to threads
  int chunk = rows / thread_arg->t_count;

  int beg = thread_arg->tid * chunk;
  int end = beg + chunk;

  // Last thread takes remaining
  if(thread_arg->tid == thread_arg->t_count - 1){
    end = rows;
  }

  // Stores sums locally on heap for each thread
  int *local_sums = calloc(vlen, sizeof(int));
  if(local_sums == NULL){
    return NULL;
  }

  // Traverse the matrix by rows
  for(int r = beg; r < end; r++){

    // Pointer to the first element of row r
    int *row_data = mat.data + (r * cols);

    // First diag num in this row
    int diag = rows - 1 - r;

    // go thro the row
    for(int c = 0; c < cols; c++){

      // Add matrix value to diag sum
      local_sums[diag] += row_data[c];

      // move to next diag
      diag++;
    }
  }

  // Add each thread's sum to the vector and lock the vector when doing that
  pthread_mutex_lock(thread_arg->lock);

  for(long i = 0; i < vlen; i++){
    vec.data[i] = vec.data[i] + local_sums[i];
  }

  pthread_mutex_unlock(thread_arg->lock);

  free(local_sums);
  return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// REQUIRED: DON'T FORGET TO PASTE YOUR TIMING RESULTS FOR
// sumdiag_benchmark FROM ZARATAN AT THE TOP OF THIS FILE
////////////////////////////////////////////////////////////////////////////////
