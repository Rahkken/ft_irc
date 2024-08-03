#include "Channel.hpp"

// FUNCTIONS //

bool	Channel::check_existing_client( int fd ) {

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() == fd)
			return true;
	}
	return false;
}

void	Channel::send_message_to_client( std::string message, Client *client ) {

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() != client->get_client_fd())
			send_message(_client_register[i].get_client_fd(), message);
	}
}

// CLEAR FUNCTIONS //

void	Channel::client_clear( int fd ) {

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() == fd)
			{_client_register.erase(_client_register.begin() + i); break;}
	}
}