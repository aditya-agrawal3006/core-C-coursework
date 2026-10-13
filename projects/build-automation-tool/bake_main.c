#include "bake.h"

int main(int argc, char *argv[]){
    //  process the command line arguments to determine if the default Bakefile or some other 
    //    file should be loaded
    char *filename = "Bakefile";
    char *target = NULL;

    if(argc >= 2 && strcmp(argv[1], "-f") == 0){
        filename = argv[2];

        if(argc >= 4){
            target = argv[3];
        }
    } else if(argc >= 2){
        target = argv[1];
    }

    //  load the requested Bakefile int a bake_t struct
    //  if the load fails, 
    //    print "ERROR: unable to process file 'XXX'" 
    //    substituting in the filename and exit the program with EXIT_FAILURE
    bake_t *bake = bake_create_from_file(filename);
    if(bake == NULL){
        printf("ERROR: unable to process file '%s'\n", filename);
        return EXIT_FAILURE;
    }

    //  if the loaded file has no rules in it (empty)
    //    print "ERROR: 'XXX' has 0 rules
    //    and exit with EXIT_FAILURE
    if(bake->rule_count == 0){
        printf("ERROR: '%s' has 0 rules\n", filename);
        bake_free(bake);
        return EXIT_FAILURE;
    }

    //  Add implict rules to the bake_t
    //  Post-process the bake_t
    bake_add_implicit_rules(bake);
    bake_post_process(bake);


    //  If a target like "all" or "clean" was specified on the command line
    //    set that as the target to update
    //    otherwise set the 0th rule target as the one to update
    if(target == NULL){
        target = bake->rules[0].target;
    }

    // Extra credit: check to make sure their are no cycles
    int cycle_ret = bake_check_cycles(bake, target, 1);

    if(cycle_ret != 0){
        printf("bake failed\n");
        bake_free(bake);
        return EXIT_FAILURE;
    }
    
    //  Analyze the bake_t starting at the specified target to set which
    //    targets need to be updated
    //
    //  If the update analysis fails
    //    print "bake failed" and exit with a failure
    int ret = bake_set_updates(bake, target);
    if(ret == -1){
        printf("bake failed\n");
        bake_free(bake);
        return EXIT_FAILURE;
    }

    //  If the update analysis indicates the target does not need to be updated
    //    Check if the target is an existing file
    //      If so, print the message "bake: file 'XXX' is up to date"
    //      If not, print the mssage "bake: nothing to be done for target 'XXX'"
    //    Then return/exit with a 0 exit code for a successful build
    if(ret == 0){
        if(access(target, F_OK) == 0){
            printf("bake: file '%s' is up to date\n", target);
        } else{
            printf("bake: nothing to be done for target '%s'\n", target);
        }

        bake_free(bake);
        return EXIT_SUCCESS;
    }

    //  Since some targets need to be updated, recursively run comands to update
    //    dependencies and targets counting the total number of rule updates
    //
    //  If updates fail (perhaps due to commands failing)
    //    print the message "bake failed" and exit with EXIT_FAILURE
    int updates = bake_do_updates(bake, target);
    if(updates == -1){
        printf("bake failed\n");
        bake_free(bake);
        return EXIT_FAILURE;
    }

    //  Otherwise, on a successful update, print the message
    //    "bake complete, NNN update(s) performed" 
    //    substituting in the number of updates (rules with commands run)
    //  
    //  return 0 for success
    printf("bake complete, %d update(s) performed\n", updates);

    bake_free(bake);
    return 0;
}