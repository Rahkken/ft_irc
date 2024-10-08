#include "Server.hpp"

// GETTERS //

Client	*Server::get_client_by_fd( const int& fd ) {

	for (size_t i = 0; i < this->_client_register.size(); i++) {

		if (this->_client_register[i].get_client_fd() == fd)
			return &this->_client_register[i];
	}
	return NULL;
}

Client	*Server::get_client( const std::string& nickname ) {

	for (size_t i = 0; i < this->_client_register.size(); i++) {

		if (this->_client_register[i].get_client_nickname() == nickname)
			return &this->_client_register[i];
	}
	return NULL;
}

Channel	*Server::get_channel( const std::string& name ) {

	for (size_t i = 0; i < this->_channel_register.size(); i++) {

		if (this->_channel_register[i].get_channel_name() == name)
			return &this->_channel_register[i];
	}
	return NULL;
}

// FUNCTIONS //

bool	Server::_signal = false;

void	Server::signal_handler( int signum ) {

	(void)signum;
	std::cout << std::endl << yellow << "Signal received !" << white << std::endl;
	_signal = true;
}

void	Server::server_init( const int& port, const std::string& password ) {

	set_port(port);
	set_password(password);
	set_oper_password();
	socket_creation();
	std::cout << green << "Server launched and waiting for requests..." << white << std::endl;

	while (get_signal() == false) {

		if((poll(&_pollfd_register[0], _pollfd_register.size(), -1) < 0) && get_signal() == false)
			throw(std::runtime_error("poll() faild"));

		for (size_t i = 0; i < _pollfd_register.size(); i++) {

			if (_pollfd_register[i].revents & POLLIN) {

				if (_pollfd_register[i].fd == get_socket_fd())
					new_client_request();
				else
					data_receiver(_pollfd_register[i].fd);
			}
		}
	}
	close_socket_fd();
}

void	Server::socket_creation() {

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(get_port());

	set_socket_fd(socket(AF_INET, SOCK_STREAM, 0));
	if (get_socket_fd() < 0) {
		throw(std::runtime_error("failed to create socket"));
	}

	int opt = 1;
	if (setsockopt(get_socket_fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw(std::runtime_error("failed to socket opt"));
	if (fcntl(get_socket_fd(), F_SETFL, O_NONBLOCK) < 0)
		throw(std::runtime_error("failed to fcntl"));
	if (bind(get_socket_fd(), (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw(std::runtime_error("failed to bind"));
	if (listen(get_socket_fd(), SOMAXCONN) < 0)
		throw(std::runtime_error("failed to listen"));

	struct pollfd pollfd;
	pollfd.fd = get_socket_fd();
	pollfd.events = POLLIN;
	pollfd.revents = 0;
	_pollfd_register.push_back(pollfd);
}

void	Server::new_client_request() {

	Client client;
	struct sockaddr_in addr;
	struct pollfd pollfd;
	socklen_t addr_len = sizeof(addr);

	int connect_fd = accept(get_socket_fd(), (sockaddr *)&(addr), &addr_len);
	if (connect_fd == -1)
		{std::cout << "accept() failed" << std::endl; return;}

	if (fcntl(connect_fd, F_SETFL, O_NONBLOCK) == -1)
		{std::cout << "fcntl() failed" << std::endl; return;}

	pollfd.fd = connect_fd;
	pollfd.events = POLLIN;
	pollfd.revents = 0;

	client.set_client_fd(connect_fd);
	client.set_client_ip_addr(inet_ntoa((addr.sin_addr)));
	client.set_connected_status(false);
	client.set_user_status(false);
	client.set_nick_status(false);
	client.set_oper_status(false);
	_client_register.push_back(client);
	_pollfd_register.push_back(pollfd);

	std::cout << yellow << "Client [" << client.get_client_fd() << "] is trying to connect. Waiting for password..." << white << std::endl;
	Client::send_message(connect_fd, "You have to enter the password to begin your adventure... PASS <password>;\n");
}

void	Server::data_receiver( const int& fd ) {

	Client *client = get_client_by_fd(fd);

	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);

	if (bytes <= 0) {

		if (bytes)
			std::cout << red << "Client [" << client->get_client_fd() << "] disconnected." << white << std::endl;
		else
			std::cerr << "recv() error on client [" << client->get_client_fd() << "]." << std::endl;
		client_clear(client->get_client_fd());
		close(client->get_client_fd());
		return;
	}
 
	std::string command_buffer = client->get_command_buffer() + std::string(buff, bytes);
	client->set_command_buffer(command_buffer);

	size_t pos;
	while ((pos = command_buffer.find('\n')) != std::string::npos) {

		std::string command_raw = command_buffer.substr(0, pos);
		if (command_raw.empty()) {

			command_buffer.clear();
			client->set_command_buffer(command_buffer);
			break;
		}

		command_buffer.erase(0, pos + 1);
		client->set_command_buffer(command_buffer);

		if (!command_raw.empty() && command_raw[command_raw.size() - 1] == '\r')
			command_raw.erase(command_raw.size() - 1);

		std::vector<std::string> command_parsed;
		size_t prev_pos = 0;
		size_t curr_pos;

		while ((curr_pos = command_raw.find(' ', prev_pos)) != std::string::npos) {

			if (curr_pos > prev_pos)
				command_parsed.push_back(command_raw.substr(prev_pos, curr_pos - prev_pos));
			prev_pos = curr_pos + 1;
		}

		if (prev_pos < command_raw.size())
			command_parsed.push_back(command_raw.substr(prev_pos));

		if (!client->get_connected_status()) {
	
			if (!strcmp(command_parsed[0].c_str(), "PASS"))
				pass_command(command_parsed, client);
			else
				Client::send_message(fd, "Try {PASS <password>}.\n");
		}
		else {

			if (!client->get_user_status() || !client->get_nick_status()) {

				if (!strcmp(command_parsed[0].c_str(), "USER"))
					user_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "NICK"))
					nick_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "QUIT"))
					quit_command(client);
				else
					Client::send_message(fd, "Create your account {NICK <nickname> | USER <username> <hostname> <servername> <realname>}.\n");
			}
			else {

				if (!strcmp(command_parsed[0].c_str(), "OPER"))
					oper_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "PRIVMSG"))
					privmsg_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "JOIN"))
					join_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "PART"))
					part_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "MODE"))
					mode_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "KICK"))
					kick_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "INVITE"))
					invite_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "TOPIC"))
					topic_command(command_parsed, client);
				else if (!strcmp(command_parsed[0].c_str(), "QUIT"))
					quit_command(client);
			}
		}
	}
}

// CHECK FUNCTIONS //

bool	Server::check_existing_channel( const std::string& channel_name ) const{

	for (size_t i = 0; i < _channel_register.size(); i++) {

		if (_channel_register[i].get_channel_name() == channel_name)
			return true;
	}
	return false;
}

bool	Server::check_existing_client( const std::string& nickname ) const{

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_nickname() == nickname)
			return true;
	}
	return false;
}

bool	Server::check_valid_nickname( const std::string &nickname ) const{

	if (nickname.empty() || nickname.length() > 9)
		return false;

	for (size_t i = 0; i < nickname.length(); i++) {

		if (!isalnum(nickname[i]))
			return false;
	}
	return true;
}

// CLEAR FUNCTIONS //

void	Server::client_clear( const int& fd ) {

	for (size_t i = 0; i < _pollfd_register.size(); i++) {

		if (_pollfd_register[i].fd == fd) {
			
			_pollfd_register.erase(_pollfd_register.begin() + i);
			break;
		}
	}

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() == fd) {
			
			close(_client_register[i].get_client_fd());
			_client_register.erase(_client_register.begin() + i);
			break;
		}
	}

	for (size_t i = 0; i < _channel_register.size(); i++) {

		_channel_register[i].client_clear(fd);
	}
}

void	Server::close_socket_fd() {

	for(size_t i = 0; i < _client_register.size(); i++) {

		std::cout << red << "Client [" << _client_register[i].get_client_username() << "] disconnected." << white << std::endl;
		Client::send_message(_client_register[i].get_client_fd(), "! IRC server closed !\n");
		close(_client_register[i].get_client_fd());
	}

	if (get_socket_fd() != -1) {

		std::cout << red << "! IRC server closed !" << white << std::endl;
		close(_socket_fd);
	}
}