/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pibosc <pibosc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 21:04:41 by pibosc            #+#    #+#             */
/*   Updated: 2023/11/23 01:54:34 by pibosc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static int	pipes(t_data *data)
{
	if (data->cmd_id == 2)
	{
		data->fd_in = open(data->argv[1], O_RDONLY);
		if (data->fd_in == -1)
			return (perror("open"), EXIT_FAILURE);
		dup2(data->fd_in, STDIN_FILENO);
	}
	else
		dup2(data->prev_pipe, STDIN_FILENO);
	if (data->cmd_id == data->argc - 2)
	{
		data->fd_out = open(data->argv[data->argc - 1],
				O_WRONLY | O_CREAT | O_TRUNC, 666);
		if (data->fd_out == -1)
			return (perror("open"), EXIT_FAILURE);
		dup2(data->fd_out, STDOUT_FILENO);
	}
	else
		dup2(data->pipe[1], STDOUT_FILENO);
	if (data->prev_pipe != -1)
		close(data->prev_pipe);
	return (EXIT_SUCCESS);
}

static int	exec(t_data *data)
{
	char	**cmd;

	cmd = ft_split(data->argv[data->cmd_id], ' ');
	execve(cmd[0], cmd, data->env);
	free_tab_2d(cmd);
	return (0);
}

static int	pipex(t_data *data)
{
	pid_t	pid;
	int		status;

	while (data->cmd_id < data->argc - 1)
	{
		if (pipe(data->pipe) == -1)
			return (perror("pipe"), EXIT_FAILURE);
		pid = fork();
		if (!pid)
		{
			if (pipes(data) == EXIT_FAILURE)
				return (EXIT_FAILURE);
			exec(data);
		}
		else
		{
			if (data->prev_pipe != -1)
				close(data->prev_pipe);
			data->prev_pipe = data->pipe[0];
			close(data->pipe[1]);
		}
		++data->cmd_id;
	}
	return (wait(&status), EXIT_SUCCESS);
}

int	main(int argc, char **argv, char **env)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (!init_args(data, argc, argv, env))
		return (free(data), EXIT_FAILURE);
	pipex(data);
	return (0);
}