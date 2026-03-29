#include <stdio.h>
#include "header.h"
#include <string.h>


P_list *head = NULL;
P_list *tail = NULL;
// extern char prompt;
extern int status;
extern int job_count;

char *external_commands[200];

// void my_handler(int signum)
// {
//     if (signum == SIGINT)
//     {
//         printf("prompt");
//     }
// }


void scan_input(char *prompt, char *input_str)
{
    system("clear");
    extract_external_commands();

    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler);
    while (1)
    {
        bzero(input_str, 50);  // Updating the input string with NULL
        
        fflush(stdout);  // Clearing the Standard output buffer
        // print the prompt
        printf(ANSI_COLOR_GREEN);
        printf("%s", prompt);
        printf(ANSI_COLOR_RESET);

        scanf(" %[^\n]", input_str);

        getchar();
        // step 1 = check input_str is PS1 = Or not (strcmp)
        if(input_str[0] == '\n' || input_str[0] == '\0')
        {
            continue;
        }
        if (strncmp(input_str, "PS1=", 4) == 0)
        {

            // step 2 = if it is PS1 = goto to step 3
            // step 3 = check input str[4] != space
            if (input_str[4] != ' ' && input_str[4] != '\0')
            {
                strcpy(prompt, input_str + 4);
            }
            else
            {
                //      print error
                printf("Insufficiant command 're check the command\n");
            }
        }
        else if(strcmp(input_str, "jobs") == 0)  // Displays all the stopped process list 
        {
            print_list(tail);  
        }
        else if(strcmp(input_str, "fg") == 0)  // Continue the stopped process in Foreground
        {
            if (head == NULL)
            {
                printf("fg : No current job to continue\n");
                continue;
            }

            printf("%s\n", head->command);

            pid_t pid = head->pid;

            kill(pid, SIGCONT);  // Continue the stopped process

            waitpid(pid, &status, WUNTRACED);  // Wait for foreground process

            if (WIFEXITED(status) || WIFSIGNALED(status))  // If process exited, remove from job list 
            {
                delete_at_first(&head, &tail);
                job_count--;
            }
        }
        else if(strcmp(input_str, "bg") == 0)  // Continue the stopped process in Background
        {
            if (head == NULL)
            {
                printf("bg : No current job to continue\n");
                continue;
            }

            printf("[%d]+  %s  &\n", job_count, head->command);

            kill(head->pid, SIGCONT);

            delete_at_first(&head, &tail);
            job_count--;
        }

        // if it's not PS1 =
        else
        {
            // commands
            // printf("Execute: %s\n", input_str);
            char *cmd = get_command(input_str);
            int ret = check_command_type(cmd);
            if (ret == BUILTIN)
            {
                // logic for internal_commands
                execute_internal_commands(input_str);
            }
            else if (ret == EXTERNAL)
            {
                // printf("narasimha\n");
                // logic for external command
                int pid = fork();
                if (pid > 0)
                {
                    // parent
                    wait(NULL);
                }
                else if (pid == 0)
                {
                    // printf("narasimha\n");
                    execute_external_commands(input_str);
                    exit(0);
                }
            }
            else
            {
                // return NO_COMMAND;
                printf("no command found\n");
            }

            memset(input_str,'\0',sizeof(input_str));
        }
    }
}