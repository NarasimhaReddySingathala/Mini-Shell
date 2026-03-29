#include "header.h"

extern P_list *head;
extern P_list *tail;

extern char prompt[20];
char input_string[50];

extern int status;
 int pid;

int job_count = 0;

void signal_handler(int sig_num)
{
    if(sig_num == SIGINT)
    {
        if(pid == 0)  // When Child is not present
        {
            printf("\n");
            printf(ANSI_COLOR_GREEN);
            printf("%s", prompt);
            printf(ANSI_COLOR_RESET);
        }
        fflush(NULL);
    }
    else if(sig_num == SIGTSTP)
    {
        if(pid == 0)  // When Child is not present
        {
            printf("\n");
            printf(ANSI_COLOR_GREEN);
            printf("%s", prompt);
            printf(ANSI_COLOR_RESET);
        }
        else if(pid > 0)  // When Child is present
        {
            insert_at_first(&head, &tail, pid, input_string);
            job_count++;
            printf("\n");
        }
        
        fflush(NULL);
    }
    else if(sig_num == SIGCHLD)
    {
        waitpid(-1, &status, WNOHANG);  // Waits till any child to terminate to clear the resources
    }
}