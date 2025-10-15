/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:41:54 by mona              #+#    #+#             */
/*   Updated: 2025/10/15 16:39:50 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_H
# define MINITALK_H

# include "libft/libft.h"
# include <signal.h>

void	signal_handler(int sig);
void	setup_signals(void);
void	send_char(pid_t server_pid, unsigned char chr);
void	send_string(pid_t server_pid, char *str);

#endif