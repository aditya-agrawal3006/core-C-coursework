#pragma once

#include <stdio.h>              // printing
#include <stdlib.h>             // malloc
#include <string.h>             // strcmp
#include <stdarg.h>             // vararg
#include <ctype.h>              // isspace
#include <unistd.h>             // fork/read/write
#include <fcntl.h>              // open
#include <sys/stat.h>           // stat
#include <sys/wait.h>           // wait

#define MAX_TOKS 128            // maximum number of tokens a cmd_t supports

#define IF_DEBUG if(getenv("DEBUG")!=NULL)
// Allows debuggin blocks as in
//   IF_DEBUG { 
//     execute_debug_code();
//   }
// which will execute when the code is run with the DEBUG environemnt
// variable set as in
//   >> DEBUG=1 ./a.out

// Macros to deal with bits in flags
#define SET_BIT(flag,   bitmask) (flag |=  bitmask)
#define CLEAR_BIT(flag, bitmask) (flag &= ~bitmask)
#define CHECK_BIT(flag, bitmask) (flag &   bitmask)

// cmd_t: single command to be run if a rule is invoked
typedef struct {
  char *tokens[MAX_TOKS];       // NULL-terminated array of tokens, pointers into a string table
  char *input_redirect;         // NULL or names an input file for stdin redirection
  char *output_redirect;        // NULL or names an output file for stdout redirection
  int line_number;              // line number for command in source file
  int cmd_flags;                // each bit indicates state, accessed via macros
  // Bit 0: is silenced
  // Bit 1: has input redirect
  // Bit 2: has output redirect
} cmd_t;

#define CMD_SILENCE_BIT 0b0001  // silence bit
#define CMD_INPREDI_BIT 0b0010  // input redirect bit
#define CMD_OUTREDI_BIT 0b0100  // output redirect bit

// Return and Exit codes for commands
#define CMD_NONSTAND_EXIT 120   // command had a nonstandard exit
#define CMD_FAIL_EXEC     121   // command failed to exec
#define CMD_FAIL_INPREDI  122   // command failed to set up input redirection
#define CMD_FAIL_OUTREDI  123   // command failed to set up output redirection

#define MAX_DEPS 128            // max number of dependencies for a rule
#define MAX_CMDS 128            // max number of commands for a rule

// rule_t: single rule in the Bakefile comprised of target,
// dependencies, and commands to run
typedef struct {                // no malloc()'d memory in rule_t, all strings point into a string_table
  int line_number;              // line number for rule in source file
  char *target;                 // name of targe (pointer to string_table)
  char *deps[MAX_DEPS];         // NULL terminated array of dependency names, pointers into string_table
  cmd_t cmds[MAX_CMDS];         // array of commands associated with rule
  int cmd_count;                // length of cmds array
  int rule_flags;               // each bit indicates a state, accessed via macros
  // Bit 0: needs_update: set to indicate rule should be run; CLEARED when it has been run
  // Bit 1: implicit_target added as it is a dependency for some rule
  // Bit 2: visited (used for cyclic dependency checking)
} rule_t;

#define RULE_UPDATE_BIT   0b0001 // update bit
#define RULE_IMPLICIT_BIT 0b0010 // implicit rule bit
#define RULE_VISITED_BIT  0b0100 // visited bit

#define DEFAULT_RULE_CAPACITY 4 // default size to malloc() for the rules[] field

// bake_t: overall struct for contents of a Bakefile
typedef struct {                
  FILE *infile;                 // file to use during reading; NULL otherwise
  char *filename;               // non-null indicates malloc()'d copy of filename for the contents of the bake
  int rule_capacity;            // capacity of rule_arr (space allocated so far)
  int rule_count;               // count of rules array
  rule_t *rules;                // malloc'd array of rules
  char *string_table;           // non-null indicates malloc'd array which must be free'd
} bake_t;

// bake_funcs.c
// problem 1 15
char *slurp_file_efficient(char *fname);
rule_t *bake_target_rule(bake_t *bake, char *targname);
rule_t *bake_add_empty_rule(bake_t *bake);
int bake_add_implicit_rules(bake_t *bake);
void bake_print_cmd(cmd_t *cmd);

// problem 2 10
int bake_execute_cmd(cmd_t *cmd);

// problem 3 10
int bake_set_updates(bake_t *bake, char *targname);
int bake_do_updates(bake_t *bake, char *targname);

// problem 4 15
int bake_cmd_postprocess(bake_t *bake, cmd_t *cmd);
void bake_post_process(bake_t *bake);

// bake_util.c
void split_into_lines(char *text, char ***lines, int *nlines);
void split_into_tokens(char *line, char ***tokens, int *ntokens);
void array_shift(char *strs[], int delpos, int maxlen);
long diff_timespec(struct timespec a, struct timespec b);
void Dprintf(const char* format, ...);
void ind_printf(int ind, const char* format, ...);
bake_t *bake_create_from_file(char *fname);
void bake_free(bake_t *bake);
void bake_show_cmd(cmd_t *cmd, int ind);
void bake_show_rule(rule_t *rule, int ind);
void bake_show_bake(bake_t *bake, int ind);

// bake_util.c MAKEUP CREDIT
int bake_check_cycles(bake_t *bake, char *targname, int do_print);


// void split_into_lines(char *text, char ***lines, int *nlines);
// void split_into_tokens(char *line, char ***tokens, int *ntokens);
// void array_shift(char *strs[], int delpos, int maxlen);
// void Dprintf(const char* format, ...);
// void ind_printf(int ind, const char* format, ...);
// long diff_timespec(struct timespec a, struct timespec b);
// void bake_show_cmd(cmd_t *cmd, int ind);
// void bake_show_rule(rule_t *rule, int ind);
// void bake_show_bake(bake_t *bake, int ind);



// extern int log_level;
// // controls logging in lprintf()

// void lprintf(int level, const char* format, ...);
// // printf() style function that prints log messages as things happen
// // to help with tracing and debugging. Output is only printed if
// // global log_level >= level parameter; higher levels are less likely
// // to be printed

