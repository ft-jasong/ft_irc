/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jiychoi <jiychoi@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/07 00:04:29 by jiychoi           #+#    #+#             */
/*   Updated: 2023/01/07 01:43:15 by jiychoi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"


bool	Server::isServerUser(int clientFd) {
	std::vector<User>::iterator	iter;

	for (iter = _s_userList.begin(); iter < _s_userList.end(); iter++) {
		if (clientFd == iter->getSocketFd())
			return (true);
	}
	return (false);
}

User&	Server::findUser(int clientFd) {
	std::vector<User>::iterator	iter;

	for (iter = _s_userList.begin(); iter < _s_userList.end(); iter++) {
		if (clientFd == iter->getSocketFd())
			return (*iter);
	}
	throw std::runtime_error(Error(ERR_CANNOTFINDUSERFD));
}

void	Server::setUserDisconnectByFd(int clientFd) {
	std::vector<User>::iterator userIter;

	for (userIter = _s_userList.begin(); userIter < _s_userList.end(); userIter++) {
		if (userIter->getSocketFd() != clientFd) continue;
		userIter->setIsDisconnected(true);
		return;
	}
}
