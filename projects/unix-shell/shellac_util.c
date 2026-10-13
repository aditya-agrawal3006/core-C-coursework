// util.c: PROVIDED functions to ease parsing and pausing.

#include "shellac.h"

// Prints out a message if the environment variable DEBUG is set;
// Try running as `DEBUG=1 ./some_program`
void Dprintf(const char* format, ...) {
  if(getenv("DEBUG") != NULL){
    va_list args;
    va_start (args, format);
    char fmt_buf[2048];
    snprintf(fmt_buf, 2048, "|DEBUG| %s", format);
    vfprintf(stderr, fmt_buf, args);
    va_end(args);
  }
}


// Return the string "NULL" if str is NULL; otherwise return the
// string itself. Useful from printing "nice" NULL versions in cases
// like: printf("%s\n",strnull(somestring)).
char *strnull(char *str){
  if(str == NULL){
    return "NULL";
  }
  else{
    return str;
  }
}

// Return a string representatin of a JOBCOND_xxx value. These numbers
// represent what state a child job is in but the string printing
// version of it is more interpretable. Used in cases like
// 
//   printf("Job condition: %s\n",job_condition_str(job));
//
// NOTE: this function uses a 'static' variable to store the
// string. This means the strings it returns do not need to be
// free()'d BUT it is also not thread safe.
char *job_condition_str(job_t *job){
  static char condition_buf[MAX_LINE]; // "private" global variable for job_condition_str()

  if(0){}
  else if(job->condition == JOBCOND_UNSET){
    snprintf(condition_buf, MAX_LINE, "UNSET");
  }
  else if(job->condition == JOBCOND_INIT){
    snprintf(condition_buf, MAX_LINE, "INIT");
  }
  else if(job->condition == JOBCOND_RUN){
    snprintf(condition_buf, MAX_LINE, "RUN");
  }
  else if(job->condition == JOBCOND_EXIT){
    snprintf(condition_buf, MAX_LINE, "EXIT(%d)",job->retval);
  }
  else if(job->condition == JOBCOND_FAIL_EXEC){
    snprintf(condition_buf, MAX_LINE, "FAIL(EXEC)");
  }
  else if(job->condition == JOBCOND_FAIL_OUTP){
    snprintf(condition_buf, MAX_LINE, "FAIL(OUTP)");
  }
  else if(job->condition == JOBCOND_FAIL_INPT){
    snprintf(condition_buf, MAX_LINE, "FAIL(INPT)");
  }
  else if(job->condition == JOBCOND_FAIL_OTHER){
    snprintf(condition_buf, MAX_LINE, "FAIL(OTHER)");
  }
  else{
    Dprintf("ERROR: job_condition_str(): unknown condition '%d'\n",job->condition);
    snprintf(condition_buf, MAX_LINE, "???");
  }
  return condition_buf;    
}

// Analyze the contents of input and assign tokens[i] to point to the
// ith space-separated string in it. Set ntok to the number of tokens
// that are found. Akin to a "string split" BUT with major caveats.
// 
// CAUTION 1: This function modifies input[] to introduce \0
// characters into it. After calling the function, expect input[] to
// look different.
//
// CAUTION 2: This function does NOT allocate any memory. It will fill
// in pointers in tokens[] to point into internal positions of
// input[]. No distinct copies are created.
// 
// CAUTION 3: The controverisal strtok() library function used. It is
// tricky to use correctly so take care.
// 
// EXAMPLE USAGE:
//
// char input[] = "gcc -o myprog source.c > output.txt";
// char *tokens[255];
// int ntok;
// tokenize_string(input, tokens, &ntok);
// // ntoks: 6;
// // tokens[0]: "gcc";
// // tokens[1]: "-o";
// // tokens[2]: "mpyprog";
// // tokens[3]: "source.c";
// // tokens[4]: ">";
// // tokens[5]: "output.txt";
// // input[] is now "gcc\0-o\0myprog\0source.c\0>\0output.txt"
void tokenize_string(char input[], char *tokens[], int *ntok){
  int i = 0;
  char *tok = strtok(input," \n");
  while(tok!=NULL && i<ARG_MAX){
    tokens[i] = tok;            // assign tokens to found string
    i++;
    tok = strtok(NULL," \n");
  }
  tokens[i] = NULL;             // null terminate tokens to ease argv[] work
  *ntok = i;
  return;
}
  
// Sleep the running program for the given number of seconds allowing
// fractional values.
void pause_for(double secs){
  int isecs = (int) secs;
  double frac = secs - ((double) isecs);
  long inanos = (long) (frac * 1.0e9);

  struct timespec tm = {
    .tv_nsec = inanos,
    .tv_sec  = isecs,
  };
  nanosleep(&tm,NULL);
}

// shift the array of strings left by 1 starting at delpos; eliminates
// delpos from the array.
void array_shift(char *strs[], int delpos, int maxlen){
  if(delpos >= maxlen){
    return;
  }
  for(int i=delpos; i<maxlen-1; i++){
    strs[i] = strs[i+1];
  }
}
