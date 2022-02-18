#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void	ft_putstr_fd2(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	write(2, str, i);
}

int	ft_execute(char **av, int i, int tmp_fd, char **env)
{
	av[i] = NULL;
	close(tmp_fd);
	execve(av[0], av, env);
	ft_putstr_fd2("error: cannot execute ");
	ft_putstr_fd2(av[0]);
	write(2, "\n", 1);
	return (1);
}

int	main(int ac, char **av, char **env)
{
	int	i;
	int	pid;
	int	fd[2];
	int	tmp_fd;
	(void)ac;

	pid = 0;
	i = 0;
	tmp_fd = dup(STDIN_FILENO);
	while (av[i] && av[i + 1])
	{
		av = &av[i + 1];
		i = 0;
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
			i++;
		if (strcmp(av[0], "cd") == 0)
		{
			if (i != 2)
				ft_putstr_fd2("error: cd: bad arguments\n");
			else if (chdir(av[1]) != 0)
			{
				ft_putstr_fd2("error: cd: cannot change directory to ");
				ft_putstr_fd2(av[1]);
				write(2, "\n", 1);
			}
		}
		else if (av != &av[i] && (av[i] == NULL || strcmp(av[i], ";") == 0))
		{
			pid = fork();
			if (pid == 0)
			{
				dup2(tmp_fd, STDIN_FILENO);
				if (ft_execute(av, i, tmp_fd, env))
					return (1);
			}
			else
			{
				close(tmp_fd);
				waitpid(-1, NULL, WUNTRACED);
				tmp_fd = dup(STDIN_FILENO);
			}
		}
		else if (av != &av[i] && strcmp(av[i], "|") == 0)
		{
			pipe(fd);
			pid  = fork();
			if (pid == 0)
			{
				dup2(tmp_fd, STDIN_FILENO);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				if (ft_execute(av, i, tmp_fd, env))
					return (1);
			}
			else
			{
				close(fd[1]);
				close(tmp_fd);
				waitpid(-1, NULL, WUNTRACED);
				tmp_fd = dup(fd[0]);
				close(fd[0]);
			}
		}
	}
	close(tmp_fd);
	return (0);
}
