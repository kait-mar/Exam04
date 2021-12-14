#include "microshell.h"

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
            //printf("argv = %s\n", argv[i]);
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
    temp = head;
    if (argv[i])
    {
        temp->next = malloc(sizeof(t_lst));
        temp->next = init_lst(temp->next);
        temp->next->arg = malloc(sizeof(char *) * 100);
        temp = temp->next;
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

t_lst   *pipe_loop(t_lst *lst)
{
    t_lst   *temp = lst;

    while (temp && temp->meta == '|')
    {
        pipe(temp->fd);
        temp = temp->next;
    }
    return (lst);
}

t_lst   *pipe_core(t_lst *temp, char **env)
{
    pid_t   pid;
    int     status;
    t_lst   *lst;

    temp = temp->next;
    while (lst && temp && temp->meta == '|')
    {
        //pipe(fd);
        
        pid = fork(); //second fork
        if (pid < 0)
            exit(EXIT_FAILURE);
        else if (pid == 0)
        {
            lst = temp;
            temp = temp->next; //maybe
            //close(fd[1]); //fd[1] = lst->fd[1]
            dup2(lst->fd[0], 0);
            //close(fd[0]);
            /**************/
            //close(temp->fd[0]);
            dup2(temp->fd[1], 1); //fd[1] = temp->fd[1] ..
            //close(fd[1]);
            builtin(temp, env);
        }
        else
        {
            waitpid(pid, &status, WUNTRACED);
            close(lst->fd[0]);
            close(temp->fd[1]);
        }
        //temp = temp->next;
    }

    return (temp);
}

t_lst    *piping(t_lst *temp, char **env)
{
    pid_t   pid;
    int     status;
    int in = 1;
    t_lst   *lst = temp;
    t_lst   *back = temp;
    int old_save = dup(STDIN_FILENO);

    //pipe_loop(temp);
    pipe(fd);
    pid = fork(); //first fork
    if (pid < 0)
        exit(EXIT_FAILURE);
    else if (pid == 0)
    {
        close(lst->fd[0]);
        dup2(lst->fd[1], 1);
        close(lst->fd[1]);
        builtin(temp, env);
        exit(EXIT_SUCCESS); //here
    }
    else
    {
        waitpid(pid, &status, WUNTRACED);
    }
    //close(lst->fd[1]);

    temp = pipe_core(temp, env);

    // temp = temp->next;
    // temp = temp->next;


    // close(lst->next->fd[1]);
    // dup2(lst->next->fd[0], 0);
    // close(lst->next->fd[0]);
    close(fd[1]);
    dup2(fd[0], 0);
    close(fd[0]);
    builtin(temp, env);
    //close_all(back);
    dup2(old_save, 0);
    close(0);
    return (temp);
}

void    close_all(t_lst *lst)
{
    while (lst && lst->meta == '|')
    {
        close(lst->fd[0]);
        close(lst->fd[1]);
        lst = lst->next;
    }
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
}

void    execute_lst(t_lst *lst, char **env)
{
    t_lst   *temp;
    pid_t   pid;
    temp = lst;
    while (temp != NULL)
    {
        if (temp->meta != '|')
        {
            builtin(temp, env);
        }
        else
            temp = piping(temp, env);
        temp = temp->next;
    }
}

int main(int argc, char **argv, char **env)
// int main()
{
    t_lst    *lst;

    // int     argc = 7;
    // char    *argv[3];
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
    // argv[2] = NULL;


    if (argc == 1)
        return (0);
    //add errors to parsing
    lst = parse_error(argv);
    // print_lst(lst);
    // printf("**************\n");
    execute_lst(lst, env);
    return (0);
}


/* piping :
    pipe(fd);
    fork(); //first fork
    in child pid == 0
    {
        close(fd[0]);
        dup2(fd[1], 1);
        close(fd[1]);
        execute..
        exit()
    }        
    close(fd[1]);

    fork(); //second fork
    in child pid == 0
    {
        close(fd[1]);
        dup2(fd[0], 0);
        close(fd[0]);
        execute
    }
    close(fd[1]);
    close(fd[0]);
*/
