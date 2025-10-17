/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:00:00 by maria-ol          #+#    #+#             */
/*   Updated: 2025/10/16 20:35:59 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minitalk_bonus.h"

pid_t	g_client_pid = 0;

void	signal_handler(int sig, siginfo_t *info, void *context)
{
	static char	current_char = 0;
	static int	bit_count = 0;

	(void)context;
	g_client_pid = info->si_pid;
	if (sig == SIGUSR2)
		current_char |= (1 << bit_count);
	bit_count++;
	if (bit_count == 8)
	{
		if (current_char == '\0')
			kill(g_client_pid, SIGUSR1);
		else
		{
			write(1, &current_char, 1);
			kill(g_client_pid, SIGUSR2);
		}
		current_char = 0;
		bit_count = 0;
	}
	else
		kill(g_client_pid, SIGUSR2);
}

void	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_sigaction = signal_handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
}

int	main(void)
{
	pid_t	server_pid;

	setup_signals();
	server_pid = getpid();
	ft_printf("Server PID: %d\n", server_pid);
	while (1)
		pause();
	return (0);
}
