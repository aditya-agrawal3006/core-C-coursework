// shellac_job.c: functions related the job_t struct abstracting a
// running command. Most functions maninpulate jot_t structs.

#include "shellac.h"

void job_print(job_t *job){
    if(job == NULL){
        printf("NULL\n");
        return;
    }

    printf("job {\n");
    printf("  .jobname       = '%s'\n", job->jobname);
    if(job->pid <= 0){
        printf("  .pid           = %d\n", job->pid);
    } else{
        printf("  .pid           = #%d\n", job->pid);        
    }
    printf("  .retval        = %d\n", job->retval);
    printf("  .condition     = %s\n", job_condition_str(job));
    printf("  .output_file   = %s\n", strnull(job->output_file));
    printf("  .input_file    = %s\n", strnull(job->input_file));
    printf("  .is_background = %d\n", job->is_background);
    printf("  .argc          = %d\n", job->argc);
    printf("  .argv[] = {\n");
    for (int i = 0; i <= job->argc; i++) {
        if (job->argv[i] == NULL) {
        printf("    [%2d] = NULL\n", i);
        } else {
        printf("    [%2d] = %s\n", i, job->argv[i]);
        }
    }
    printf("   }\n");
    printf("}\n");
}
// Prints a representation of the job. Used primarily in testing but
// useful as well for debugging. Several provided utility functions
// from shellac_util.c are useful to simplify the formatting process:
// strnull() simplifies printing nice "NULL" strings and
// job_condition_str() simplifies creating a string based on condition
// codes.
//
// SPECIAL CASE: If the pid of the child is <= 0, print it just as 0
// or -2, or whatever negative number it is without the leading #
// (hash symbol).
//
// SAMPLE OUTPUT FORMAT: 
// job {
//   .jobname       = 'diff'
//   .pid           = #2378
//   .retval        = 1
//   .condition     = EXIT(1)
//   .output_file   = diff_output.txt
//   .input_file    = NULL
//   .is_background = 1
//   .argc          = 3
//   .argv[] = {
//     [ 0] = diff
//     [ 1] = file1.txt
//     [ 2] = file2.txt
//     [ 3] = NULL
//    }
// }



job_t *job_new(char *argv[]){
    // Allocates heap memory for a job_t struct
    job_t *job = malloc(sizeof(job_t));

    if(job == NULL){
        return NULL;
    }

    // The initial condition of the job is INIT with -1 for its pid and retval
    // Normal foreground jobs have is_background set to 0.
    job->jobname[0] = '\0';
    job->argc = 0;
    job->pid = -1;
    job->retval = -1;
    job->condition = JOBCOND_INIT;
    job->output_file = NULL;
    job->input_file = NULL;
    job->is_background = 0;

    // Copies Job name into the new job
    if (argv[0] != NULL) {
        strncpy(job->jobname, argv[0], MAX_LINE - 1);
        job->jobname[MAX_LINE - 1] = '\0';
    } else {
        job->jobname[0] = '\0';
    }

    // Creates heap copies of argv[] via string duplication
    int i = 0;      // index original argv[]
    int j = 0;      // index in job argv[]

    while (argv[i] != NULL && j < ARG_MAX) {
        if (strcmp(argv[i], ">") == 0) {
            // handle output redirection
            if (argv[i+1] == NULL) {
                fprintf(stderr, "ERROR: No file given for output redirection\n");

                job->argc = j;
                job_free(job);
                return NULL;
            }

            job->output_file = strdup(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "<") == 0) {
            // handle input redirection
            if (argv[i+1] == NULL) {
                fprintf(stderr, "ERROR: No file given for input redirection\n");

                job->argc = j;
                job_free(job);
                return NULL;
            }

            job->input_file = strdup(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "&") == 0) {
            // mark background
            job->is_background = 1;
        } else {
            job->argv[j] = strdup(argv[i]);
            j++;
        }

        i++;
    }
    job->argv[j] = NULL;
    job->argc = j;

    return job;
}
// Create a new job based on the argv[] provided. The parameter argv[]
// will be NULL terminated to allow detecing the end of the
// array. Allocates heap memory for a job_t struct and creates heap
// copies of argv[] via string duplication. The last element in
// job.argv[] will be NULL terminated as the paramter array is. The
// initial condition of the job is INIT with -1 for its pid and
// retval. The jobname is argv[0] and.  Normal foreground jobs have
// is_background set to 0.
//
// PROBLEM 3: If argv[] contains input or output redirection (> outfile
// OR < infile) or the background symbol &, then removes these from
// the argv[] and sets appropriate other fields. If problems are found
// with input/output redirection such as a ">" with no following file,
// prints an error and return NULL.
// 
// HINTS for PROBLEM 3: The provided array_shift() function from the
// shellac_util.c may prove useful to shift over input / output
// redirection though other methods are possible. Note that the ">"
// "<" and "&" strings are removed from the command line so must be
// handled wth care: either don't duplicate them or free() any
// duplicates of them before returning.



void job_free(job_t *job){
    if (job == NULL) {
        return;
    }

    // free each argv element
    for (int i = 0; i < job->argc; i++) {
        free(job->argv[i]);
        job->argv[i] = NULL;

    }

    // Free output_file and input_file if they’re not NULL
    if (job->output_file != NULL) {
        free(job->output_file);
    }

    if (job->input_file != NULL) {
        free(job->input_file);
    }

    free(job);
}
// Deallocates a job structure. Deallocates the strings in the argv[]
// array. Deallocates any input / output file associated with
// fields. Finally de-allocates the struct itself.



void job_start(job_t *job){
    pid_t pid = fork();

    if (pid < 0) {
        job->condition = JOBCOND_FAIL_OTHER;
        return;
    }

    if (pid == 0) {
        if (job->output_file != NULL) {
            int outfd = open(job->output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

            if (outfd < 0) {
                fprintf(stderr, "ERROR: cannot open output file '%s': %s\n",
                                            job->output_file, strerror(errno));
                exit(JOBCOND_FAIL_OUTP);
            }

            // Any ouput gets redirected to output_file
            dup2(outfd, STDOUT_FILENO);
            close(outfd);
        }

        if (job->input_file != NULL) {
            int infd = open(job->input_file, O_RDONLY);

            if (infd < 0) {
                fprintf(stderr, "ERROR: cannot open input file '%s': %s\n",
                                             job->input_file, strerror(errno));
                exit(JOBCOND_FAIL_INPT);
            }

            // Any input gets redirected to input_file
            dup2(infd, STDIN_FILENO);
            close(infd);
        }

        execvp(job->argv[0], job->argv);

        // exec failed
        fprintf(stderr, "ERROR: job failed to exec: %s\n", strerror(errno));
        exit(JOBCOND_FAIL_EXEC);
    }

    job->pid = pid;
    job->condition = JOBCOND_RUN;
}
// Forks a process and executes the command described in the job as a
// process.  Changes the condition field to "RUN".
//
// PROBLEM 3: If input/output redirection is indicated by fields of
// the job, sets this up using dup2() calls. For output redirection,
// ensures any output files are created and if they already exist, are
// "clobbered" via appropriate options to open(). If input/output
// redirection fails, the child process should exit with
// JOBCOND_FAIL_OUTP or JOBCOND_FAIL_INPT.  If a job fails to exec(),
// the child should exit() with code JOBCOND_FAIL_EXEC. In this case,
// there is no need for the child process to attempt to de-allocate
// any memory as the OS will recoup all its resources on exit.



int job_update_status(job_t *job){
    int status;
    int options = 0;

    // Check if background is 1 and set options
    if (job->is_background) {
        options = WNOHANG;
    }

    // Waited on process and set the condition
    pid_t ret = waitpid(job->pid, &status, options);
    if (ret < 0) {
        job->condition = JOBCOND_FAIL_OTHER;
        return 0;
    }

    if (job->is_background && ret == 0) {
    // still running
        return 0;
    }

    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);

        if (code == JOBCOND_FAIL_EXEC) {
            // exec failed in child
            // retval is -1
            job->condition = JOBCOND_FAIL_EXEC;
        } else if (code == JOBCOND_FAIL_OUTP) {
            job->condition = JOBCOND_FAIL_OUTP;
        } else if (code == JOBCOND_FAIL_INPT) {
            job->condition = JOBCOND_FAIL_INPT;
        } else {
            job->retval = code;
            job->condition = JOBCOND_EXIT;
        }
    } else {
        job->condition = JOBCOND_FAIL_OTHER;
    }

  return 1;
}
// Checks on the job for a status update. This utilizes a wait() or
// waitpid() system call. If the job has completed, updates its
// condition to reflect either EXIT or FAIL. For exits, uses macros to
// extract the exit status and assigns it the retval field.
// 
// PROBLEM 2: For foreground (default) jobs, blocks the parent process
// until the child is completed. Returns 1 for a condition change
// (e.g. RUN to EXIT / FAIL).
//
// PROBLEM 3: For background jobs, uses the WNOHANG option to avoid
// blocking the parent. If the job is finished, updates its retval,
// condition, and returns 1. If the job is not finished, just returns
// 0.
//
// For erroneous calls such as calls on a NULL job or on a non-running
// job without a pid, the behavior of this function is implementation
// dependent (may segfault, may exit with an error message, etc.) This
// situation is not tested.
