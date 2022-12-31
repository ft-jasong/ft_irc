/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jiychoi <jiychoi@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/30 04:09:31 by jiychoi           #+#    #+#             */
/*   Updated: 2022/12/31 16:54:06 by jiychoi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void		Server::commandCAP(User* user, std::vector<std::string>& parameters) {
	if (parameters.size() != 2) throw Error::AuthorizeException(); // TODO: 다른 오류로 교체
	std::cout << "CAP LS received\n";
	if (*(parameters.begin() + 1) == "LS") {
		sendClientMessage(user, "CAP * LS ");
	}
}

void	Server::commandNICK(User* user, std::vector<std::string>& parameters) {
	std::vector<User>::iterator	iter;
	const std::string nickname = *(parameters.end() - 1);
	if (parameters.size() != 2) throw Error::AuthorizeException();
	if (nickname.length() <= 0 || nickname.length() > 9) throw Error::AuthorizeException();
	if (!ft_isValidNickname(nickname)) throw Error::AuthorizeException();

	user->setNickname(nickname);

	for (iter = _user_vector.begin(); iter < _user_vector.end(); iter++) {
		if ((*iter).getNickname() == nickname)
			throw Error::AuthorizeException();
	}
}

void	Server::commandUser(User* user, std::vector<std::string>& parameters) {
	const std::string username = *(parameters.begin() + 1);

	if (parameters.size() != 5) throw Error::AuthorizeException();
	if (username.length() <= 0)
		throw Error::AuthorizeException();
	user->setUsername(username);

	_user_vector.push_back(*user);
	sendClientMessage(user, ":127.0.0.1 001 chichoon :Welcome to the IRCServ Network " + user->getNickname() + "!" + username + "@127.0.0.1");
}
