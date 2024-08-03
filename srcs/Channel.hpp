#include <vector>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>

#include "Client.hpp"

class Channel {

	public :

		Channel(){};
		~Channel(){};

		// Getters //

		std::string	get_channel_name(){ return this->_name; }
		bool	get_channel_invite_status(){ return this->_invite_status; }

		// Setters //

		void	set_channel_name( std::string name ){ this->_name = name; }
		void	set_channel_invite_status( bool status ){ this->_invite_status = status; }

		// Functions //

		void	add_new_client( Client client ){ this->_client_register.push_back(client); }
		bool	check_existing_client( int fd );
		void	send_message_to_client( std::string message, Client *client );
		void	send_message( int fd, std::string message ){ send(fd, message.c_str(), message.length(), 0); }

		// Clear Functions //

		void	client_clear( int fd );

	private :

	std::string	_name;

	std::vector<Client> _client_register;

	bool	_invite_status;
};