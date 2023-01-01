/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jiychoi <jiychoi@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/30 04:09:31 by jiychoi           #+#    #+#             */
/*   Updated: 2023/01/01 16:18:35 by jiychoi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void	Server::commandNICK(User& user, std::vector<std::string>& parameters) {
	std::vector<User>::iterator	iter;
	const std::string nickname = *(parameters.end() - 1);
	if (parameters.size() != 2) throw Error::AuthorizeException();
	if (nickname.length() <= 0 || nickname.length() > 9) throw Error::AuthorizeException();
	if (!ft_isValidNickname(nickname)) throw Error::AuthorizeException();

	user.setNickname(nickname);

	for (iter = _user_vector.begin(); iter < _user_vector.end(); iter++) {
		if ((*iter).getNickname() == nickname)
			throw Error::AuthorizeException();
	}
}

void	Server::commandUser(User& user, std::vector<std::string>& parameters) {
	if (parameters.size() < 5)
		throw Error::AuthorizeException();
	if (parameters[1].length() <= 0 || parameters[2].length() <= 0)
		throw Error::AuthorizeException();
	user.setUsername(parameters[1]);
	user.setHostname(parameters[2]);

	_user_vector.push_back(user);

	sendClientMessage(user,
		":127.0.0.1 001 " + user.getNickname() + " :\033[1;32mWelcome to the " + SERVER_NAME + "\e[0m " + \
		user.getNickname() + "!" + user.getUsername() + "@" + user.getHostname()
	);
	sendClientMessage(user,
		":127.0.0.1 002 " + user.getNickname() + " :\033[1;32mYour host is " + SERVER_NAME + ", " + "running version 0.1\e[0m"
	);
	sendClientMessage(user,
		":127.0.0.1 003 " + user.getNickname() + " :\033[1;32mThis server was created " + ctime(&_created_time) + "\e[0m"
	);
	sendClientMessage(user,
		":127.0.0.1 004 " + user.getNickname() + " :\033[1;32m" + SERVER_NAME + " 0.1\e[0m"
	);
}