#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

typedef struct lst
{
    char    *cmd;
    char    **arg;
    char    meta;
    struct lst   *next;
    int         fd[2];
}   t_lst;


int     ft_strlen(char *str)
{
    int i = 0;
    while (str[i])
        i++;
    return (i);
}

char    *ft_strdup(char *str)
{
    int i = 0;
    char    *s;

    s = malloc(ft_strlen(str) + 1);
    while (str[i])
    {
        s[i] = str[i];
        i++;
    }
    s[i] = '\0';
    return (s);
}

t_lst   *init_lst(t_lst *lst)
{
    //lst->arg = NULL;
    lst->cmd= NULL;
    lst->meta = 0;
    lst->next = NULL;
    return (lst);
}

t_lst    *parse_error(char **argv)
{
    t_lst   *temp;
    t_lst   *head;
    int     i = 1;
    int check = 0;
    int     j;

    head = malloc(sizeof(t_lst));
    head = init_lst(head);
    head->arg = malloc(sizeof(char *) *  1000);
    head->cmd = ft_strdup(argv[i++]);
    head->arg[0] = head->cmd;
    if (strcmp(argv[i], ";") == 0)
    {
        head->meta = ';';
        i++;
    }
    else if (strcmp(argv[i], "|") == 0)
    {
        head->meta = '|';
        i++;
    }
    else
    {
        j = 1;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
        {
            //printf("argv = %s\n", argv[i]);
            head->arg[j++] = ft_strdup(argv[i++]);
        }
    }
    if (strcmp(argv[i], ";") == 0)
    {
        head->meta = ';';
        i++;
    }
    else if (strcmp(argv[i], "|") == 0)
    {
        head->meta = '|';
        i++;
    }
    temp = head;
    temp->next = malloc(sizeof(t_lst));
    temp->next = init_lst(temp->next);
    temp->next->arg = malloc(sizeof(char *) * 100);
    temp = temp->next;
    while (argv[i])
    {
        if (check != 1)
        {
            temp->cmd = ft_strdup(argv[i++]);
            temp->arg[0] = temp->cmd;
        }
        if (argv[i] && strcmp(argv[i], ";") == 0)
        {
            check = 0;
            temp->meta = ';';
            temp->next = malloc(sizeof(t_lst));
            temp->next = init_lst(temp->next);
            temp->next->arg = malloc(sizeof(char *) *  1000);
            temp = temp->next;
            i++;
        }
        else if (argv[i] && strcmp(argv[i], "|") == 0)
        {
            check = 0;
            temp->meta = '|';
            temp->next = malloc(sizeof(t_lst));
            temp->next = init_lst(temp->next);
            temp->next->arg = malloc(sizeof(char *) *  1000);
            temp = temp->next;
            i++;
        }
        else
        {
            j = 1;
            while (argv[i] &&  strcmp(argv[i], ";") != 0 && strcmp(argv[i], "|") != 0)
            {
                temp->arg[j++] = ft_strdup(argv[i++]);
            }
            check = 1;
        }
    }
    temp->next = NULL;
    return (head);
}

void    print_lst(t_lst *lst)
{
    t_lst   *temp = lst;
    while (temp != NULL)
    {
        int i = 0;
        printf("cmd ==> %s\n", temp->cmd);
        while (temp->arg[i])
            printf("%s ", temp->arg[i++]);
        printf("\nmeta ==> %c\n", temp->meta);
        printf("---------------\n");
        temp = temp->next;
    }
}

void    print_error(char *s1, char *s2)
{
    if (s1)
        write(2, s1, ft_strlen(s1));
    if (s2)
        write(2, s2, ft_strlen(s2));
    write(2, "\n", 1);
}

void    cd(char **str)
{
    int i = 0;
    while (str[i])
        i++;
    if (i > 2)
    {
        print_error("error: cd: bad arguments", NULL);
        return ;
    }
    if (chdir(str[1]) < 0)
    {
        print_error("error: cd: cannot change directory to ", str[1]);
    }
}

void    execute_lst(t_lst *lst, char **env)
{
    t_lst   *temp;
    pid_t   pid;
    int     status;
    temp = lst;
    while (temp != NULL)
    {
        if (strcmp("cd", temp->cmd) == 0)
        {
            cd(temp->arg);
            temp = temp->next;
            continue ;
        }
        pid = fork();
        if (pid < 0)
            exit(EXIT_FAILURE);
        else if (pid == 0){
            if (execve(temp->cmd, temp->arg, env) == -1)
            {
                print_error("error: cannot execute ", temp->cmd);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            waitpid(pid, &status, WUNTRACED);
        }
        temp = temp->next;
    }
}

int main(int argc, char **argv, char **env)
//int main()
{
    t_lst    *lst;

    // int     argc = 7;
    // char    *argv[10];
    // argv[1] = "/bin/echo";
    // argv[2] = "hello";
    // argv[3] = "yes";
    // argv[4] = ";";
    // argv[5] = "/bin/echo";
    // argv[6] = "hell";
    // argv[7] = "|";
    // argv[8] = "/bin/ls";
    // argv[4] = "usr/bin/grep";
    // argv[5] = "microshell";
    //argv[9] = NULL;


    if (argc == 1)
        return (0);
    //add errors to parsing
    lst = parse_error(argv);
    // print_lst(lst);
    // printf("**************\n");
    execute_lst(lst, env);
    return (0);
}
