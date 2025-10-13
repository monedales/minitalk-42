/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mona <mona@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:41:08 by mona              #+#    #+#             */
/*   Updated: 2025/10/13 19:34:54 by mona             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

void    signal_handler(int sig)
{
	static char current_char = 0;
	static int  bit_count = 0;

	current_char = current_char << 1;
	if (sig == SIGUSR2)
	{
		current_char = current_char | 1;
	}
	if (sig == SIGUSR1) {}

	bit_count++;
	if (bit_count == 8)
	{
		write(1, &current_char, 1);
		current_char = 0;
		bit_count = 0;
	}
	
//     Logic:
//     1. Shift current_char left by 1 bit (make room for new bit)
//     2. If sig == SIGUSR2, add 1 to current_char (OR with 1)
//     3. If sig == SIGUSR1, add 0 (do nothing)
//     4. Increment bit_count
//     5. If bit_count == 8:
//    ├─ Write current_char to stdout
//    ├─ Reset current_char to 0
//    └─ Reset bit_count to 0
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
	
	// 1. Create struct sigaction variable (sa)
	// 2. Initialize sa.sa_mask with sigemptyset()
	// 3. Set sa.sa_handler to point to our signal_handler function
	// 4. Set sa.sa_flags = 0 (no special flags needed for basic version)
	// 5. Call sigaction(SIGUSR1, &sa, NULL) to register SIGUSR1
	// 6. Call sigaction(SIGUSR2, &sa, NULL) to register SIGUSR2
	// 7. Handle errors if sigaction fails
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