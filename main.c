#include <stdio.h>
#include "header.h"
char prompt[] = "minishell$:";
char input_str[50];
int main()
{
        // system("clear");
        // char prompt[] = "minishell$:";
        // char input_str[25];

        scan_input(prompt, input_str);

        return 0;
}