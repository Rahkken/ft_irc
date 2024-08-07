#include "Channel.hpp"

// FUNCTIONS //

bool	Channel::check_existing_client( Client *client ) {

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() == client->get_client_fd())
			return true;
	}
	return false;
}

bool	Channel::check_operator_status( Client *client ) {

	for (size_t i = 0; i < _operator_register.size(); i++) {

		if (_operator_register[i].get_client_fd() == client->get_client_fd())
			return true;
	}
	return false;
}

void	Channel::remove_operator_status( Client client ) {
	
	for (unsigned long int i = 0; i < _operator_register.size(); i++) {

		if (client.get_client_fd() == _operator_register[i].get_client_fd())
			_operator_register.erase(_operator_register.begin()+i);
	}
}

void	Channel::send_message_to_client( std::string message ) {

	for (size_t i = 0; i < _client_register.size(); i++) {

		Client::send_message(_client_register[i].get_client_fd(), message);
	}
}

std::string	Channel::list_of_users( Client *client ) {

	std::string message;

	message += ":IRC 353 " + client->get_client_username() + "  = #" + get_channel_name() + " :";
	for (unsigned long int i = 0; i < _client_register.size(); i++) {

		for (unsigned long int j = 0; j < _operator_register.size(); j++) {

			if (_client_register[i].get_client_username() == _operator_register[j].get_client_username())
				message += "@";
		}
		message += _client_register[i].get_client_username() + " ";
	}
	message += "\n";
	return message;
}

// CLEAR FUNCTIONS //

void	Channel::client_clear( int fd ) {

	for (size_t i = 0; i < _client_register.size(); i++) {

		if (_client_register[i].get_client_fd() == fd)
			{_client_register.erase(_client_register.begin() + i); break;}
	}
}