/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk_bonus.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maria-ol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:41:54 by mona              #+#    #+#             */
/*   Updated: 2025/10/16 20:35:58 by maria-ol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_BONUS_H
# define MINITALK_BONUS_H

# include "libft/libft.h"
# include <signal.h>

void	signal_handler(int sig, siginfo_t *info, void *context);
void	setup_signals(void);
void	send_char(pid_t server_pid, unsigned char chr);
void	setup_signal_handlers(void);

#endif