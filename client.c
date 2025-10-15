/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:41:11 by mona              #+#    #+#             */
/*   Updated: 2025/10/15 19:13:44 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minitalk.h"

void	send_char(pid_t server_pid, unsigned char chr)
{
	ssize_t			len;
	unsigned int	bit;
	unsigned int	signal;

	len = 7;
	while (len >= 0)
	{
		bit = (chr >> len) & 1;
		if (bit == 1)
			signal = SIGUSR2;
		else
			signal = SIGUSR1;
		if (kill(server_pid, signal) == -1)
		{
			ft_error();
			exit(1);
		}	
		usleep(100);
		len--;
	}
}

void	send_string(pid_t server_pid, char *str)
{
	unsigned int	i;

	i = 0;
	while (str[i])
	{
		send_char(server_pid, (unsigned char)str[i]);
		i++;
	}
	send_char(server_pid, '\0');
}

int	main(int argc, char **argv)
{
	pid_t	server_pid;

	if (argc != 3)
	{
		ft_printf("Usage: %s <server_pid> <message>\n", argv[0]);
		return (1);
	}
	server_pid = ft_atoi(argv[1]);
	if (server_pid <= 0)
	{
		ft_error();
		return (1);
	}
	send_string(server_pid, argv[2]);
	ft_printf("\nMessage sent to PID %d!\n", server_pid);
	return (0);
}
