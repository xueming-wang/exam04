#include "microshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
// #include <signal.h>

#define ARG_E "error: cd: bad arguments\n"
#define CD_E "error: cd: cannot change directory to "
#define FAT_E "error: fatal\n"
#define EXE_E "error: cannot execute "

#define PIPE 1
#define MULTI 2
#define NO_FLAG 0

#define ERROR 1
#define SUCCESS 0


typedef struct s_microshell
{
    int len;
    char **args;
    char **env;
    int flag;

}t_microshell;

t_microshell microshell;
int fd[2];

void  ft_bzero(void *s, int n)
{
    while (n)
        ((unsigned char *)s)[--n] = 0;
}
int ft_strlen(char *s)
{
    // char *tmp;
    // tmp = s;
    // while (*tmp)
    //     tmp++;
    // return (tmp - s);
    int i= 0;
    if (!s)
        return (0);
    while (s[i])
        i++;
    return (i);
}
void ft_putstr_fd(char *s, int fd)
{
    write(fd, s, ft_strlen(s));
}
void	print_msg(char *msg1, char *msg2, int fd)
{
	if (msg1)
		ft_putstr_fd(msg1, fd);
	if (msg2)
    {
		ft_putstr_fd(msg2, fd);
        ft_putstr_fd("\n", fd);  
    }  
}
int arg_len(char **s) //一段cmd的 args长度 echo 1 2 3 | ls 返回4 
{
    int i = 0;
    while(s[i] != NULL)
     {
        if ((s[i][0] == '|' || s[i][0] == ';') && ft_strlen(s[i]) == 1) //如果碰到
        {
            if (i == 0)  //???????????
                i = 1;
            break;
        }
        i++;
     }
    return (i);
}
void free_agrs(void)
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
void  _exit_(char *msg1, char *msg2, int fd, int ret)
{
    print_msg(msg1, msg2, fd);
    free_agrs();
    exit(ret);
}



void *ft_calloc(size_t count, size_t size)  //malloc 一段字符 初始化0
{
    void *tab;
    tab = (void *)malloc(count * size);  // 
    if (!tab)
        return NULL;
    ft_bzero(tab, count * size);
    return tab;
}

char* ft_strdup(char *s)
{
    char *tab;
    int i = 0;
    if (!s)
        return NULL;
    i = ft_strlen(s);
    
    tab = ft_calloc(i+1, sizeof(char));
    if (!tab)
        return NULL;
    i = 0;
    while (s[i])
    {
        tab[i] = s[i];
        i++;
    }
    //tab[i] = '\0';
    return tab;
}

void ft_cd (void)
{
    if (microshell.len != 2) {//只能有两位 比如 cd  /bin/ls
        print_msg(ARG_E, NULL, STDERR_FILENO);
        return;
    }
    if (chdir(microshell.args[1]) == -1) { //没有打开
        print_msg(CD_E, microshell.args[1], STDERR_FILENO);
        return;
    }
}

void  _parser(char **av, int i)  //i 表示 第几个 arg 。 这个函数保存microshell.args. （一个commande直到结束或者看见|;) 确认 falg; 
{
    ft_bzero(&microshell, sizeof(t_microshell));
    microshell.len = arg_len(av + i);
    microshell.args = ft_calloc(microshell.len + 1, sizeof(char *)); //malloc一个位置并且都赋予0
    if (!microshell.args)
        _exit_(FAT_E, NULL, STDERR_FILENO, ERROR);
    // int k = i;
    int j = 0;
    while (av[i] && j < microshell.len) //把一个arg 比如 “echo 1 3 4" 保存进args 直到遇到 ;或 |
    {
        microshell.args[j] = ft_strdup(av[i]);
        if(av[i] && ft_strlen(av[i]) == 1 && (av[i][0] == '|' || av[i][0] == ';'))//遇到 | ；停止
            break;
        j++;
        i++;
    }
    
    if (av[i] && ft_strlen(av[i])== 1 && av[i][0] == '|')
        microshell.flag = PIPE;
    else if (av[i] && ft_strlen(av[i]) == 1 && av[i][0] == ';' )
        microshell.flag = MULTI;
    else
        microshell.flag = NO_FLAG;
}

void fork_exec(void)  //运行一个cmd
{  
    pid_t pid; //进程id
        //如果第0位是 cd strcmp返回是负数运行cd 函数 返回是负数
    if (microshell.args[0] && !strcmp(microshell.args[0], "cd")) 
        ft_cd();
    else
    {   /*int pipe(int fd[2]);功能：创建一个简单的管道，若成功则为数组fd分配两个文件描述符，其中fd[0] 用于read，fd[1]用于write。
        返回：成功返回0，失败返回-1； pfd[0] 进入的东西会从 pfd[1] 显示出来*/
        if (microshell.flag == PIPE)
            pipe(fd); //创建一个管道
        pid = fork(); //创建了一个子进程

        if (pid < 0) //创建失败
            _exit_(FAT_E, NULL, STDERR_FILENO, ERROR);
        else if (pid == 0) // 子进程负责write 再执行下一个命令
        {
            if (microshell.flag == PIPE) 
            {
                close(fd[0]);//关闭read管道
                dup2(fd[1], STDOUT_FILENO); //把结果的东西放进fd[1] 
            } //【0】是 文件名
            if(execve(microshell.args[0], microshell.args, microshell.env) == -1)//执行文件
                _exit_(EXE_E, microshell.args[0], STDERR_FILENO, ERROR);
        }
        else //(pid > 0) 是父进程负责读  
        {
            if (microshell.flag == PIPE) {
                close(fd[1]);//关闭write进程
                dup2(fd[0], STDIN_FILENO); //标准输入(read)读到的东西放进fd【0】
            }
            waitpid(pid, NULL, 0);  //等待返回 父进程暂停
        }
        if(fd[0] != -1) //说明读取管道有内容
        close(fd[0]);
    }

}
void _exec(char **av, char **env)
{
    int i = 1;
    while (av[i]) //从第一位开始
    {
        _parser(av, i); // 保存一段 cmd
        microshell.env = env;

        if (!(microshell.args[0] && ft_strlen(microshell.args[0]) == 1 && microshell.args[0][0] == ';'))  //不是 ; 就能运行
            fork_exec();
        
        if (microshell.flag == NO_FLAG || microshell.flag == MULTI)
            i += microshell.len;
        else
            i += microshell.len + 1;
        free_agrs();
    }
}

int main(int ac, char **av, char **env)
{
    if (ac <= 1)
        return (0);
    fd[0] = -1;
    fd[1] = -1;
    _exec(av, env);
    return(0);
}