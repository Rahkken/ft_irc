#include <csignal> // signal()

#include "Server.hpp"

int	main( int argc, char **argv ) {

	if (argc == 3) {
		
		Server ircserv;
		try {

			signal(SIGINT, ircserv.signal_handler);
			signal(SIGQUIT, ircserv.signal_handler);
			ircserv.server_init(atoi(argv[1]), argv[2]);
		}
		catch (std::exception &e) {

			ircserv.close_socket_fd();
			std::cout << e.what() << std::endl;
		}
	}
	else
		std::cout << "Wrong arguments..." << std::endl;
}