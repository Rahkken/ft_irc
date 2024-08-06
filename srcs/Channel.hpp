#include <iostream> // string
#include <sys/socket.h> //
#include <sys/types.h> // send()
#include <vector> // vector

#include "Client.hpp"

class Channel {

	public :

		Channel(){};
		~Channel(){};

		// Getters //

		std::string	get_channel_name() const{ return this->_name; }
		size_t		get_channel_size() const{ return this->_client_register.size(); }

		bool	get_key_status() const{ return this->_key_status; }
		bool	get_invite_status() const{ return this->_invite_status; }
		bool	get_topic_status() const{ return this->_topic_status; }
		bool	get_limit_status() const{ return this->_limit_status; }

		std::string	get_channel_password() const{ return this->_password; }
		std::string	get_channel_topic() const{ return this->_topic; }
		size_t		get_user_limit() const{ return this->_user_limit; }

		// Setters //

		void	set_channel_name( std::string name ){ this->_name = name; }
	
		void	set_key_status( bool status ){ _key_status = status; }
		void	set_invite_status( bool status ){ _invite_status = status; }
		void	set_topic_status( bool status ){ _topic_status = status; }
		void	set_limit_status( bool status ){ _limit_status = status; }
		void	remove_operator_status( Client client );

		void	set_channel_password( std::string password ){ _password = password; }
		void	set_channel_topic( std::string topic ) { _topic = topic; }
		void	set_user_limit( unsigned long int limit ){ _user_limit = limit; }

		// Functions //

		void	add_new_client( Client *client ){ this->_client_register.push_back(*client); }
		void	add_operator( Client *client ){ this->_operator_register.push_back(*client); }
		bool	check_existing_client( Client *client );
		bool	check_operator_status( Client *client );
		void	send_message_to_client( std::string message, Client *client );

		// Clear Functions //

		void	client_clear( int fd );

	private :

		std::string	_name;

		std::vector<Client> _client_register;
		std::vector<Client> _operator_register;

		bool	_invite_status;
		bool 	_topic_status;
		bool 	_key_status;
		bool	_limit_status;

		std::string _topic;
		std::string _password;
		size_t		_user_limit;
};