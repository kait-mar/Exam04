#include "microshell.h"

t_lst      *piping(t_lst *temp, char **env)
{
    pid_t   pid;
    int     status;

    int save = dup(STDIN_FILENO);
    pipe_loop(temp);
    while (temp)
    {
        pid = fork();
        if (pid < 0)
        {
            print_error("error: fatal", NULL);
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            if (temp->meta == '|')
            {
                dup2(temp->fd[1], 1);
                close(temp->fd[1]);
            }
            if (temp->previous && temp->previous->meta == '|')
            {
                dup2(temp->previous->fd[0], 0);
                close(temp->previous->fd[0]);
            }
                
            builtin(temp, env);
            exit(EXIT_SUCCESS);
        }
        else
        {
            waitpid(pid, &status, WUNTRACED);
            if (temp->meta == '|')
                close(temp->fd[1]);
            if (temp->previous && temp->previous->meta == '|')
                close (temp->previous->fd[0]);
            temp = temp->next;
            if (temp && temp->previous && temp->previous->meta != '|')
                break;
        }
    }
    dup2(save, 0);
    return (temp);
}


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

int     till_meta(char **argv, int start)
{
    int i = 0;

    // printf("intered with [%d]\n", start);
    while (argv[start] && strcmp(argv[start], ";") && strcmp(argv[start], "|"))
    {
        start++; i++;
    }
    // printf("allocated [%d]\n", i -1);
    return (i - 1);
}


/*
t_lst    *parse_error(char **argv)
{
    t_lst   *temp;
    t_lst   *head;
    int     i = 1;
    int check = 0;
    int     j;

    head = malloc(sizeof(t_lst));
    head = init_lst(head);
    head->arg = malloc(sizeof(char *) *  (100));
    head->cmd = ft_strdup(argv[i++]);
    head->arg[0] = head->cmd;
    if (argv[i] && strcmp(argv[i], ";") == 0)
    {
        head->meta = ';';
        i++;
    }
    else if (argv[i] && strcmp(argv[i], "|") == 0)
    {
        head->meta = '|';
        i++;
    }
    else
    {
        j = 1;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
        {
            head->arg[j++] = ft_strdup(argv[i++]);
        }
    }
    if (argv[i] && strcmp(argv[i], ";") == 0)
    {
        head->meta = ';';
        i++;
    }
    else if (argv[i] && strcmp(argv[i], "|") == 0)
    {
        head->meta = '|';
        i++;
    }
    head->previous = NULL;
    temp = head;
    if (argv[i])
    {
        temp->next = malloc(sizeof(t_lst));
        temp->next = init_lst(temp->next);
        temp->next->arg = malloc(sizeof(char *) *  (100));
        temp = temp->next;
        temp->previous = head;
    }
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
            temp->next->arg = malloc(sizeof(char *) *  (100));
            temp->next->previous = temp;
            temp = temp->next;
            i++;
        }
        else if (argv[i] && strcmp(argv[i], "|") == 0)
        {
            check = 0;
            temp->meta = '|';
            temp->next = malloc(sizeof(t_lst));
            temp->next = init_lst(temp->next);
            temp->next->arg = malloc(sizeof(char *) * (100));
            temp->next->previous = temp;
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
}*/

//////////////this is it
t_lst   *parse_error(char **argv)
{
    t_lst   *head;
    t_lst   *temp;
    int j;

    int i = 1;
    head = malloc(sizeof(t_lst));
    head->arg = malloc(sizeof(char *) * 100/*till_meta(argv, i)*/);
    head = init_lst(head);
    if (argv[i])
    {
        head->cmd = ft_strdup(argv[i++]);
        head->arg[0] = head->cmd;
    }
    if (argv[i] && strcmp(argv[i], ";") == 0)
    {
        i++;
        head->meta = ';';

    }
    else if (argv[i] && strcmp(argv[i], "|") == 0)
    {
        i++;
        head->meta = '|';
    }
    else
    {
        j = 1;
        while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
        {
            head->arg[j++] = ft_strdup(argv[i++]);
        }
        head->arg[j] = NULL;
    }
    if (argv[i] && strcmp(argv[i], ";") == 0)
    {
        i++;
        head->meta = ';';
    }
    else if (argv[i] && strcmp(argv[i], "|") == 0)
    {
        i++;
        head->meta = '|';
    }
    head->next = malloc(sizeof(t_lst));
    head->next->arg = malloc(sizeof(char *) * 100/*till_meta(argv, i)*/);
    head->previous = NULL;
    head->next = init_lst(head->next);
    temp = head->next;
    temp->previous = head;
    int check = 0;
    while (argv[i])
    {
        if (check != 1)
        {
            temp->cmd = ft_strdup(argv[i++]);
            temp->arg[0] = temp->cmd;
        }
        if (argv[i] && strcmp(argv[i], ";") == 0)
        {
            i++;
            check = 0;
            temp->meta = ';';
            temp->next = malloc(sizeof(t_lst));
            temp->next->arg = malloc(sizeof(char *) * 100/*till_meta(argv, i)*/);
            temp->next->previous = temp;
            temp->next = init_lst(temp->next);
            temp = temp->next;
        }
        else if (argv[i] && strcmp(argv[i], "|") == 0)
        {
            i++;
            check = 0;
            temp->meta = '|';
            temp->next = malloc(sizeof(t_lst));
            temp->next->arg = malloc(sizeof(char *) * 100/*till_meta(argv, i)*/);
            temp->next->previous = temp;
            temp->next = init_lst(temp->next);
            temp = temp->next;
        }
        else
        {
            j = 1;
            check = 1;
            while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
            {
                temp->arg[j++] = ft_strdup(argv[i++]);
            }
            temp->arg[j] = NULL;
        }
    }
    temp->next = NULL;
    return(head);
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
        if (temp->previous)
            printf("\npprevious %s\n", temp->previous->cmd);
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

t_lst   *pipe_loop(t_lst *lst)
{
    t_lst   *temp = lst;

    while (temp && temp->meta == '|')
    {
        if (pipe(temp->fd) < 0)
            print_error("error: fatal", NULL);
        temp = temp->next;
    }
    return (lst);
}

void    builtin(t_lst *temp, char **env)
{
    int status;
    pid_t   pid;

    if (temp->cmd && strcmp("cd", temp->cmd) == 0)
        {
            cd(temp->arg);
            temp = temp->next;
            return ;

        }
    if (temp->cmd && strcmp(";", temp->cmd) == 0)
    {
        print_error("error bash", NULL);
        return ;
    }
        pid = fork();
        if (pid < 0)
        {
            print_error("error: fatal", NULL);
            exit(EXIT_FAILURE);
        }
        else if (pid == 0){
            if (execve(temp->cmd, temp->arg, env) == -1)
            {
                print_error("error: cannot execute ", temp->cmd);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if (waitpid(pid, &status, WUNTRACED) < 0)
                print_error("error: fatal", NULL);
        }
}

void    execute_lst(t_lst *lst, char **env)
{
    t_lst   *temp;

    temp = lst;
    while (temp != NULL)
    {
        if (temp->meta != '|')
        {
            builtin(temp, env);
            temp = temp->next;
        }
        else
            temp = piping(temp, env);
    }
}
// int main()
int main(int argc, char **argv, char **env)
{
    t_lst    *lst;

    // int     argc = 10;
    // char    *argv[10];
    // argv[1] = "/usr/bin/ls";
    // argv[2] = "hello";
    // argv[3] = "yes";
    // argv[4] = ";";
    // argv[5] = "/bin/echo";
    // argv[6] = "hell";
    // argv[7] = "|";
    // argv[8] = "/bin/ls";
    // argv[4] = "usr/bin/grep";
    // argv[5] = "microshell";
    // argv[9] = NULL;
    if (argc == 1)
        return (0);
    lst = parse_error(argv);
    print_lst(lst);
    // execute_lst(lst, env);
    return (0);
}
