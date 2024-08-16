class Client {

	public :

		Client(){};
		~Client(){};

		// Getters //

		const int&		get_client_fd() const{ return this->_client_fd; }

		const std::string&	get_command_buffer() const{ return _command_buffer; }
		const std::string&	get_client_nickname() const{ return this->_nickname; }
		const std::string&	get_client_username() const{ return this->_username; }
		const std::string&	get_client_hostname() const{ return this->_hostname; }
		const std::string&	get_client_servername() const{ return this->_servername; }
		const std::string&	get_client_realname() const{ return this->_realname; }

		bool	get_connected_status() const{ return this->_connected; }
		bool	get_user_status() const{ return this->_user_checked; }
		bool	get_nick_status() const{ return this->_nick_checked; }
		bool	get_oper_status() const{ return this->_oper_checked; }

		// Setters //

		void	set_client_fd( const int& fd ){ this->_client_fd = fd; }
		void	set_client_ip_addr( const std::string& ip_addr ){ this->_client_ip_addr = ip_addr; }

		void	set_command_buffer( const std::string& buffer ){ _command_buffer = buffer; }
		void	set_client_nickname( const std::string& nickname ){ this->_nickname = nickname; }
		void	set_client_username( const std::string& username ){ this->_username = username; }
		void	set_client_hostname( const std::string& hostname ){ this->_hostname = hostname; }
		void	set_client_servername( const std::string& servername ){ this->_servername = servername; }
		void	set_client_realname( const std::string& realname ){ this->_realname = realname; }

		void	set_connected_status( bool status ){ this->_connected = status; }
		void	set_user_status( bool status ){ this->_user_checked = status; }
		void	set_nick_status( bool status ){ this->_nick_checked = status; }
		void	set_oper_status( bool status ){ this->_oper_checked = status; }

		// Function //

		static void	send_message( int fd, std::string message ){ send(fd, message.c_str(), message.length(), 0); }

	private :

		int		_client_fd;
		std::string	_client_ip_addr;

		std::string _command_buffer;
		std::string _nickname;
		std::string _username;
		std::string _hostname;
		std::string _servername;
		std::string _realname;

		bool	_connected;
		bool	_user_checked;
		bool	_nick_checked;
		bool	_oper_checked;
};