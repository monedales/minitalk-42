/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:00:00 by maria-ol          #+#    #+#             */
/*   Updated: 2025/10/15 19:00:09 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk_bonus.h"

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
		while (!g_received)
			pause();
		g_received = 0;
		usleep(200);
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
