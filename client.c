/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 17:00:00 by maria-ol          #+#    #+#             */
/*   Updated: 2025/10/16 22:48:26 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minitalk.h"

/**
 * @brief Global variable to indicate bit acknowledgment.
 *
 * Used by the client to synchronize communication — set to 1
 * whenever the server confirms that a bit was successfully received.
 */
volatile sig_atomic_t	g_received = 0;

/**
 * @brief Handles acknowledgment signals (SIGUSR2) from the server.
 *
 * When the client sends one bit, the server replies with SIGUSR2 to indicate
 * it has successfully received that bit. This function simply sets the
 * `g_received` flag to 1, allowing the client to continue sending the next bit.
 *
 * @param sig Signal number (should be SIGUSR2).
 */
static void	handle_received(int sig)
{
	if (sig == SIGUSR2)
		g_received = 1;
}

/**
 * @brief Handles completion signal (SIGUSR1) sent by the server.
 *
 * When the full message has been received and processed by the server,
 * it sends a SIGUSR1 signal to the client. This handler prints a confirmation
 * message and terminates the program gracefully.
 *
 * @param sig Signal number (should be SIGUSR1).
 */
static void	handle_completion(int sig)
{
	if (sig == SIGUSR1)
	{
		ft_printf("\nMessage received by server!\n");
		exit(0);
	}
}

/**
 * @brief Sets up all necessary signal handlers for the client program.
 *
 * This function uses the `sigaction` structure to define how the process reacts
 * to specific signals:
 * - SIGUSR2 → Confirms that a bit sent was received by the server.
 * - SIGUSR1 → Indicates that the entire message has been received.
 *
 * Using `sigaction` allows the use of flags like `SA_RESTART` which ensures 
 * that system calls (like `pause()`) automatically resume if interrupted by a
 * signal.

 * After sending each bit, the client blocks on `pause()` until the corresponding
 * acknowledgment signal is handled here.
 */
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

/**
 * @brief Sends one character to the server process, bit by bit.
 *
 * Each bit of the character is transmitted as a UNIX signal:
 * - SIGUSR1 for bit 0
 * - SIGUSR2 for bit 1
 *
 * After sending each bit, the client waits (using `pause()`) until the server
 * confirms receipt by sending SIGUSR2, ensuring reliable sequential 
 * communication.
 *
 * @param server_pid PID of the server process.
 * @param chr Character to be sent.
 */
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

/**
 * @brief Entry point of the client program.
 *
 * Usage: ./client_bonus <server_pid> <message>
 *
 * The client sends the provided string message to the given server PID.
 * After sending all characters (including the null terminator),
 * it waits indefinitely for a SIGUSR1 confirmation.
 * 
 * @note The use of an infinite `while (1) pause();` at the end ensures 
 * the client remains idle, while still responsive to any pending signals.
 *
 * @param argc Argument count (should be 3).
 * @param argv Argument vector ([1] = server PID, [2] = message).
 * @return 0 on success, 1 on failure.
 */
int	main(int argc, char **argv)
{
	pid_t	server_pid;
	int		i;

	if (argc != 3)
		return (1);
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
