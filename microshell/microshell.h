#ifndef MICROSHELL_H
#define MICROSHELL_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>


typedef struct lst
{
    char    *cmd;
    char    **arg;
    char    meta;
    struct lst   *next;
    struct lst     *previous;
    int         fd[2];
}   t_lst;

int     fd[2];

void    builtin(t_lst *temp, char **env);
char    *ft_strdup(char *str);
t_lst   *init_lst(t_lst *lst);
t_lst    *parse_error(char **argv);
void    print_lst(t_lst *lst);
void    print_error(char *s1, char *s2);
void    cd(char **str);
t_lst   *pipe_loop(t_lst *lst);
t_lst   *pipe_core(t_lst *temp, char **env);
t_lst    *piping(t_lst *temp, char **env);
void    close_all(t_lst *lst);
void    execute_lst(t_lst *lst, char **env);


#endif