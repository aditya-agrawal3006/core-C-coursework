// bake_funcs.c: required service functions for handling of Bakefiles

#include "bake.h"

////////////////////////////////////////////////////////////////////////////////
// PROBLEM 1
////////////////////////////////////////////////////////////////////////////////

char *slurp_file_efficient(char *fname){
    // initialize and set stat struct
    struct stat sb;
    int ret = stat(fname, &sb);
    if(ret == -1){
        perror("Couldn't open file");
        return NULL;
    }

    // Open file
    int fd = open(fname, O_RDONLY);
    if(fd == -1){
        perror("Couldn't open file");
        return NULL;
    }

    // Allocate file size in heap
    int size =  sb.st_size;
    char *f_data = malloc(size + 1);
    if(f_data == NULL){
        close(fd);
        return NULL;
    }

    // Read file into heap
    int b_read = read(fd, f_data, size);
    if(b_read == -1){
        free(f_data);
        close(fd);
        return NULL;
    }

    f_data[b_read] = '\0';

    close(fd);
    return f_data;
}
// PROBLEM 1: Uses combination of stat() and read() to efficiently
// read in the entire contents of a file into a malloc()'d block of
// memory, null terminates it (\0). Returns a pointer to the file data
// or NULL if the file could not be opened. If the file can't be
// opened, use perror() to print "Couldn't open file" and why.

rule_t *bake_target_rule(bake_t *bake, char *targname){
    // iterate through to find the targ
    for(int i = 0; i < bake->rule_count; i++){
        if(strcmp(bake->rules[i].target, targname) == 0){
            return &bake->rules[i];
        }
    }

    return NULL;
}
// PROBLEM 1: Searches bake for a rule with a target that equals
// `targname`. If found, returns a pointer to that rule. Otherwise
// returns NULL if no rule with target `targname` exists.
//
// DESIGN NOTE: The curent design makes use of linear search for
// target location, O(rule_count) for each lookup. Maintainting a hash
// table of target->rule key/vals would allow O(1) lookup for rules
// with given target names.

void bake_print_cmd(cmd_t *cmd){
    // check if silenced
    if(CHECK_BIT(cmd->cmd_flags, CMD_SILENCE_BIT)){
        return;
    }

    // prints all tokens in command
    for(int i = 0; cmd->tokens[i] != NULL; i++){
        printf("%s ", cmd->tokens[i]);
    }

    // checks if input/output redirect
    if(CHECK_BIT(cmd->cmd_flags, CMD_INPREDI_BIT)){
        printf("< %s ", cmd->input_redirect);
    }

    if(CHECK_BIT(cmd->cmd_flags,CMD_OUTREDI_BIT)){
        printf("> %s ", cmd->output_redirect);
    }

    printf("\n");


}
// PROBLEM 1: If the SILENCE_BIT is set, does nothing and immediately
// returns. Otherwise, prints the given command token by token. If I/O
// redirects are indicated by the fields of the command, prints the
// after all tokens with "< infile" for input followed by "> outfile"
// if present. This function is used to print out commands in
// bake_execute_cmd().

////////////////////////////////////////////////////////////////////////////////
// PROBLEM 2
////////////////////////////////////////////////////////////////////////////////

int bake_execute_cmd(cmd_t *cmd){
    bake_print_cmd(cmd);
    
    pid_t child = fork();

    // Child
    if(child == 0){
        // Sets up I/O redirection in child process if indicated by cmd flags and fields
        if(CHECK_BIT(cmd->cmd_flags, CMD_INPREDI_BIT)){
            int in_fd = open(cmd->input_redirect, O_RDONLY);
            if(in_fd == -1){
                perror("ERROR: can't open file for input");
                exit(CMD_FAIL_INPREDI);
            }

            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        if(CHECK_BIT(cmd->cmd_flags, CMD_OUTREDI_BIT)){
            int out_fd = open(cmd->output_redirect, O_WRONLY | O_CREAT | O_TRUNC,
                                S_IRUSR | S_IWUSR);

            if(out_fd == -1){
                perror("ERROR: can't open file for output");
                exit(CMD_FAIL_OUTREDI);
            }

            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        // Replace child process with command
        execvp(cmd->tokens[0], cmd->tokens);

        // Only runs if execvp fails
        perror("ERROR: command failed to exec");
        exit(CMD_FAIL_EXEC);
    }

    // Parent
    int status;
    waitpid(child, &status, 0);

    if(WIFEXITED(status)){
        return WEXITSTATUS(status);
    }
    
    return CMD_NONSTAND_EXIT;
}
// PROBLEM 2: Called during bake_do_update(). Prints the command using
// bake_print_cmd() unless its SILENCE bit is set. fork()'s a child
// process which exec's the command specified in cmd->tokens.  Sets up
// I/O redirection in child process if indicated by cmd flags and
// fields. Uses a wait() call to wait until the child process is
// done. If the child completes normally, its exit code is passed up,
// 0 or non-zero. If the child process fails to complete normally or
// prints an error message with line number of command
// CMD_NONSTAND_EXIT. Non-zero returns from this function will usually
// be handled in a build by cancelling the build whil 0 return will
// cause the build to prceed and execute subsequent commands.
//
// During Input and Output redirection, if the requrested files cannot
// be opened, the child process exits with code CMD_FAIL_INPREDI or
// CMD_FAIL_OUTREDI. Additionally, if the child fails to exec() a
// command, CMD_FAIL_EXEC is used as its return code. All of these are
// non-zero and trigger builds to fail. In the above failure cases,
// prints one of the below error messages appropriate to the error.
// 
// perror("ERROR: can't open file for output")
// perror("ERROR: can't open file for output")
// perror("ERROR: job failed to exec");
//
// CLARIFICATION: When open()'ing files for output redirection,
// certain options should be passed to ensure that the created file
// adheres to conventions
// - For the second argument to open(), pass options
//     O_WRONLY|O_CREAT|O_TRUNC
//   which will open the file for writing, create it if not present,
//   and truncate the file if it already exists
// - For the third argument to open(), pass the arguments
//     S_IRUSR|S_IWUSR
//   which will set the "rw" permissions on the created file for the
//   owner (user) of the file

////////////////////////////////////////////////////////////////////////////////
// PROBLEM 3
////////////////////////////////////////////////////////////////////////////////

int bake_set_updates(bake_t *bake, char *targname){
    // Find Rule
    rule_t *targ_rule = bake_target_rule(bake, targname);
    if(targ_rule == NULL){
        printf("No rule to create target '%s'\n", targname);
        return -1;
    }

    // Clear udpate bit
    CLEAR_BIT(targ_rule->rule_flags, RULE_UPDATE_BIT);

    // Checks for implicit rule
    if(CHECK_BIT(targ_rule->rule_flags, RULE_IMPLICIT_BIT)){
        if(access(targname, F_OK) == 0){
            return 0;
        }
        else{
            printf("Implicit rule cannot find file '%s'\n", targname);
            return -1;
        }
    }

    // Recursion through Dependencies
    for(int i = 0; targ_rule->deps[i] != NULL; i++){
        char *depname = targ_rule->deps[i];

        int dep_update = bake_set_updates(bake, depname);

        // Checks if updated
        if(dep_update == -1){
            return -1;
        } else if (dep_update == 1){
            SET_BIT(targ_rule -> rule_flags, RULE_UPDATE_BIT);
        } else if (access(targname, F_OK) == 0 && access(depname, F_OK) == 0) {
            // Checks if files exist and updates depending on time
            struct stat targ_sb;
            struct stat dep_sb;

            stat(targname, &targ_sb);
            stat(depname, &dep_sb);

            if(dep_sb.st_mtime > targ_sb.st_mtime){
                SET_BIT(targ_rule -> rule_flags, RULE_UPDATE_BIT);
            } 
        }
    }

    // If targ file not exist, then mark for update
    if(access(targname, F_OK) != 0 && targ_rule->cmd_count > 0){
        SET_BIT(targ_rule->rule_flags, RULE_UPDATE_BIT);
    }

    // If rule needs udpate then return 1
    if(CHECK_BIT(targ_rule->rule_flags, RULE_UPDATE_BIT)){
        return 1;
    }

    // rule doesnt need update
    return 0;
}
// PROBLEM 3: Starting at `targname`, recurses down the
// target/dependency DAG setting the UPDATE bit in rule_flags to
// indicate whether a target requires updates. Makes use of the
// CLEAR_BIT(), CHECK_BIT(), and SET_BIT() macros when dealing with
// bit-manipulations. For a given rule, first visits all its
// dependencies to check if they require updates via recursion. Then
// determines if `targname` requires an update. The conditions that
// require an update are described in detail in the program
// specification and shoudl be consulted while implementing this
// function. Returns 1 if target needs to be updated so associated
// rule commands should be run later. Returns 0 if the named target
// does not need to be updated. Prints an error if targname is not
// found and returns -1. If any dependency returns -1, an error
// ocurred lower down the DAG. In case, nothing is printed nothing and
// -1 is returned up the nested recursion.

int bake_do_updates(bake_t *bake, char *targname){
    rule_t *targ_rule = bake_target_rule(bake, targname);
    
    // Check if rule exists and if it needs to be updated
    if(targ_rule == NULL){
        printf("No rule to create target '%s'\n", targname);
        return -1;
    }

    if(!CHECK_BIT(targ_rule -> rule_flags, RULE_UPDATE_BIT)){
        return 0;
    }

    // recursively update dependencies
    int updates = 0;
    for(int i = 0; targ_rule->deps[i] != NULL; i++){
        int dep_updates = bake_do_updates(bake, targ_rule->deps[i]);

        if(dep_updates == -1){
            return -1;
        }

        updates = updates + dep_updates;
    }

    printf("bake: updating '%s' via %d command(s)\n",
           targname, targ_rule->cmd_count);

    // Execute commands
    for(int i = 0; i < targ_rule -> cmd_count; i++){
        int ret = bake_execute_cmd(&targ_rule->cmds[i]);

        if(ret != 0){
            printf("%s:%d ERROR during target '%s', exit code %d\n",bake->filename,
                   targ_rule->cmds[i].line_number, targname, ret);

            return -1;
        }
    }

    CLEAR_BIT(targ_rule->rule_flags, RULE_UPDATE_BIT);

    return updates + 1;
}
// PROBLEM 3: Starting at `targname`, run commands if the UPDATE bit
// is set. Before running commands associated with `targname`,
// recursively visit dependencies and if their UPDATE bit is set, run
// their commands first. Returns number of rules that were updated or
// -1 to indicate an error ocurred while running commands.

////////////////////////////////////////////////////////////////////////////////
// PROBLEM 4
////////////////////////////////////////////////////////////////////////////////

int bake_cmd_postprocess(bake_t *bake, cmd_t *cmd){
    if(cmd->tokens[0] != NULL && strcmp(cmd->tokens[0], "@") == 0){
        SET_BIT(cmd->cmd_flags, CMD_SILENCE_BIT);
        array_shift(cmd->tokens, 0, MAX_TOKS);
    }

    for(int i = 0; cmd->tokens[i] != NULL; i++){
        // Input and output redirection
        if(strcmp(cmd->tokens[i], "<") == 0){
            if(cmd->tokens[i+1] == NULL){
                return -1;
            }

            SET_BIT(cmd->cmd_flags, CMD_INPREDI_BIT);
            cmd->input_redirect = cmd->tokens[i+1];

            // remove '<' and file name from tokens
            array_shift(cmd->tokens, i, MAX_TOKS);
            array_shift(cmd->tokens, i, MAX_TOKS);

            // subtract from i to prevent skipping a token
            i--;
        } else if(strcmp(cmd->tokens[i], ">") == 0){
            if(cmd->tokens[i+1] == NULL){
                return -1;
            }

            SET_BIT(cmd->cmd_flags, CMD_OUTREDI_BIT);
            cmd->output_redirect = cmd->tokens[i+1];

            // remove '>' and file name from tokens
            array_shift(cmd->tokens, i, MAX_TOKS);
            array_shift(cmd->tokens, i, MAX_TOKS);

            // subtract from i to prevent skipping a token
            i--;
        }
    }

    return 0;
}
// PROBLEM 4: Examines cmd->tokens to set cmd->flags and a few other
// fields. Used to post-process the raw tokens of a command. Likely
// uses the utility function array_shift() to ease the re-arrangment
// of arrays.
// 
// 1. If tokens[0] = "@", eliminates this token by shifting all tokens
// over and sets the SILENCED bit
// 
// 2. If tokens[i] = "<", input redirection is desired; if tokens[i+1]
// is not NULL, returns -1. Otherwise sets the INPREDI bit and
// input_redirect field to tokens[i+1]. Then shifts all tokens over to
// eliminate tokens[i] and tokens[i+1].
// 
// 3. If tokens[i] = ">", output redirection is desired and performs
// similar operations to #2 with the OUTREDI flag and output_redirect
// fields.
//
// Correctly handles any order of "< input" / "> output" and other
// tokens (ex: output redirection may appear first followed by normal
// tokens followed by input redirection)
// 
// If multiple "<" or ">" tokens appear, the behavior of this function
// is unspecified: it may return an error, segfault, or behave randomly.
// 
// Returns 0 on succesful completion.
//
// DESIGN NOTE: This function must be called prior to any commands
// being run. The ideal location is during parsing to modify commands
// as a bake_t structure is built. However, this functionality is
// separated out to enable future functionality such as variable
// substitution to be added to commands and tested separately from
// parsing. Any additional context needed for such things will be part
// of the 'bake' parameter (e.g. may contain a map of variable
// names/values for substitutions).

void bake_post_process(bake_t *bake){
    // iterate through rules
    for(int i = 0; i < bake->rule_count; i++){
        // iterate through commands
        for(int j = 0; j < bake->rules[i].cmd_count; j++){
            bake_cmd_postprocess(bake, &bake->rules[i].cmds[j]);
        }
    }
}
// PROBLEM 4: Applies postprocessing operations to the bake after
// loading it from a file. Currently only iterates over all rules and
// applies bake_cmd_postprocess() to each of their commands.
//
// DESIGN NOTE: This function is where additional operations could be
// used to further modify the bakefile such as performing variable
// substitutions on rules / targets, including other bakefiles. etc.

////////////////////////////////////////////////////////////////////////////////
// PROBLEM 5 MAKEUP CREDIT (OPTIONAL)
////////////////////////////////////////////////////////////////////////////////
int bake_check_cycles_help(bake_t *bake, char *targname, char *path[], int path_len, int do_print){
    rule_t *rule = bake_target_rule(bake, targname);

    // Check if rule is NULL and if it is then throw error
    if(rule == NULL){
        printf("No rule to create target '%s'\n", targname);
        return -1;
    }

    // If this rule is already marked as visited, there is a cycle
    if(CHECK_BIT(rule->rule_flags, RULE_VISITED_BIT)){
        if(do_print != 0){
            printf("ERROR Cyclic Dependency detected:\n");

            // Print everything already in the path
            for(int i = 0; i < path_len; i++){
                printf("%s -> ", path[i]);
            }

            printf("%s\n", targname);
        }

        return 1;
    }

    // Add the current target to the path and mark it as visited
    path[path_len] = targname;
    SET_BIT(rule->rule_flags, RULE_VISITED_BIT);

    // Go through all the dependencies of the current rule and recursively check them
    for(int i = 0; rule->deps[i] != NULL; i++){
        int ret = bake_check_cycles_help(bake, rule->deps[i], path, path_len + 1, do_print);

        // if error, clear this rule visited bit and return
        if(ret != 0){
            CLEAR_BIT(rule->rule_flags, RULE_VISITED_BIT);
            return ret;
        }
    }

    CLEAR_BIT(rule->rule_flags, RULE_VISITED_BIT);
    return 0;
}

int bake_check_cycles(bake_t *bake, char *targname, int do_print){
    // Clear the rules visited flag by going through the rules
    for(int i = 0; i < bake->rule_count; i++){
        CLEAR_BIT(bake->rules[i].rule_flags, RULE_VISITED_BIT);
    }

    // Create array to keep track of path
    char *path[2048];

    // use helper to 
    int ret = bake_check_cycles_help(bake, targname, path, 0, do_print);

    // resets rule visited bit
    for(int i = 0; i < bake->rule_count; i++){
        CLEAR_BIT(bake->rules[i].rule_flags, RULE_VISITED_BIT);
    }

    return ret;
}
// PROBLEM 5 MAKEUP: Analyzes `bake` starting at targname to
// deteremine if any cycles are present which would create problems
// completing a build: if targA depends on targB depends an targA, a
// cyclcil dependency exists and could not be fulfilled.  Returns 1 if
// a cycle is present, 0 if no cycle is present, and -1 if an error
// occurs such as not finding a needed target.
//
// If do_print is non-zero, prints out a message like:
// 
//   ERROR Cyclic Dependency detected:
//   fileA.txt -> fileB.txt -> fileC.txt -> fileA.txt
// 
// where the first line is always identical and the second line shows
// the path from the initial target through a cycle. Implementations
// assume that these messages will fit in a 2048-byte array and have
// undefined behavior if the message is larger.
//
// NOTES: This function needs to initiate a recursive descent into
// `bake` via a helper function which does the brunt of the work.
// This helper function may be defined in whatever way with whatever
// prototype is convenient. Global variables may be used to help
// generate cycle message BUT implementations with pride will find
// ways to avoid the use of global variables.