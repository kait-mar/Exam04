#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct lst
{
    char    *cmd;
    char    *arg;
    char    meta;
    t_lst   *next;
}   t_lst;
char    *my_substr(char *str, char a, char b)
{
    char    *cmd;
    int     i = 0;

    while (str[i] && str[i] != a && str[i] != b)
        i++;
    //cmd
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
    lst->arg = NULL;
    lst->cmd= NULL;
    lst->meta = 0;
    return (lst);
}

int    parse_error(char **argv, t_lst **cmd)
{
    t_lst   *temp;
    int     i = 1;

    while (argv[i])
    {
        temp = malloc(sizeof(t_lst));
        temp = init_lst(temp);
        temp->cmd = ft_strdup(argv[i++]);
        if (strcmp(argv[i], ";") == 0)
        {
            temp->meta = ';';
            i++;
        }
        else if (strcmp(argv[i], "|") == 0)
        {
            temp->meta = '|';
            i++;
        }
        else
            temp->arg = ft_strdup(argv[i++]);
    }
}

int main(int argc, char **argv, char **env)
{
    t_lst    *cmd;

    if (argc == 1)
        return (0);
    cmd = malloc(sizeof(t_lst));
    parse_error(argv[1], &cmd);
}
