/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:41:08 by mona              #+#    #+#             */
/*   Updated: 2025/10/15 19:15:53 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minitalk.h"

void	signal_handler(int sig)
{
	static char	current_char = 0;
	static int	bit_count = 0;

	current_char = current_char << 1;
	if (sig == SIGUSR2)
	{
		current_char = current_char | 1;
	}
	bit_count++;
	if (bit_count == 8)
	{
		write(1, &current_char, 1);
		current_char = 0;
		bit_count = 0;
	}
}

void	setup_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_handler = signal_handler;
	sa.sa_flags = 0;
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
