/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:00:00 by maria-ol          #+#    #+#             */
/*   Updated: 2025/10/16 22:47:53 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minitalk_bonus.h"

/**
 * @brief Global variable holding the current client PID.
 *
 * Used by the server to identify which process sent the signal.
 */
pid_t	g_client_pid = 0;

/**
 * @brief Handles incoming signals (SIGUSR1 and SIGUSR2) from the client.
 *
 * Each signal represents one bit of a character:
 * - SIGUSR1 → bit 0
 * - SIGUSR2 → bit 1
 *
 * The handler uses two static variables:
 * - `current_char` accumulates the bits to form a full character.
 * - `bit_count` tracks how many bits have been received.
 * 
 * Once 8 bits are received, a full character is reconstructed.
 * If the character is '\0', a SIGUSR1 is sent back to the client to confirm 
 * completion.
 * Otherwise, the character is printed and the next bit is awaited.
 *
 * @param sig Signal number (SIGUSR1 or SIGUSR2).
 * @param info Signal info structure containing sender PID.
 * @param context Unused pointer to signal context (required by SA_SIGINFO).
 */
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

/**
 * @brief Configures signal handling for SIGUSR1 and SIGUSR2.
 *
 * This function sets up a `sigaction` structure that links both signals
 * to the `signal_handler()` function. The flag `SA_SIGINFO` is used so that
 * the handler can access additional information (like the client's PID)
 * via the `siginfo_t` parameter.
 *
 */
void	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_sigaction = signal_handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
}

/**
 * @brief Entry point of the server program.
 *
 * The server prints its PID so that clients know where to send messages.
 * Then it enters an infinite loop, waiting for incoming signals.
 *
 * The use of `pause()` inside the `while (1)` loop puts the process in a 
 * sleeping state until a signal is received waking only when a signal arrives 
 * to be processed by the handler.
 *
 * @return Always returns 0.
 */
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
