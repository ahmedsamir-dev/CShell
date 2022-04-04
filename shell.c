#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>


char **get_input(char *);
int cd(char *);

int main() {
    char **command;
    char *input;
    pid_t child_pid;
    int stat_loc;

    while (1) {
        input = readline("ahmed-shell> ");
        command = get_input(input);

        if (!command[0]) {      // Handle empty commands
            free(input);
            free(command);
            continue;
        }

        /* Excute the "cd" command in the main process to change the directory */
        if (strcmp(command[0], "cd") == 0) {
            if (cd(command[1]) < 0) {
                perror(command[1]);
            }

            // Skip the Forking and continue to the next input command
            continue;
        }


        /* Forking a cild process to excute the command in,
            inorder to take the user input and to avoid main process crash */
        child_pid = fork();

        if (child_pid == 0) {
            /* That means the code running in the child process,
            so excute the given command */

            if (execvp(command[0], command) < 0) {
                // If the child process failed to execute the command, terminate it
                system(command[0]);
                perror(command[0]);
                exit(1); // Terminate the child process
            }
            
            printf("This won't be printed if execvp is successul\n");

        } else if (child_pid > 0) {
            /* That means the code running in the main (parent) process,
                so wait the child process to excute the command */
            waitpid(child_pid, &stat_loc, WUNTRACED);
        
        } else if (child_pid < 0) {
            // Error in froking the child process, so exit the process
            perror("Faild to fork child process");
            exit(1); // Terminate the Main Process
        }

        //Deaclocting the allocated memory after each command execution
        free(input);
        free(command);
    }

    return 0;
}

char **get_input(char *input) {
    char **command = malloc(8 * sizeof(char *));
    char *separator = " ";
    char *parsed;
    int index = 0;

    //ls -a -l
    parsed = strtok(input, separator);
    while (parsed != NULL) {
        command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}



int cd(char *path) {
    return chdir(path);
}

//gcc -o shell shell.c -lreadline && ./shell