/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandChannel.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jasong <jasong@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 17:53:57 by san               #+#    #+#             */
/*   Updated: 2023/01/08 14:10:16 by jasong           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/Server.hpp"
# include "../includes/Reply.hpp"

void	Server::commandJOIN(User* user, stringVector& parameters) {
	if (user->getIsVerified() != ALL_VERIFIED) throw std::runtime_error(Error(ERR_NOTREGISTERED));
	if (parameters.size() < 2) throw std::runtime_error(Error(ERR_NEEDMOREPARAMS, CMD_JOIN));

	std::string	channelName = parameters[1];
	bool		isChannelAvailable = isChannel(channelName);

	if (isChannelAvailable) {
		Channel*	ch = findChannel(channelName);
		if (ch->isUserInChannel(user->getNickname()))
			return ;
		sendMessage(user, " JOIN " + channelName);
		ch->joinNewUser(user);
		user->addJoinedChannel(channelName);
		sendMessage(user, Reply(RPL_NAMREPLY, user->getNickname(), ch->getUserList()));
		sendMessage(user, Reply(RPL_ENDOFNAMES, user->getNickname() + " " + ch->getChannelName()));
		sendMessageBroadcast(1, ch, user, "JOIN :" + ch->getChannelName());
	}
	else {
		Channel*	ch = new Channel(user, channelName);
		sendMessage(user, " JOIN " + channelName);
		_channelList.push_back(ch);
		user->addJoinedChannel(channelName);
		sendMessage(user, Reply(RPL_NAMREPLY, user->getNickname(), ch->getUserList()));
		sendMessage(user, Reply(RPL_ENDOFNAMES, user->getNickname() + " " + ch->getChannelName()));
	}
}

void	Server::commandTOPIC(User* user, stringVector& parameters) {
	if (user->getIsVerified() != ALL_VERIFIED) throw std::runtime_error(Error(ERR_NOTREGISTERED));
	if (parameters.size() < 3) throw std::runtime_error(Error(ERR_NEEDMOREPARAMS, CMD_TOPIC));
	if (!isChannel(parameters[1])) throw std::runtime_error(Error(ERR_NOSUCHCHANNEL, parameters[1]));

	Channel*	ch = findChannel(parameters[1]);
	std::string	channelTopic = ft_getMsgString(parameters, 2);

	if (!ch->isOperator(user->getNickname()))
		throw std::runtime_error(Error(ERR_CHANOPRIVSNEEDED, parameters[1]));
	ch->setTopic(channelTopic);
	sendMessage(user, " TOPIC " + parameters[1] + " " + channelTopic);
	sendMessageBroadcast(1, ch, user, "TOPIC " + parameters[1] + " " + channelTopic);
}

void	Server::commandMSG(User* user, stringVector& parameters) {
	if (user->getIsVerified() != ALL_VERIFIED) throw std::runtime_error(Error(ERR_NOTREGISTERED));

	std::string message = ft_getMsgString(parameters, 2);

	if (isChannel(parameters[1])) {
		Channel*	ch = findChannel(parameters[1]);
		std::string	botMessage = ch->parseBotCommand(message);

		if (ch->isUserInChannel(user->getNickname()) || ch->isOperator(user->getNickname()))
			sendMessageBroadcast(1, ch, user, "PRIVMSG " + ch->getChannelName() + " :" + ft_getMsgString(parameters, 2));
		if (botMessage.length() > 0)
			sendMessageBroadcastBot(ch, "PRIVMSG "  + ch->getChannelName() + " " + botMessage);
		else if (ch->isBadWordIncluded(message))
			kickUserFromChannel(ch, user, "금지어 사용으로 인한 Kick");
		return;
	}
	if (isServerUser(parameters[1])) {
		User*	receiver = findUser(parameters[1]);
		sendMessage(user, receiver, "PRIVMSG " + parameters[1] + " :" + ft_getMsgString(parameters, 2));
		return;
	}
	throw std::runtime_error(Error(ERR_NOSUCHNICK));
}

void	Server::commandNOTICE(User* user, stringVector& parameters) {
	if (user->getIsVerified() != ALL_VERIFIED) throw std::runtime_error(Error(ERR_NOTREGISTERED));

	if (isChannel(parameters[1])) {
		Channel*	ch = findChannel(parameters[1]);
		if (ch->isUserInChannel(user->getNickname()) || ch->isOperator(user->getNickname()))
			sendMessageBroadcast(1, ch, user, "NOTICE " + ch->getChannelName() + " :" + ft_getMsgString(parameters, 2));
		return;
	}
	if (isServerUser(parameters[1])) {
		User*	receiver = findUser(parameters[1]);
		sendMessage(user, receiver, "NOTICE " + parameters[1] + " :" + ft_getMsgString(parameters, 2));
		return;
	}
	throw std::runtime_error(Error(ERR_NOSUCHNICK));
}

void	Server::commandMODE(User* user, stringVector& parameters) {
	int paramLen = parameters.size();
	if (paramLen < 2) throw std::runtime_error(Error(ERR_NEEDMOREPARAMS));
	if (!isServerUser(parameters[1]) && !isChannel(parameters[1]))
		throw std::runtime_error(Error(ERR_NOSUCHNICK));

	if (paramLen == 2) {
		if (isChannel(parameters[1])) {
			Channel*	ch = findChannel(parameters[1]);
			sendMessage(user, Reply(RPL_CHANNELMODEIS, user->getNickname(), ch->getChannelName() + " :" + ch->getChannelMode()));
			return;
		}
		if (!parameters[1].compare(user->getNickname()))
			sendMessage(user, Reply(RPL_UMODEIS, user->getNickname() + " :" + user->getUserMode()));
		else
			throw std::runtime_error(Error(ERR_USERSDONTMATCH, user->getNickname()));
		return;
	}
	if (paramLen == 3) {
		if (isChannel(parameters[1])) {
			Channel*	ch = findChannel(parameters[1]);
			ch->setChannelMode(parameters[2]);
		} else
			user->setUserMode(parameters[2]);
		sendMessage(user, "MODE " + user->getNickname() + " :" + parameters[2]);
	}
}

void	Server::commandPART(User* user, stringVector& parameters) {
	if (user->getIsVerified() != ALL_VERIFIED) throw std::runtime_error(Error(ERR_NOTREGISTERED));

	if (isChannel(parameters[1])) {	// 채널이면
		Channel*	ch = findChannel(parameters[1]);

		if (ch->isUserInChannel(user->getNickname())) {
			ch->deleteNormalUser(user->getNickname());
			// return;
		}
		else if (ch->isOperator(user->getNickname())) {
			if (ch->deleteOperatorUser(user->getNickname())) {	// 오퍼레이터가 새로 바뀌었다.
				sendMessageBroadcast(0, ch, user, "MODE " + ch->getChannelName() + " +o :" + ch->getOperatorVector()[0]->getNickname());
			} else {
				ch->setIsDeleted(true);
			}
			// return;
		} else {
			throw std::runtime_error(Error(ERR_NOTONCHANNEL));
		}
		sendMessage(user, "PART :" + ch->getChannelName());
		sendMessageBroadcast(0, ch, user, "PART :" + ch->getChannelName());
		user->deleteJoinedChannel(ch->getChannelName());
	}
}

void	Server::commandNAMES(User* user, stringVector& parameters) {
	if (user->getIsVerified() != ALL_VERIFIED) throw std::runtime_error(Error(ERR_NOTREGISTERED));

	Channel*	ch = findChannel(parameters[1]);
	sendMessage(user, Reply(RPL_NAMREPLY, user->getNickname(), ch->getUserList()));
	sendMessage(user, Reply(RPL_ENDOFNAMES, user->getNickname() + " " + ch->getChannelName()));
}

void	Server::commandKICK(User* user, stringVector& parameters) {
	Channel*	ch = findChannel(parameters[1]);

	if (!ch->isOperator(user->getNickname()))
		throw std::runtime_error(Error(ERR_CHANOPRIVSNEEDED));
	if (!ch->isUserInChannel(parameters[2]))
		throw std::runtime_error(Error(ERR_NOSUCHNICK));

	User*	kickedUser = findUser(parameters[2]);
	kickUserFromChannel(ch, kickedUser, ft_getMsgString(parameters, 3));
}
