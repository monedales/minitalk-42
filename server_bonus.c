/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:00:00 by maria-ol          #+#    #+#             */
/*   Updated: 2025/10/15 18:57:06 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk_bonus.h"

pid_t	g_client_pid = 0;

void	signal_handler(int sig, siginfo_t *info, void *context)
{
	static char	current_char = 0;
	static int	bit_count = 0;

	(void)context;
	g_client_pid = info->si_pid;
	current_char = current_char << 1;
	if (sig == SIGUSR2)
		current_char = current_char | 1;
	bit_count++;
	kill(g_client_pid, SIGUSR2);
	if (bit_count == 8)
	{
		if (current_char == '\0')
		{
			kill(g_client_pid, SIGUSR1);
		}
		else
			write(1, &current_char, 1);
		current_char = 0;
		bit_count = 0;
	}
}

void	setup_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = signal_handler;
	sa.sa_flags = SA_SIGINFO;
	if (sigaction(SIGUSR1, &sa, NULL) == -1)
	{
		ft_error();
		exit(1);
	}
	if (sigaction(SIGUSR2, &sa, NULL) == -1)
	{
		ft_error();
		exit(1);
	}
}

int	main(void)
{
	ft_printf("Server PID: %d\n", getpid());
	setup_signals();
	while (1)
	{
		pause();
	}
	return (0);
}
