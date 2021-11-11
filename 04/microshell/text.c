#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CD_E "error: cd: cannot change directory to "
#define ARG_E "error: cd: bad arguments\n"
#define FAT_E "error: fatal\n"
#define EXE_E "error: cannot execute "

#define ERROR 1
#define SUCCESS 0

#define NO_FLAG 0
#define PIPE 1
#define MULTI 2

typedef struct s_microshell
{
    char **args;
    char **env;
    int len;
    int flag;
}t_microshell;

t_microshell microshell;
int fd[2];

void ft_bzero(void *s, int n)
{
    while (n)  //!!!!!!!!!!!!!!!!
        ((unsigned char *)s)[--n] = 0;
}

int ft_strlen(char *s)
{
    int i = 0;
    if (!s)
        return(0);
    while (s[i])
        i++;
    return i;
}

void ft_putstr_fd(char *s, int fd)
{
    write(fd, s, ft_strlen(s));
}

void *ft_calloc(size_t count, size_t size)  ///！！!!!!！！！空指针
{
    void *tab;

    tab = (void *)malloc(count * size);
    if (!tab)
        return NULL;
    ft_bzero(tab, count * size);
    return (tab);
}

char *ft_strdup(char *s)
{
    char *tab;
    if (!s)
        return NULL;
    int i = ft_strlen(s);
    tab = ft_calloc(i+1, sizeof(char));
    if (!tab)
        return NULL;
    i = 0;
    while (s[i])
    {
        tab[i] = s[i];
        i++;
    }
    return tab;
}

void print_msg(char *msg1, char *msg2, int fd)
{
    if (msg1)
        ft_putstr_fd(msg1, fd);
    if (msg2)
    {
        ft_putstr_fd(msg2, fd);
        ft_putstr_fd("\n", fd);
    }
}

void free_args(void)
{
    int i = 0;
    while (microshell.args[i])
    {
        free(microshell.args[i]);
        microshell.args[i] = NULL;
        i++;
    }
    free(microshell.args);
    microshell.args = NULL;
}

void _exit_(char *msg1, char *msg2, int fd, int ret)
{
    print_msg(msg1, msg2, fd);
    free_args();
    exit(ret);
}

int arg_len(char **av)  //!!!!!!!!!!!/没有 i
{
    int i = 0;
    while (av[i])
    {
        if (ft_strlen(av[i])==1 && (av[i][0] == '|' || av[i][0] == ';'))
        {
            if(i == 0)
                i = 1;
            break;
        }
        i++;
    }
    return (i);
}

void ft_cd(void)
{
    if (microshell.len != 2)
    {
        print_msg(ARG_E, NULL, STDERR_FILENO);
        return;
    }
    if (chdir(microshell.args[1]) == -1)
    {
        print_msg(CD_E, microshell.args[1], STDERR_FILENO);
        return;
    }
}

void _parser(char **av, int i)
{
    ft_bzero(&microshell, sizeof(t_microshell));
    microshell.len = arg_len(av + i); //!!!!!
    microshell.args = ft_calloc(microshell.len+1, sizeof(char *));  //一个字符串的长度
    if (!microshell.args)
        _exit_(FAT_E, NULL, STDERR_FILENO, ERROR);
    int j = 0;
    while (av[i] && j < microshell.len)
    {
        microshell.args[j] = ft_strdup(av[i]);
        if (av[i] && ft_strlen(av[i]) == 1 && (av[i][0] == '|' || av[i][0] == ';'))
            break ;
        i++;
        j++;
    }
    if (av[i] && ft_strlen(av[i]) == 1 && av[i][0] == '|')
        microshell.flag = PIPE;
    else if (av[i] && ft_strlen(av[i])==1 && av[i][0] == ';')
        microshell.flag = MULTI;
    else
        microshell.flag = NO_FLAG;

}

void fork_exec(void)
{
    pid_t pid;

    if (microshell.args[0] && !strcmp(microshell.args[0], "cd"))
        ft_cd();
    else
    {
        if (microshell.flag == PIPE)
            pipe(fd);  //创建一个管道
        pid = fork(); //创建子进程
        if(pid < 0)
            _exit_(FAT_E, NULL, STDERR_FILENO, ERROR);
        else if (pid  == 0)  //子进程
        {
            if (microshell.flag == PIPE)
            {
                close(fd[0]); //关闭读
                dup2(fd[1], STDOUT_FILENO); //允许输出放入1
            }
            if (execve(microshell.args[0], microshell.args, microshell.env) == -1)//!!!!!!!运行第0位 比如 ls
                _exit_(EXE_E, microshell.args[0], STDERR_FILENO, ERROR);
        }
        else
        {
            if (microshell.flag == PIPE) 
            {
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
            }
            waitpid(pid, NULL, 0);
        }
    }
    if (fd[0] != -1)
        close(fd[0]);

}

void _exec(char **av, char **env)//!!!!!!!!!!
{
    int i = 1;
    while (av[i])
    {
         _parser(av, i);  //解析
         microshell.env = env; //init env 为了fork
        if (!(microshell.args[0] && microshell.args[0][0] == ';' && ft_strlen(microshell.args[0]) == 1))
            fork_exec();
        if (microshell.flag == MULTI || microshell.flag == NO_FLAG)
            i += microshell.len;
        else 
            i += microshell.len + 1;
        free_args();
    }
}

int main(int ac, char **av, char **env)
{
    if (ac <= 1)
        return (0);
    fd[1] = -1;
    fd[0] = -1;
    _exec(av, env);
    return (0);
}