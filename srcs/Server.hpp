#include <unistd.h>
#include <string.h>

#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>

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
		int		get_socket_fd() const{ return this->_socket_fd; }
		int		get_port() const{ return this->_port; }
		std::string	get_password() const{ return this->_password; }
		std::string	get_oper_password() const{ return this->_oper_password; }
		Client		*get_client_by_fd( int fd );
		Client		*get_client_by_username( std::string username );
		Client		*get_client_by_nickname( std::string nickname );
		Channel		*get_channel( std::string name );

		// Setters //

		void	set_socket_fd( int fd ){ this->_socket_fd = fd; }
		void	set_port( int port ){ this->_port = port; }
		void	set_password( std::string password ){ this->_password = password; }
		void	set_oper_password(){ this->_oper_password = "operator"; }

		// Functions //

		static void	signal_handler( int signum );

		void	send_message( int fd, std::string message ){ send(fd, message.c_str(), message.length(), 0); }
		void	server_init( int port, std::string password );
		void	socket_creation();
		void	new_client_request();
		void	data_receiver( int fd );
		bool	check_existing_channel( std::string channel_name );
		bool	check_existing_client_by_username( std::string username );
		bool	check_existing_client_by_nickname( std::string nickname );

		// Command Functions //

		void	pass_command( std::vector<std::string> command_parsed, Client *client );
		void	user_command( std::vector<std::string> command_parsed, Client *client );
		void	nick_command( std::vector<std::string> command_parsed, Client *client );
		void	quit_command( std::vector<std::string> command_parsed, Client *client );
		void	join_command( std::vector<std::string> command_parsed, Client *client );
		void	part_command( std::vector<std::string> command_parsed, Client *client );
		void	privmsg_command( std::vector<std::string> command_parsed, Client *client );
		void	oper_command( std::vector<std::string> command_parsed, Client *client );
		void	kick_command( std::vector<std::string> command_parsed, Client *client );
		void	invite_command( std::vector<std::string> command_parsed, Client *client );
		// void			topic_command(int fd, std::vector<std::string> command_parsed);
		// void			mode_command(int fd, std::vector<std::string> command_parsed);

		// Clear Functions //

		void	client_clear( int fd );
		void	close_socket_fd();

	private :

		static bool		_signal;

		int		_port;
		int		_socket_fd;

		std::string	_password;
		std::string	_oper_password;

		std::vector<Client>	_client_register;
		std::vector<struct pollfd>	_pollfd_register;
		std::vector<Channel> _channel_register;

		std::vector<std::string>	_command_raw;
};