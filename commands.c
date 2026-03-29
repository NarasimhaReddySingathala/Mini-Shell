/*builtin commands*/
#include "header.h"
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
                    "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
                    "exit", "exec", "shopt", "caller","vi", "true", "type","hash","clear", "bind", "help", NULL};

extern char *external_commands[200];
extern int status;

void extract_external_commands()
{
    // step1 open a file and read till \n
    // step2 take the length of the command
    // step3 allocating the memory based on the length
    // store the command into external_command[][]

    int fd = open("external_command.txt", O_RDONLY);

    char ch;
    int i = 0, j = 0;

    external_commands[i] = malloc(30);

    while (read(fd, &ch, 1) > 0)
    {
        // printf("hello\n");
        if (ch != '\n')
        {
            external_commands[i][j++] = ch;
        }
        else
        {
            external_commands[i][j] = '\0';
            i++;
            j = 0;
            external_commands[i] = malloc(30);
        }
    }

    external_commands[i] = NULL;

    close(fd);
}

char *get_command(char *input_string)
{
    // get the first word -> input string => ls -l ,return ls
    static char cmd[20];
    int i = 0;

    while (input_string[i] != ' ' && input_string[i] != '\0')
    {
        cmd[i] = input_string[i];
        i++;
    }

    cmd[i] = '\0';

    return cmd;
}

int check_command_type(char *cmd)
{
    // check cmd is internal cmd or not
    // campare smd arr with built_in[][]
    // return  BUILT_IN
    // char *builtins[] = {"exit", "cd", "pwd", NULL};

    int i = 0;

    while (builtins[i] != NULL)
    {
        if (strcmp(cmd, builtins[i]) == 0)
        {
            return 1;
        }
        i++;
    }

    // compare cmd arr with extwrnal_commands[][]
    // if it is matching

    for (i = 0; i < 200; i++)
    {
        if (external_commands[i] == NULL)
        {
            break;
        }

        if (strncmp(cmd, external_commands[i], strlen(cmd)) == 0)
        {
            return 2;
        }
    }

    return 3;
}

void execute_internal_commands(char *input_str)
{
    /*
       i) exit
       ii) cd
       iii) pwd
       */

    // check the input_str contains exit or not
    // if it is exit
    // exit(0);
    if (strcmp(input_str, "exit") == 0)
    {
        exit(0);
    }

    // check input_str contains cd or not
    // if its cd

    // chdir(input_str+3);
    else if (strncmp(input_str, "cd", 2) == 0)
    {
        chdir(input_str + 3);
    }

    // check the input_str contains pwd or not
    // if its pwd

    // char buf[50];
    // getcwd(buf,50);
    // pf -> buf
    else if (strcmp(input_str, "pwd") == 0)
    {
        char buf[100];
        getcwd(buf, sizeof(buf));
        printf("%s\n", buf);
    }
    else if(strncmp(input_str, "echo", 4) == 0)
    {
        echo(input_str + 5, status);
    }
    else if(strcmp(input_str, "clear") == 0)
    {
        system("clear");  // Clear's the terminal
    }
}
void echo(char *input_str, int status)
{
    if(strcmp(input_str, "$SHELL") == 0)
    {
        printf("%s\n", getenv("SHELL"));  // Prints the shell 
    }
    else if(strcmp(input_str, "$$") == 0)
    {
        printf("%d\n", getpid());  // Pid of the process
    }
    else if(strcmp(input_str, "$?") == 0)
    {
        printf("%d\n", WIFEXITED(status));  // Exit status of the previous process
    }
    else
    {
        printf("%s\n", input_str);
    }
}

void execute_external_commands(char *input_str)
{
    char *argv[50], buffer[50];

    int i = 0, j = 0, index = 0;

    while (input_str[i] != '\0')
    {
        if (input_str[i] != ' ')
        {
            buffer[j] = input_str[i];
            j++;
        }
        else
        {
            buffer[j] = '\0';
            argv[index] = malloc(strlen(buffer) + 1);
            strcpy(argv[index], buffer);
            j = 0;
            index++;
        }
        i++;
    }

    buffer[j] = '\0';
    argv[index] = malloc(strlen(buffer) + 1);

    strcpy(argv[index], buffer);

    j = 0;
    index++;

    argv[index] = NULL;

    // call your function
    execute_external_commands_argu(input_str, index, argv);
}
void execute_external_commands_argu(char *input_str, int argc, char *argv[])
{
    // logic for converting (1d arr)input_str[] to (2d arr)argv[][]
    // convert(input_str);

    // step:2 check the pipe is present or not
    int cmd_pos[argc];
    int cmd_count = 1;
    cmd_pos[0] = 0;

    // cmd_pos[0] = 1;

    for (int i = 0; argv[i]; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            argv[i] = NULL;
            cmd_pos[cmd_count++] = i + 1;
        }
    }

    int fd[2];

    if (cmd_count > 1)
    {

        for (int i = 0; i < cmd_count; i++)
        {
            if (i < cmd_count - 1)
            {
                pipe(fd);
            }
            int ret = fork();
            if (ret > 0)
            {
                if (i < cmd_count - 1)
                {
                    close(fd[1]);
                    dup2(fd[0], 0);
                }
            }
            else if (ret == 0)
            {
                if (i < cmd_count - 1)
                {
                    close(fd[0]);
                    dup2(fd[1], 1);
                    execvp(argv[cmd_pos[i]], argv + cmd_pos[i]);
                }
                else
                {
                    execvp(argv[cmd_pos[i]], argv + cmd_pos[i]);
                }
            }
            else
            {
                perror("fork\n");
                exit(1);
            }
        }

        for (int i = 0; i < cmd_count; i++)
        {
            wait(NULL);
        }
    }
    else
    {
        execvp(argv[0], argv + 0);
    }
}
