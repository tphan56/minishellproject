#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h> // fork, execlp
#include <sys/wait.h>// man 2 wait
#include <sys/types.h> //pid
#include <signal.h> //sig int
#include <string.h> 
int main(int argc, char *argv[]){
    (void) argc;
    (void) argv;
     char *input;
    char *args[100]; 
/* NOTE: while true, this will read the line
 * add nonempty input to history and can be accessed with up arrow,
 * and free the memory as we allocated the memory when we read the line.
 */ 

    signal(SIGINT, SIG_IGN); // this keeps the shell open when put ctrl + c
 
    while (1){
        input = readline("user> ");
        if (input == NULL){
            free(input);
            break;
        }
        // checks user input for spaces as well as if the user presses enter.
        // expected to keep continuing the shell line like normally.
        
        else if (input != NULL){
            if (input[0] != '\0'){
                add_history(input); // history of commands.  press up on the d pad.
            }else{
                free(input);
                continue;
        }
       }
// exit the minishell by typing "exit". frees the input and break.
        if (strcmp(input, "exit") == 0){
            free(input);
            break;
        
        }

/* NOTE: whole section uses execvp which automatically finds a program and runs with the
 * arguments you give it. instead of using execlp where we CANNOT predict what the user types
 * we use execvp for conveinience as a way where we don't have to predict how many words user
 * will type.
 */
        int i = 0;
        char *word = strtok(input, " \t"); //gets the first word

        while(word != NULL && i < 99) { //saved word where args[i] = word
            args[i] = word;
            i++;
            word = strtok(NULL, " \t"); //get the next word after iteration
        }

        args[i] = NULL; //all the way until args are NULL so no words left

        if(args[0] == NULL){ //make sure to free if there nothing. continue with the minishell prompt
            free(input);
            continue;
        }
/* NOTE: Here is a use of fork where the 
 * parent waits while the child is executing the commands
 * waitpid is a UNIX function that lets the parents sleep
 * while the child runs: NO BUSY WAIT
 */
         pid_t pid = fork();

// here in the child, calls execlp() as it reads the user typed input, and looks for ls
// in directories listed in PATH. Child becomes ls and list the files.
         if(pid == 0){
             signal(SIGINT, SIG_DFL); //child responds to crtl 
             execvp(args[0], args);
             perror("execvp");
             _exit(EXIT_FAILURE);
             break;
//this is the parent who waits for the child to finish. parent sleeps
        }else if (pid > 0) {
           int status;
           if (waitpid(pid, &status, 0) == -1){
               perror("waitpid");
          }else if (WIFSIGNALED(status)) {
                printf("Child (command) was killed by signal\n"); //shows if child is terminated
            }
             free(input);
         }else {
             fprintf(stderr, "Fork Failed!\n");
             return 1;
         }

    }

    return 0;
    
}
