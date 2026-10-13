#include "shellac.h"
#include <stdio.h>
#include <string.h>

void print_help(){
  char *helpstr = "\
SHELLAC COMMANDS\n\
help               : show this message\n\
exit               : exit the program\n\
jobs               : list all background jobs that are currently running\n\
pause <secs>       : pause for the given number of seconds, fractional values supported\n\
wait <jobnum>      : wait for given background job to finish, error if no such job is present\n\
tokens [arg1] ...  : print out all the tokens on this input line to see how they apper\n\
command [arg1] ... : Non-built-in is run as a job\n\
";
  printf("%s",helpstr);
}


int main(int argc, char *argv[]){
    // Echo Functionality
    int echo = 0;
    if (argc > 1 && strcmp(argv[1], "--echo") == 0) {
        echo = 1;
    }

    shellac_t sh;
    shellac_init(&sh);

    char line[MAX_LINE];
    char *tokens[ARG_MAX + 1];
    int ntok;

    while (1) {
        // print prompt
        printf("(shellac) ");
        fflush(stdout);

        // read a line
        if(fgets(line, MAX_LINE, stdin) == NULL){
            printf("\nEnd of input\n");
            break;
        }

        // Remove \n and instead put \0
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }

        // If echo is on, print the line
        if(echo == 1 && len > 0){
            printf("%s\n", line);
        }

        // Tokenize String
        tokenize_string(line, tokens, &ntok);
        if(ntok == 0){
            printf("\n");
            continue;
        }

        // Check token 0 to act on user command
        if(strcmp(tokens[0], "help") == 0){                                     // Help
            print_help();
        } else if (strcmp(tokens[0], "exit") == 0) {                            // Exit
            break;
        } else if (strcmp(tokens[0], "jobs") == 0) {                            // Jobs
            shellac_print_jobs(&sh);
        } else if (strcmp(tokens[0], "pause") == 0) {                           // Pause
            if (ntok >= 2) {
                double secs = atof(tokens[1]);
                printf("Pausing for %.3f seconds\n", secs);
                pause_for(secs);
            }
        } else if (strcmp(tokens[0], "wait") == 0) {                            // Wait
            if (ntok >= 2) {
                int jobnum = atoi(tokens[1]);
                shellac_wait_one(&sh, jobnum);
            } else {
                printf("ERROR: No job '' to wait for\n");
            }
        } else if (strcmp(tokens[0], "tokens") == 0) {                          // Tokens
            printf("%d tokens in input line\n", ntok);
            for (int i = 0; i < ntok; i++) {
                printf("tokens[%d]: %s\n", i, tokens[i]);
            }
        } else{
            // Run it as a job as this is a non builtin command
            job_t *job = job_new(tokens);
            int idx = shellac_add_job(&sh, job);
            shellac_start_job(&sh, idx);

            if (!job->is_background) {
                // run in foreground until block is done
                shellac_update_one(&sh, idx);
            }
        }

        // Shell checks on background jobs everytime loop is iterated
        shellac_update_all(&sh);
    }


    return 0;
}

