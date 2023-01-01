/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jiychoi <jiychoi@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/29 15:55:05 by jiychoi           #+#    #+#             */
/*   Updated: 2022/12/31 17:31:38 by jiychoi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include "irc.hpp"

class User {
	private:
		int					_clientSocket; // 클라이언트 소켓 fd
		struct sockaddr_in*	_clientAddress; // 클라이언트 소켓 정보
		socklen_t*			_clientAddressSize; // 클라이언트 소켓 정보 크기
		std::string			_nickname;
		std::string			_username;
		std::string			_hostname;
		// bool				_isAdmin;

	public:
		User();
		User(const User& instance);
		~User();
		User&				operator=(const User& instance);

		int					getSocketDesc() const;
		struct sockaddr_in*	getAddressPtr() const;
		socklen_t*			getAddressSizePtr() const;
		std::string			getNickname() const;
		std::string			getUsername() const;
		std::string			getHostname() const;
		// bool				getIsAdmin() const;

		void				setNickname(std::string nickname);
		void				setUsername(std::string username);
		void				setHostname(std::string hostname);
		void				setSocketDesc(int clientSocket);
};

std::ostream&	operator<<(std::ostream& out, const User& instance);

#endif