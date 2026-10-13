#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// #include <math.h>       // for fmod() in util

// #define eprintf(...) fprintf (stderr, __VA_ARGS__)


// compile time constants for sizes
#define BUFSIZE 1024            // size of read/write buffers
#define ARG_MAX 255             // max number of arguments
#define MAX_LINE 1024           // maximum length of input lines
#define MAX_JOBS 256            // maximum number of active commands in shellac


// // specific code for certain failure types
// #define FAIL_EXEC 128           
// #define FAIL_OUTP 129           // numeric code indicating a failure due to output redirection
// #define FAIL_INPT 130           // numeric code indicating a failure due to input redirection

// numeric condition numbers associated with job states, possibly
// better as an enum but it matters very little
#define JOBCOND_UNSET 0                // not set, likely 0 due to memory state
#define JOBCOND_INIT  1                // just created, not started yet
#define JOBCOND_RUN   2                // forked / exec()'d and running
#define JOBCOND_EXIT  3                // exited normally, retval has return value
#define JOBCOND_FAIL_EXEC  128         // numeric code indicating a failure to exec() a command
#define JOBCOND_FAIL_OUTP  129         // numeric code indicating a failure due to output redirection
#define JOBCOND_FAIL_INPT  130         // numeric code indicating a failure due to input redirection
#define JOBCOND_FAIL_OTHER 131         // numeric code indicating a failure for other undiagnosed reasons


// job_t: struct to represent a running job/child process.
typedef struct {
  char   jobname[MAX_LINE];        // name of command like "ls" or "gcc"
  char  *argv[ARG_MAX+1];          // argv for running child, NULL terminated
  int    argc;                     // number of elements on command line
  pid_t  pid;                      // PID of child
  int    retval;                   // return value of child, -1 if not finished
  int    condition;                // one of the JOBCOND_xxx values whic indicates state of job
  char  *output_file;              // name of output file or NULL if stdout
  char  *input_file;               // name of input file or NULL if stdin
  char   is_background;            // 1 for background job (& on command line), 0 otherwise
} job_t;

// shellac_t: struct for tracking state of shellac program
typedef struct {                
  job_t *jobs[MAX_JOBS];         // array of pointers to job_t structs; may have NULLs internally
  int job_count;                 // count of non-null job_t entries
} shellac_t;


// shellac_util.c: PROVIDED UTILITY FUNCTIONS
void Dprintf(const char* format, ...);
char *strnull(char *str);
char *job_condition_str(job_t *job);
void tokenize_string(char input[], char *tokens[], int *ntok); 
void pause_for(double secs);
void array_shift(char *strs[], int delpos, int maxlen);

// shellac_job.c
job_t *job_new(char *argv[]);
void job_free(job_t *job);
void job_print(job_t *job);
int job_update_status(job_t *job);
void job_start(job_t *job);

// shellac_control.c
void shellac_init(shellac_t *shellac);
int shellac_add_job(shellac_t *shellac, job_t *job);
int shellac_remove_job(shellac_t *shellac, int idx);
void shellac_start_job(shellac_t *shellac, int jobnum);
void shellac_print_jobs(shellac_t *shellac);
void shellac_free_jobs(shellac_t *shellac);
void shellac_update_one(shellac_t *shellac, int jobnum);
void shellac_update_all(shellac_t *shellac);
void shellac_wait_one(shellac_t *shellac, int jobnum);


// // cmd.c
// cmd_t *cmd_new(char *argv[]);
// void cmd_free(cmd_t *cmd);
// void cmd_start(cmd_t *cmd);
// void cmd_fetch_output(cmd_t *cmd);
// void cmd_print_output(cmd_t *cmd);
// void cmd_update_state(cmd_t *cmd, int nohang);
// char *read_all(int fd, int *nread);

// // cmdcol.c
// void cmdcol_print(cmdcol_t *col);
// void cmdcol_add(cmdcol_t *col, cmd_t *cmd);
// void cmdcol_update_state(cmdcol_t *col, int nohang);
// void cmdcol_freeall(cmdcol_t *col);
