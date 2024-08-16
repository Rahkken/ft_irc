#include <unistd.h> // close()
#include <string.h> // strcmp()
#include <stdlib.h> // strtoul()
#include <arpa/inet.h> // struct sockaddr_in
#include <fcntl.h> // fcntl()
#include <poll.h> // poll()

#define white "\e[0;37m"
#define red "\e[1;31m"
#define green "\e[1;32m"
#define yellow "\e[1;33m"

#include "Channel.hpp"

class Server {

	public :

		Server(){};
		~Server(){};

		// Getters //

		bool	get_signal() const{ return this->_signal; }
		const int&		get_socket_fd() const{ return this->_socket_fd; }
		const int&		get_port() const{ return this->_port; }
		const std::string&	get_password() const{ return this->_password; }
		const std::string&	get_oper_password() const{ return this->_oper_password; }
		Client		*get_client_by_fd( const int& fd );
		Client		*get_client( const std::string& nickname );
		Channel		*get_channel( const std::string& name );

		// Setters //

		void	set_socket_fd( const int& fd ){ this->_socket_fd = fd; }
		void	set_port( const int& port ){ this->_port = port; }
		void	set_password( const std::string& password ){ this->_password = password; }
		void	set_oper_password(){ this->_oper_password = "operator"; }

		// Functions //

		static void	signal_handler( int signum );
		bool	is_digit( const std::string &str ){ return str.find_first_not_of("0123456789") == std::string::npos; }

		void	server_init( const int& port, const std::string& password );
		void	socket_creation();
		void	new_client_request();
		void	data_receiver( const int& fd );

		// Check Functions //

		bool	check_existing_channel( const std::string& channel_name ) const;
		bool	check_existing_client( const std::string& nickname ) const;
		bool	check_valid_nickname( const std::string &nickname ) const;

		// Clear Functions //

		void	client_clear( const int& fd );
		void	close_socket_fd();

		// Command Functions //

		void	pass_command( std::vector<std::string> command_parsed, Client *client );
		void	user_command( std::vector<std::string> command_parsed, Client *client );
		void	nick_command( std::vector<std::string> command_parsed, Client *client );
		void	quit_command( Client *client );
		void	oper_command( std::vector<std::string> command_parsed, Client *client );
		void	privmsg_command( std::vector<std::string> command_parsed, Client *client );
		void	join_command( std::vector<std::string> command_parsed, Client *client );
		void	part_command( std::vector<std::string> command_parsed, Client *client );
		void	mode_command( std::vector<std::string> command_parsed, Client *client );
		void	kick_command( std::vector<std::string> command_parsed, Client *client );
		void	invite_command( std::vector<std::string> command_parsed, Client *client );
		void	topic_command( std::vector<std::string> command_parsed, Client *client );

	private :

		static bool	_signal;

		int		_port;
		int		_socket_fd;

		std::string	_password;
		std::string	_oper_password;

		std::vector<Client>	_client_register;
		std::vector<struct pollfd>	_pollfd_register;
		std::vector<Channel>	_channel_register;

		std::vector<std::string>	_command_raw;
};