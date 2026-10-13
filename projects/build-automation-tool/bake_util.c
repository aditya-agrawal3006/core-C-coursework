// bake_util.c: provided functions for the bake program
#include "bake.h"

////////////////////////////////////////////////////////////////////////////////
// string processing
////////////////////////////////////////////////////////////////////////////////

// Modifies `text` so that each line ends with a \0 (rather than
// \n). Sets lines to point to a malloc()'d array of the start of each
// line. Sets nlines to be the number of lines.
//
// EXAMPLE:
// char text[32] = "every good\nboy does fine\nFACEG\n\0";
// char **lines; int nlines;
// split_into_lines(text, &lines, &nlines);
// - text[] is now "every good\0boy does fine\0FACEG\0\0"
// - nlines is now 4
// - lines is now {"every good","boy does fine","FACEG",""}
// - lines[i] points into text[]
void split_into_lines(char *text, char ***lines, int *nlines){
  int nl = 0;
  for(int i=0; text[i] != '\0'; i++){
    if(text[i] == '\n'){
      nl++;                     // found a line
      text[i] = '\0';           // adjust to be null terminated
    }
  }
  char **lin = malloc(sizeof(char*) * nl);
  int pos = 0;
  for(int i=0; i<nl; i++){
    lin[i] = text+pos;          // set position of current line
    pos += strlen(lin[i])+1;    // update position past current line
  }
  *nlines = nl;
  *lines = lin;
  return;
}

// Modifies line so that whitespace-separate characters are replaced
// with \0's to denote 'tokens'. Sets parameter `tokens` to be a
// malloc()'d array with pointers to each of the tokens. Sets ntokens
// to be the nubmer of tokens in the line.
//
// EXAMPLE:
// char line[32] = "hello  :  friendly world\0";
// char **tokens; int notkens;
// split_into_tokens(line, &tokens, &ntokens);
// - line[] is now "hello\0\0:\0\0friendly\0world\0"
// - ntokens is now 4
// - tokens is now {"hello",":","friendly","world"}
// - tokens[i] points into line[]
void split_into_tokens(char *line, char ***tokens, int *ntokens){
  // first count tokens in the line to allocate space for them
  int nt = 0;
  int last_space = 1;
  for(int i=0; line[i] != '\0'; i++){
    // transition from space to nonspace is beginning of a token
    if(last_space && !isspace(line[i])){
      nt++;
    }
    last_space = isspace(line[i]);   // is this char a space
  }

  // allocate space for pointers to tokens and scan line again
  // assigning pointers to tokens found within.
  char **toks = malloc(sizeof(char*) * nt);
  int tok_idx = 0;
  last_space = 1;
  for(int i=0; line[i] != '\0'; i++){
    if(last_space && !isspace(line[i])){
      toks[tok_idx] = line+i;
      tok_idx++;
    }
    last_space = isspace(line[i]);   // is this char a space
    if(isspace(line[i])){
      line[i] = '\0';                // make all spaces nulls
    }
  }
  for(int i=0; i<nt; i++){           // debug output to print resulting tokens
    Dprintf("toks[%d] = '%s'\n",i,toks[i]);
  }
  *ntokens = nt;
  *tokens = toks;
  return;
}

////////////////////////////////////////////////////////////////////////////////
// general utilities
////////////////////////////////////////////////////////////////////////////////

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

// returns difference in times between a-b; positive return means a is
// newer than b; negative indicates b is newer than a
long diff_timespec(struct timespec a, struct timespec b){
  long diff = a.tv_sec - b.tv_sec;
  if(diff != 0){
    return diff;
  }
  diff = a.tv_nsec - b.tv_nsec;
  return diff;
}

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

// Like printf but indents 'ind' spaces; used for printing recursive
// data structures
void ind_printf(int ind, const char* format, ...) {
  for(int i=0; i<ind; i++){
    printf(" ");
  }
  va_list args;
  va_start (args, format);
  vfprintf(stdout, format, args);
  va_end(args);
}

////////////////////////////////////////////////////////////////////////////////
// bake-specific provided functions
////////////////////////////////////////////////////////////////////////////////


// Modifies `bake` to add a new, empty rule to it and returns a
// pointer to that rule. If bake->rules[] is full (rule_capacity and
// rule_count are equal) doubles the size of rules[] via realloc() in
// order create room at its end for the new empty rule. Returns a
// pointer to the new empty rule.
//
// This function intitalizes all the data in the returned rule to NULL
// or 0 including the nested arrays. It uses of the memset() to
// quickly initialize the entire rule_t struct to 0's. This is
// possible due to the nested arrays being within the rule_t rather
// than pointers to other blocks of memory which means a single
// memset() call will suffice.
// 
// CAUTION: Calling this function MAY invalidate any pointers to
// existing rules as the array that houses the rules may move.
// This sequence is dangerous:
//   rule_t *rule  = bake_target_rule(bake, "sometarget");
//   rule_t *empty = bake_add_empty_rule(bake);
//   rule may now point to de-allocated memory
rule_t *bake_add_empty_rule(bake_t *bake){
  if(bake->rule_count == bake->rule_capacity){
    bake->rule_capacity *= 2;
    bake->rules = realloc(bake->rules, sizeof(rule_t)*bake->rule_capacity);
  }
  rule_t *new_rule = &bake->rules[bake->rule_count];
  memset(new_rule, 0, sizeof(rule_t));
  bake->rule_count++;
  return new_rule;
}

// Iterate over all rules appending implicit rules for any dependency
// that is not an explicit target. New rules added in this way are
// marked with the RULE_IMPLICIT_BIT. The number of new rules added is
// returned.  
//
// Situations where a program artifact has a source file dependency
// often occur such as in:
// ------------------------------------------------------------
// progA : src1.o src2.o           # EXPLICIT RULE 1
//         gcc src1.o src2.o
//
// src1.o : src1.c                 # EXPLICIT RULE 2
//         gcc -c src1.c
//
// src2.o : src2.c                 # EXPLICIT RULE 3
//         gcc -c src2.c
// ------------------------------------------------------------
// This would create an irregular rules array which would complicate
// the recursive traversal of the DAG data structure. Instead,
// implicit rules are added for the .c files here which need to exist
// to make the overall data structure look like this:
// ------------------------------------------------------------
// progA : src1.o src2.o           # EXPLICIT RULE 1
//         gcc src1.o src2.o
//
// src1.o : src1.c                 # EXPLICIT RULE 2
//         gcc -c src1.c
//
// src2.o : src2.c                 # EXPLICIT RULE 3
//         gcc -c src2.c
//
// src1.c :                        # IMPLICIT RULE 4
//
// src2.c :                        # IMPLICIT RULE 5
// ------------------------------------------------------------
// The Implicit rules are added at the end and are automatically
// satisfied so long as the Target files exist.
//
// CAUTION: Since bake->rules[] may expand, care must be taken when
// referencing rules in this function to avoid inadvertently derefing
// a pointer to free()'d memory.
int bake_add_implicit_rules(bake_t *bake){
  Dprintf("bake_add_implicit_rules()\n");
  int expl_count = bake->rule_count;
  int nadded = 0;
  for(int i=0; i<expl_count; i++){
    for(int j=0; bake->rules[i].deps[j] != NULL; j++){
      if(bake_target_rule(bake, bake->rules[i].deps[j]) == NULL){ 
        rule_t *new_rule = bake_add_empty_rule(bake); // create implicit rule
        new_rule->target = bake->rules[i].deps[j];
        SET_BIT(new_rule->rule_flags, RULE_IMPLICIT_BIT);
        nadded++;
      }
    }
  }
  return nadded;
}

// parse file and create a bakefile from it
bake_t *bake_create_from_file(char *fname){
  Dprintf("START bake_create_from_file(%s)\n",fname);
  char *text = slurp_file_efficient(fname);

  if(text == NULL){
    return NULL;
  }

  int nlines = -1;
  char **lines = NULL;
  split_into_lines(text, &lines, &nlines);
  Dprintf("split_into_lines() gave %d lines\n",nlines);

  // Allocate space for the bake data and initialize it to default
  // values. The string_table will track the text slurped from the
  // file which will be split into lines and tokens.
  bake_t *bake = malloc(sizeof(bake_t));
  bake->rules = malloc(sizeof(rule_t) * DEFAULT_RULE_CAPACITY);
  bake->rule_count = 0;
  bake->rule_capacity = DEFAULT_RULE_CAPACITY;
  bake->string_table = text;
  bake->filename = strdup(fname);

  // loop logic skips empty lines until a rule is reached; consumes
  // that rule with its target/dependencies + all commands
  for(int i=0; i < nlines; i++){
    Dprintf("line %3d: '%s'\n",i,lines[i]);
    char lead = lines[i][0];
    if(lead == '#' || lead == '\0'){
      Dprintf("Comment / Empty\n");
      continue;                 // skip comments / blanks
    }
    if(lead==' '||lead=='\t'){
      printf("%s:%d: Syntax error, no leading spaces allowed on non-command lines\n",
             fname,i);
      exit(EXIT_FAILURE);
    }

    // lead must be a non-blank so start a rule, consume it
    Dprintf("Rule Start\n");
    int ntokens;
    char **tokens;
    split_into_tokens(lines[i], &tokens, &ntokens);
    Dprintf("split_int_tokens() gave %d tokens\n",ntokens);
    if(ntokens<2 || strcmp(tokens[1], ":") != 0){
      printf("%s:%d: Syntax error, rules have format 'target : dep1 dep2 ...'\n",
             fname,i);
      exit(EXIT_FAILURE);
    }

    // add a rule and set its target/deps based first line
    rule_t *rule = bake_add_empty_rule(bake);
    rule->line_number = i+1;
    rule->target = tokens[0];
    Dprintf("RULE target: %s\n",rule->target);
    for(int j=2; j<ntokens; j++){
      rule->deps[j-2] = tokens[j];
      Dprintf("RULE dep[%d]: %s\n",j-2,tokens[j]);
    }
    rule->deps[ntokens-2] = NULL;
    free(tokens);

    // iterate over lines which are indented (start with space)
    // consuming them as commands. Start on next line and iterate forwards
    for(i++; i<nlines && (lines[i][0]==' '||lines[i][0]=='\t'); i++){    
      Dprintf("line %d RULE command[%d]: '%s'\n",i,rule->cmd_count,lines[i]);
      split_into_tokens(lines[i], &tokens, &ntokens);
      if(ntokens == 0){
        printf("%s:%d: Syntax error, empty command line with leading space\n",
               fname,i+1);
        exit(EXIT_FAILURE);
      }
      cmd_t *cmd = &rule->cmds[rule->cmd_count];
      cmd->line_number = i+1;
      rule->cmd_count++;
      for(int j=0; j<ntokens; j++){
        cmd->tokens[j] = tokens[j];
      }
      // cmd->tokens[ntokens] = NULL;  // not needed due to NULLs in array
      free(tokens);
    }
    
  }

  // after end of main for loop will have iterated over all lines in
  // the file adding rules, now ready to return
  free(lines);
  return bake;
}


// De-allocates memory associated with a bake_t
void bake_free(bake_t *bake){
  if(bake->filename != NULL){
    free(bake->filename);
  }
  free(bake->rules);
  if(bake->string_table != NULL){
    free(bake->string_table);
  }
  free(bake);
}

// print a formatted version of cmd_t for testing / debugging
void bake_show_cmd(cmd_t *cmd, int ind){
  ind_printf(ind,"cmd_t {\n"); 
  int i;
  for(i=0; cmd->tokens[i] != NULL; i++){
    ind_printf(ind+2,"tokens[%2d] = '%s'\n", i, cmd->tokens[i]);
  }
  ind_printf(ind+2,"tokens[%2d] = %s\n", i, cmd->tokens[i]);
  ind_printf(ind+2,"input_redirect = '%s'\n",cmd->input_redirect);
  ind_printf(ind+2,"output_redirect = '%s'\n",cmd->output_redirect);
  ind_printf(ind+2,"line_number = %d\n",cmd->line_number);
  ind_printf(ind+2,"cmd_flags = 0x%02x ", cmd->cmd_flags);
  printf("[ ");
  printf("%s ", CHECK_BIT(cmd->cmd_flags, CMD_SILENCE_BIT) ? "SI":"--");
  printf("%s ", CHECK_BIT(cmd->cmd_flags, CMD_INPREDI_BIT) ? "IR":"--");
  printf("%s ", CHECK_BIT(cmd->cmd_flags, CMD_OUTREDI_BIT) ? "OR":"--");
  printf("]\n");
  ind_printf(ind,"}\n");
}

// print a formatted version of a rule_t for testing / debugging
void bake_show_rule(rule_t *rule, int ind){
  ind_printf(ind,"rule_t {\n");
  ind_printf(ind+2,"target = %s\n",rule->target);
  int i;
  for(i=0; rule->deps[i] != NULL; i++){
    ind_printf(ind+2,"deps[%2d] = '%s'\n", i, rule->deps[i]);
  }
  ind_printf(ind+2,"deps[%2d] = %s\n", i, rule->deps[i]);
  ind_printf(ind+2,"rule_flags = 0x%02x ", rule->rule_flags);
  printf("[ ");
  printf("%s ", CHECK_BIT(rule->rule_flags, RULE_UPDATE_BIT)   ? "UP":"--");
  printf("%s ", CHECK_BIT(rule->rule_flags, RULE_IMPLICIT_BIT) ? "IM":"--");
  printf("%s ", CHECK_BIT(rule->rule_flags, RULE_VISITED_BIT)  ? "VI":"--");
  printf("]\n");
  ind_printf(ind+2,"cmd_count = %d\n", rule->cmd_count);
  for(i=0; i<rule->cmd_count; i++){
    ind_printf(ind+2,"cmds[%2d] =\n",i);
    bake_show_cmd(&rule->cmds[i],ind+4);
  }
  ind_printf(ind,"}\n");
}

// print a formatted version of a bake_t for testing / debugging
void bake_show_bake(bake_t *bake, int ind){
  ind_printf(ind,"bake_t {\n");
  ind_printf(ind+2,"filename = %s\n",bake->filename);
  ind_printf(ind+2,"rule_capacity = %d\n",bake->rule_capacity);
  ind_printf(ind+2,"rule_count = %d\n",bake->rule_count);
  int i;
  for(i=0; i<bake->rule_count; i++){
    ind_printf(ind+2,"rules[%2d] =\n",i);
    bake_show_rule(&bake->rules[i],ind+4);
  }
  // intentionally skipping the string_table as printing with this
  // will be hard
  printf("}\n");
}
