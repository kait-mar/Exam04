#include "microshell.h"

int     ft_strlen(char *s)
{
    int i = 0;
    while (s[i])
        i++;
    return (i);
}

void    print_error(char *s1, char *s2)
{
    if (s1)
        write(2, s1, ft_strlen(s1));
    if (s2)
        write(2, s2, ft_strlen(s2));
    write(2, "\n", 1);
}

char    *ft_strdup(char *s)
{
    int i = 0;
    char    *str = malloc(ft_strlen(s) + 1);

    while (s[i])
    {
        str[i] = s[i];
        i++;
    }
    str[i] = '\0';
    return (str);
}

int till_meta(char **argv, int start)
{
    int i = 0;
    while (argv[start] && strcmp(argv[start], ";") && strcmp(argv[start], "|"))
    {
        start++;
        i++;
    }
    return (i + 1);
}

t_lst   *init_lst(t_lst *lst, int k)
{
    lst->arg[0] = NULL;
    lst->cmd = NULL;
    lst->meta = 0;
    lst->next = NULL;
    if (k > 1)
        lst->arg[1] = NULL;
    return (lst);
}

void    pipe_loop(t_lst *lst)
{
    while (lst && lst->meta == '|')
    {
        pipe(lst->fd);
        lst = lst->next;
    }
}

t_lst   *piping(t_lst *temp, char **env)
{
    pid_t   pid;
    int save = dup(STDIN_FILENO);

    pipe_loop(temp);
    while (temp)
    {
        pid = fork();
        if (pid == 0)
        {
            if (temp->meta == '|')
                dup2(temp->fd[1], 1);
            if (temp->previous && temp->previous->meta == '|')
                dup2(temp->previous->fd[0], 0);
            builtin(temp, env);
            exit(EXIT_SUCCESS);
        }
        else
        {
            waitpid(pid, NULL, WUNTRACED);
            if (temp->meta == '|')
                close(temp->fd[1]);
            if (temp->previous && temp->previous->meta == '|')
                close (temp->previous->fd[0]);
            temp = temp->next;
            if (temp && temp->previous && temp->previous->meta != '|')
                break ;
        }
    }
    dup2(save, 0);
    return (temp);
}

void    execute_lst(t_lst *lst, char **env)
{
    while (lst)
    {
        if (lst->meta == '|')
        {
            lst = piping(lst, env);
        }
        else
        {
            builtin(lst, env);
            lst = lst->next;
        }
    }
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
    if (chdir(str[1]) == -1)
        print_error("error: cd: cannot change directory to ", str[1]);
}

void    builtin(t_lst *temp, char **env)
{
    if (temp->cmd == NULL)
        return ;
    if (strcmp(temp->cmd, "cd") == 0)
    {
        cd(temp->arg);
        return ;
    }
    pid_t  pid = fork();
    if (pid == 0)
    {
        if (execve(temp->cmd, temp->arg, env) == -1)
        {
            print_error("error: cannot execute ", temp->cmd);
            exit(EXIT_FAILURE);
        }
    }
    else
        waitpid(pid, NULL, WUNTRACED);
}

t_lst   *parse_error(char **argv)
{
    int i = 1;
    int check = 0;
    int k = 0;
    t_lst   *head;
    t_lst   *temp;
    int j;

    head = malloc(sizeof(t_lst));
    k = till_meta(argv, i);
    head->arg = malloc(sizeof(char *) * k);
    head = init_lst(head, k);
    if (argv[i] && strcmp(argv[i], ";"))
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
            head->arg[j++] = argv[i++];
        }
        head->arg[j] = NULL;
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
    }
    temp = malloc(sizeof(t_lst));
    k = till_meta(argv, i);
    temp->arg = malloc(sizeof(char *) * k);
    temp = init_lst(temp, k);
    head->previous = NULL;
    temp->previous = head;
    head->next = temp;
    while (argv[i])
    {
        if (check != 1 && strcmp(argv[i], ";"))
        {

            temp->cmd = ft_strdup(argv[i++]);
            temp->arg[0] = temp->cmd;
        }
        if (argv[i] && strcmp(argv[i], ";") == 0)
        {
            temp->meta = ';';
            i++;
            check = 0;
            k = till_meta(argv, i);
            temp->next = malloc(sizeof(t_lst) * k);
            temp->next->arg = malloc(sizeof(char *) * k);
            temp->next->previous = temp;
            temp = temp->next;
            temp = init_lst(temp, k);
        }
        else if (argv[i] && strcmp(argv[i], "|") == 0)
        {
            temp->meta = '|';
            i++;
            check = 0;
            k = till_meta(argv, i);
            temp->next = malloc(sizeof(t_lst) * k);
            temp->next->arg = malloc(sizeof(char *) * k);
            temp->next->previous = temp;
            temp = temp->next;
            temp = init_lst(temp, k);
        }
        else
        {
            check = 1;
            j = 1;
            while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
            {
                temp->arg[j++] = ft_strdup(argv[i++]);
            }
            temp->arg[j] = NULL;
        }
    }
    return (head);
}

void    print_lst(t_lst *lst)
{
    t_lst   *temp = lst;
    while (temp != NULL)
    {
        int i = 0;
        printf("cmd ==> %s\n", temp->cmd);
        while (temp->arg[i] != NULL)
            printf("%s ", temp->arg[i++]);
        if (temp->previous)
            printf("\npprevious %s\n", temp->previous->cmd);
        printf("\nmeta ==> %c\n", temp->meta);
        printf("---------------\n");
        temp = temp->next;
    }
}

int main(int argc, char **argv, char **env)
{
    t_lst   *lst;

    if (argc < 2)
        return (0);
    lst = parse_error(argv);
    // print_lst(lst);
    execute_lst(lst, env);
    return (0);
}
