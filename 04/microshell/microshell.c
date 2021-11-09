#include "microshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

# define  ARG_E "error: cd: bad arguments\n"
# define  CD_E "error: cd: cannot change directory to path_to_change\n"
# define FAT_E "error: fatal\n"
# define EXE_E "error: cannot execute "

# define PIPE 0;
# define MULTI 1;

# define ERROR 1;
# define SUCCESS 0;

typedef struct t_microshell
{
    int len;
    char **agrs;
    char **env;
    int flag;

}t_microshell;


void	print_msg(char *msg1, char *msg2, int fd)
{
	if (msg1)
		ft_putstr_fd(msg1, fd);
	if (msg2)
		ft_putstr_fd(msg2, fd);
    ft_putstr_fd("\n", fd);    
}


int arg_len(char **av) 
{
    int i = 1;
    while(a[i])
        i++;
    return (i);
}

void free_agrs()
{
    int i = 0;
    while (microshell.args[i])
    {
        free(microshell.args[i])
        microshell.agrs[i] = NULL;
        i++;
    }
    free(microshell.args);
    microshell.args = NULL;
}
void  _exit(char *msg1, char *msg2, int fd, int ret)
{
    print_msg(msg1, msg2, fd);
    free_agrs();
    exit(ret);
}

void ft_cd (void)
{
    if (microshell)
}

void  parser(char **av)
{
    t_microshell microshell;
    
    b_zero(&microshell, sizeof(t_microshell));
    microshell.len = arg_len(av);
    microshell.args = calloc(microshell.len + 1, sizeof(char *));
    if (!microshell.args)
        _exit(FAT_E, NULL, STDERRE_FILENO, FAILURE);

}
int main(int ac, char **av, char **env)
{
    
}