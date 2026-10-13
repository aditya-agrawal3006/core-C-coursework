// shellac_control.c: functions related the shellac_t struct that controls
// multiple jobs

#include "shellac.h"

void shellac_init(shellac_t *shellac) {
    shellac->job_count = 0;

    for (int i = 0; i < MAX_JOBS; i++) {
        shellac->jobs[i] = NULL;
    }
}
// Initialize all fields of the shellac jobs[] array to NULL and set
// the job_count to 0

int shellac_add_job(shellac_t *shellac, job_t *job){
    for (int i = 0; i < MAX_JOBS; i++) {
        if (shellac->jobs[i] == NULL) {
            shellac->jobs[i] = job;
            shellac->job_count++;
            return i;
        }
    }

    printf("ERROR: job table full\n");
    return -1;
}
// Add a single job to the jobs[] array. Search for the first NULL
// entry in the array and add that on. Does basic error checking to
// see if array is full and prints an error message of some kind of
// so. Returns the index in the jobs array (job number) where the new
// job is added. If the jobs array is full, prints an error message
// and returns -1.

int shellac_remove_job(shellac_t *shellac, int jobnum){
    if (shellac->jobs[jobnum] == NULL) {
        printf("ERROR: No such job '%d'\n", jobnum);
        return -1;
    }

    job_free(shellac->jobs[jobnum]);
    shellac->jobs[jobnum] = NULL;
    shellac->job_count--;

    return 0;
}
// Remove the indicated job from the jobs array and replace its entry
// with NULL. Decrements the job count. De-allocates memory associated
// with the job via a call to job_free(). Does basic error checking so
// that if the specified jobnum is already NULL, prints an error to
// that effect.

void shellac_start_job(shellac_t *shellac, int jobnum){
    job_t *job = shellac->jobs[jobnum];
    
    if (job == NULL) {
        printf("ERROR: no job %d\n", jobnum);
        return;
    }

    printf("=== JOB %d STARTING: %s ===\n", jobnum, job->jobname);
    job_start(job);
}
// Starts the specified job number. First prints a message about
// starting the job of the format
// 
//   === JOB %d STARTING: %s ===\n
// 
// with jobnum and jobname filled in. Then uses a call to job_start()
// to start the job.

void shellac_print_jobs(shellac_t *shellac){
    for (int i = 0; i < MAX_JOBS; i++) {
        if (shellac->jobs[i] != NULL) {
            printf("[%d] %s\n", i, shellac->jobs[i]->jobname);
        }
    }

    printf("%d total jobs\n", shellac->job_count);
}
// Prints the job number and jobname of all non-NULL jobs in the jobs
// array.

void shellac_free_jobs(shellac_t *shellac){
    for (int i = 0; i < MAX_JOBS; i++) {
        if (shellac->jobs[i] != NULL) {
            job_free(shellac->jobs[i]);
            shellac->jobs[i] = NULL;
        }
    }

    shellac->job_count = 0;
}
// Traverses the jobs array and de-allocates any non-null jobs.

void shellac_update_one(shellac_t *shellac, int jobnum){
    job_t *job = shellac->jobs[jobnum];
    if (job == NULL) {
        return;
    }

    int changed = job_update_status(job);
    if (changed) {
        printf("=== JOB %d COMPLETED %s [#%d]: %s ===\n", jobnum, job->jobname,
            job->pid, job_condition_str(job));

        shellac_remove_job(shellac, jobnum);
    }
}
// Updates a single job via a cal to job_update_status().  If that
// functions return value indicates that the job completed, prints a
// message of the form
//
// === JOB %d COMPLETED %s [#%d]: %s ===\n
//
// with jobnum, name, pid number, and ending condition reported. Uses
// the job_condition_str() functon to create the condition string. For
// completed jobs, de-allocates them and removes them from the jobs
// array.
// 
// Examples of the printed message:
// === JOB 0 COMPLETED bash [#1000]: EXIT(0) ===
// === JOB 5 COMPLETED gcc [#22830]: EXIT(1) ===
// === JOB 1 COMPLETED cat [#22833]: FAIL(INPT) ===

void shellac_update_all(shellac_t *shellac){
    for (int i = 0; i < MAX_JOBS; i++) {
        if (shellac->jobs[i] != NULL) {
            shellac_update_one(shellac, i);
        }
    }
}
// Iterates through all jobs in the jobs array and updates them. Used
// mainly to check for the completion of background jobs at the end of
// each interactive loop iteration.

void shellac_wait_one(shellac_t *shellac, int jobnum){
    if (shellac->jobs[jobnum] == NULL) {
        printf("ERROR: No job '%d' to wait for\n", jobnum);
        return;
    }

    job_t *job = shellac->jobs[jobnum];
    job->is_background = 0;
    shellac_update_one(shellac, jobnum);
}
// Change the status of a background job to foreground
// (e.g. is_background becomes 0) then update that job to wait for
// it. Does basic error checking so that if the jobnum indicated
// doesn't exit, an error message of some sort is printed.