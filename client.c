/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:00:00 by maria-ol          #+#    #+#             */
/*   Updated: 2025/10/16 20:50:58 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minitalk.h"

volatile sig_atomic_t	g_received = 0;

static void	handle_received(int sig)
{
	if (sig == SIGUSR2)
		g_received = 1;
}

static void	handle_completion(int sig)
{
	if (sig == SIGUSR1)
	{
		ft_printf("\nMessage received by server!\n");
		exit(0);
	}
}

void	setup_signal_handlers(void)
{
	struct sigaction	sa_received;
	struct sigaction	sa_completion;

	sa_received.sa_handler = handle_received;
	sigemptyset(&sa_received.sa_mask);
	sa_received.sa_flags = SA_RESTART;
	sigaction(SIGUSR2, &sa_received, NULL);
	sa_completion.sa_handler = handle_completion;
	sigemptyset(&sa_completion.sa_mask);
	sa_completion.sa_flags = SA_RESTART;
	sigaction(SIGUSR1, &sa_completion, NULL);
}

void	send_char(pid_t server_pid, unsigned char chr)
{
	int				i;
	unsigned int	bit;
	unsigned int	signal;

	i = 0;
	while (i < 8)
	{
		bit = (chr >> i) & 1;
		if (bit == 1)
			signal = SIGUSR2;
		else
			signal = SIGUSR1;
		g_received = 0;
		if (kill(server_pid, signal) == -1)
		{
			ft_error();
			exit(1);
		}
		while (g_received == 0)
			pause();
		i++;
	}
}

int	main(int argc, char **argv)
{
	pid_t	server_pid;
	int		i;

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
	ft_printf("Client PID: %d\n", getpid());
	setup_signal_handlers();
	i = 0;
	while (argv[2][i])
	{
		send_char(server_pid, (unsigned char)argv[2][i]);
		i++;
	}
	send_char(server_pid, '\0');
	while (1)
		pause();
	return (0);
}
