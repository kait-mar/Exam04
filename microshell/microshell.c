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
    printf("B\n");
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
        j = 0;
        while (argv[i] && !strcmp(argv[i], ";") && !strcmp(argv[i], "|"))
        {
                            printf("argv = %s\n", argv[i]);
            head->arg[j++] = ft_strdup(argv[i++]);
        }
    }
    printf("Asdd\n");
    temp = head;
       temp->next = malloc(sizeof(t_lst));
        temp->next = init_lst(temp->next);
        temp->next->arg = malloc(sizeof(char *) * 100);
        temp = temp->next;
    while (argv[i])
    {
        if (check != 1)
            temp->cmd = ft_strdup(argv[i++]);
        if (strcmp(argv[i], ";") == 0)
        {
            check = 0;
            temp->meta = ';';
            printf("Enter 1\n");

            temp->next = malloc(sizeof(t_lst));
            temp->next = init_lst(temp->next);
            temp->next->arg = malloc(sizeof(char *) *  1000);
            temp = temp->next;
            i++;
        }
        else if (strcmp(argv[i], "|") == 0)
        {
            printf("Enter2 \n");
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
            j = 0;
            printf("Enter \n");
                printf("argv = %s\n", argv[i]);
            while (argv[i] &&  strcmp(argv[i], ";") != 0 && strcmp(argv[i], "|") != 0)
            { 
                printf("argc    v = %s\n", argv[i]);
                temp->arg[j++] = ft_strdup(argv[i++]);
                printf("ééé\n");
            }
            printf("End\n");
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
        printf("cmd ==> %s\n", temp->cmd);
        printf("arg ==> %s\n", temp->arg[0]);
        printf("meta ==>; %c\n", temp->meta);
        temp = temp->next;
    }
}
 int main(int argc, char **argv, char **env)
//int main()
{
    t_lst    *lst;
    // int     argc = 7;
    // char    **argv;
    // argv[1] = "/bin/ls";
    // argv[2] = "|";
    // argv[3] = "usr/bin/ls";
    // argv[4] = "usr/bin/grep";
    // argv[5] = "microshell";
    // argv[6] = NULL;

    if (argc == 1)
        return (0);
    printf("Before\n");
    lst = parse_error(argv);
    printf("A\n");
    print_lst(lst);
    return (0);
}
;
