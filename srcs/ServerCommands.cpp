#include "Server.hpp"

// COMMANDS FUNCTIONS //

void	Server::pass_command( std::vector<std::string> command_parsed, Client *client ) {

	if (command_parsed.size() < 2)
        Client::send_message(client->get_client_fd(), ":IRC 461 PASS :Not enough parameters\r\n");
	else if (command_parsed.size() == 2) {

		if (!strcmp(command_parsed[1].c_str(), get_password().c_str())) {

			client->set_connected_status(true);
			Client::send_message(client->get_client_fd(), "Create your account {NICK <nickname> | USER <username> <mode> <unused> <realname>}.\n");
			std::cout << green << "Client [" << client->get_client_fd() << "] is now connected." << white << std::endl;
		}
		else {

			Client::send_message(client->get_client_fd(), ":IRC 464 :Password incorrect\r\n");
			std::cout << red << "Client [" << client->get_client_fd() << "] entered the wrong password." << white << std::endl;
			client_clear(client->get_client_fd());
			close(client->get_client_fd());
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Try {PASS <password>}.\n");
}

void	Server::user_command( std::vector<std::string> command_parsed, Client *client ) {

	if (command_parsed.size() < 5)
		Client::send_message(client->get_client_fd(), ":IRC 461 USER :Not enough parameters\r\n");
	else if (command_parsed.size() == 5 || command_parsed.size() == 6) {

		if (!client->get_user_status()) {

			if (command_parsed[2] != "0")
				{Client::send_message(client->get_client_fd(), "Mode must be set to '0'.\n"); return;}
			if (command_parsed[3] != "*")
				{Client::send_message(client->get_client_fd(), "Unused parameter must be set to '*'.\n"); return;}
			std::string	realname;
			if (command_parsed.size() == 6)
				realname += command_parsed[4] + ' ' + command_parsed[5];
			else
				realname += command_parsed[4];
			client->set_client_username(command_parsed[1]);
			client->set_client_hostname(command_parsed[2]);
			client->set_client_servername(command_parsed[3]);
			client->set_client_realname(realname);
			std::cout << "Client [" << client->get_client_fd() << "] :" << std::endl;
			std::cout << "<username> : " << client->get_client_username() << std::endl;
			std::cout << "<mode> : " << client->get_client_hostname() << std::endl;
			std::cout << "<unused> : " << client->get_client_servername() << std::endl;
			std::cout << "<realname> : " << client->get_client_realname() << std::endl;
			std::cout << "USER parameters setted." << std::endl;
			Client::send_message(client->get_client_fd(), "USER parameters setted.\n");
			client->set_user_status(true);
			if (client->get_nick_status())
				Client::send_message(client->get_client_fd(), ":IRC 001 " + client->get_client_nickname() + " :You are now connected. Welcome to IRC world ! " + client->get_client_nickname() + "!" + client->get_client_username() + "@localhost\r\n");
		}
		else
			Client::send_message(client->get_client_fd(), ":IRC 462 " + client->get_client_username() + " :You may not reregister\r\n");
	}
	else
		Client::send_message(client->get_client_fd(), "Try {USER <username> <mode> <unused> <realname>}.\n");
}

void	Server::nick_command( std::vector<std::string> command_parsed, Client *client ) {

	if (command_parsed.size() < 2)
		Client::send_message(client->get_client_fd(), ":IRC 431 :No nickname given\r\n");
	else if (command_parsed.size() == 2) {

		if (!client->get_nick_status()) {

			if (!check_valid_nickname(command_parsed[1]))
				{Client::send_message(client->get_client_fd(), ":IRC 432 " + command_parsed[1] + " :Erroneous nickname\r\n"); return;}
			if (get_client(command_parsed[1]))
				{Client::send_message(client->get_client_fd(), ":IRC 433 " + command_parsed[1] + " :Nickname is already in use\r\n"); return;}
			client->set_client_nickname(command_parsed[1]);
			std::cout << "Client [" << client->get_client_fd() << "] :" << std::endl;
			std::cout << "<nickname> : " << client->get_client_nickname() << std::endl;
			std::cout << "Nickname setted." << std::endl;
			Client::send_message(client->get_client_fd(), "Nickname setted !\n");
			client->set_nick_status(true);
			if (client->get_user_status())
				Client::send_message(client->get_client_fd(), ":IRC 001 " + client->get_client_nickname() + " :You are now connected. Welcome to IRC world ! " + client->get_client_nickname() + "!" + client->get_client_username() + "@localhost\r\n");
		}
		else
			Client::send_message(client->get_client_fd(), ":IRC 462 " + client->get_client_nickname() + " :You may not reregister\r\n");
	}
	else
		Client::send_message(client->get_client_fd(), "Try {NICK <nickname>}.\n");
}

void	Server::oper_command( std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() < 3)
        Client::send_message(client->get_client_fd(), ":IRC 461 " + client->get_client_nickname() + " OPER :Not enough parameters\r\n");
	else if (command_parsed.size() == 3) {

		if (!check_existing_client(command_parsed[1]))
			{Client::send_message(client->get_client_fd(), ":IRC 401 " + client->get_client_nickname() + " " + command_parsed[1] + " :No such nickname\r\n"); return;}
		if (!strcmp(client->get_client_nickname().c_str(), command_parsed[1].c_str())) {

			if (!strcmp(command_parsed[2].c_str(), get_oper_password().c_str())) {

				std::cout << "Client [" << client->get_client_nickname() << "] is now an operator." << std::endl;
				Client::send_message(client->get_client_fd(), ":IRC 381 " + client->get_client_nickname() + " :You are now an IRC operator.\r\n");
				client->set_oper_status(true);
			}
			else
				Client::send_message(client->get_client_fd(), ":IRC 464 " + client->get_client_nickname() + " :Password incorrect.\r\n");
		}
		else
			Client::send_message(client->get_client_fd(), "You aren't this client.\n");
	}
	else
		Client::send_message(client->get_client_fd(), "Try {OPER <username> <password>}.\n");
}

void	Server::privmsg_command( std::vector<std::string> command_parsed,  Client *client ) {

	if (command_parsed.size() == 2)
		Client::send_message(client->get_client_fd(), ":IRC 412 " + client->get_client_nickname() + " :No text to send\r\n");
	else if (command_parsed.size() >= 3) {

		std::string	message;

		message += ":" + client->get_client_nickname() + "!" + client->get_client_username() + "@localhost PRIVMSG " + command_parsed[1] + " :" + command_parsed[2];
		for (unsigned long int i = 3; i < command_parsed.size(); i++)
			message += " " + command_parsed[i];
		message += "\r\n";

		if (client->get_client_nickname() == command_parsed[1])
			Client::send_message(client->get_client_fd(), "You can't send to yourself.\n");
		else if (command_parsed[1][0] == '#') {

			if (check_existing_channel(command_parsed[1].erase(0, 1))) {
			
				Channel *channel = get_channel(command_parsed[1]);

				if (channel->check_existing_client(client)) {

					channel->send_message_to_client(message);
					Client::send_message(client->get_client_fd(), "Message sent to this channel.\n");
					std::cout << "Client [" << client->get_client_nickname() << "] send a message to Channel [" << channel->get_channel_name() << "]." << std::endl;
				}
				else
					Client::send_message(client->get_client_fd(), ":IRC 404 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :Cannot send to channel\r\n");
			}
			else
				{Client::send_message(client->get_client_fd(), ":IRC 403 " + client->get_client_nickname() + " #" + command_parsed[1] + " :No such channel\r\n"); return;}
		}
		else if (check_existing_client(command_parsed[1])) {

			Client *client_receiver = get_client(command_parsed[1]);

			Client::send_message(client_receiver->get_client_fd(), message);
			Client::send_message(client->get_client_fd(), "Message sent to this user.\n");
			std::cout << "Client [" << client->get_client_nickname() << "] send a message to Client [" << client_receiver->get_client_nickname() << "]." << std::endl;
		}
		else
			{Client::send_message(client->get_client_fd(), ":IRC 401 " + client->get_client_nickname() + " " + command_parsed[1] + " :No such nickname\r\n"); return;}
	}
	else
		Client::send_message(client->get_client_fd(), "Try {PRIVMSG <user/channel> <message>}.\n");
}

void	Server::join_command( std::vector<std::string> command_parsed, Client *client ) {

	if (command_parsed.size() < 2)
        Client::send_message(client->get_client_fd(), ":IRC 461 " + client->get_client_nickname() + " JOIN :Not enough parameters\r\n");
	else if (command_parsed.size() == 2 || command_parsed.size() == 3) {

		for (size_t i = 0; i < command_parsed[1].size(); i++) {

			if (command_parsed[1][i] == ' ' || command_parsed[1][i] == '\a' || command_parsed[1][i] == ',')
				{Client::send_message(client->get_client_fd(), "Channel name mustn't have ' ', '\a' or ','.\n"); return;}
		}
		if (command_parsed[1][0] != '#')
			Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n");
		else if (!check_existing_channel(command_parsed[1].erase(0, 1))) {

			Channel channel;
			channel.set_channel_name(command_parsed[1]);
			channel.set_key_status(false);
			channel.set_invite_status(false);
			channel.set_topic_status(false);
			channel.set_limit_status(false);
			channel.add_new_client(client);
			channel.add_operator(client);
			_channel_register.push_back(channel);
			std::cout << "Channel [" << channel.get_channel_name() << "] created by Client [" << client->get_client_nickname() << "]." << std::endl;
			channel.send_message_to_client(":" + client->get_client_nickname() + "!" + client->get_client_username() + "@localhost JOIN :#" + channel.get_channel_name() + "\r\n");
			Client::send_message(client->get_client_fd(), ":IRC 332 " + client->get_client_nickname() + " #" + channel.get_channel_name()  + "\r\n");
			Client::send_message(client->get_client_fd(), channel.list_of_users(client));
			Client::send_message(client->get_client_fd(), ":IRC 366 " + client->get_client_nickname() + " #" + channel.get_channel_name()  + " :END of /NAMES list\r\n");
		}
		else {

			Channel *channel = get_channel(command_parsed[1]);

			if (channel->get_key_status()) {

				if (command_parsed.size() != 3)
					{Client::send_message(client->get_client_fd(), ":IRC 475 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :Cannot join channel (+k)\r\n"); return;}
				else if (strcmp(channel->get_channel_password().c_str(), command_parsed[2].c_str()))
					{Client::send_message(client->get_client_fd(), "Wrong password.\n"); return;}
			}
			if (channel->get_invite_status())
				{Client::send_message(client->get_client_fd(), ":IRC 473 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :Cannot join channel (+i)\r\n"); return;}
			if (channel->get_limit_status()) {

				if (channel->get_user_limit() == channel->get_channel_size())
					{Client::send_message(client->get_client_fd(), ":IRC 471 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :Cannot join channel (+l)\r\n"); return;}
			}
			if (!channel->check_existing_client(client)) {

				channel->add_new_client(client);
				std::cout << "Client [" << client->get_client_nickname() << "] entered the Channel [" << channel->get_channel_name() << "]." << std::endl;
				channel->send_message_to_client(":" + client->get_client_nickname() + "!" + client->get_client_username() + "@localhost JOIN :#" + channel->get_channel_name() + "\r\n");
				Client::send_message(client->get_client_fd(), ":IRC 332 " + client->get_client_nickname() + " #" + channel->get_channel_name()  + "\r\n");
				Client::send_message(client->get_client_fd(), channel->list_of_users(client));
				Client::send_message(client->get_client_fd(), ":IRC 366 " + client->get_client_nickname() + " #" + channel->get_channel_name()  + " :END of /NAMES list\r\n");
			}
			else
				Client::send_message(client->get_client_fd(), "You are already in this channel.\n");
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Try {JOIN <channel>}.\n");
}

void	Server::part_command( std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() < 2)
        Client::send_message(client->get_client_fd(), ":IRC 461 " + client->get_client_nickname() + " PART :Not enough parameters\r\n");
	else if (command_parsed.size() >= 2) {

		if (command_parsed[1][0] != '#')
			Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n");
		else if (!check_existing_channel(command_parsed[1].erase(0, 1)))
			Client::send_message(client->get_client_fd(), ":IRC 403 " + client->get_client_nickname() + " #" + command_parsed[1] + " :No such channel\r\n");
		else {

			Channel *channel = get_channel(command_parsed[1]);
			if (!channel->check_existing_client(client))
				Client::send_message(client->get_client_fd(), ":IRC 442 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :You're not on that channel\r\n");
			else {
				
				channel->client_clear(client->get_client_fd());
				std::cout << "Client [" << client->get_client_nickname() << "] quitted the Channel [" << channel->get_channel_name() << "]." << std::endl;
				Client::send_message(client->get_client_fd(), ":PART #" + channel->get_channel_name() + " " + client->get_client_nickname() + "\r\n");
				Client::send_message(client->get_client_fd(), ":" + client->get_client_nickname() + "!" + client->get_client_username() + "@localhost PART #" + channel->get_channel_name() + "\r\n");
				channel->send_message_to_client(":" + client->get_client_nickname() + "!" + client->get_client_username() + "@localhost PART #" + channel->get_channel_name() + "\r\n");
			}
		}
	}
}

void	Server::mode_command( std::vector<std::string> command_parsed, Client *client ) {
	
	if (command_parsed.size() < 3)
        Client::send_message(client->get_client_fd(), ":IRC 461 " + client->get_client_nickname() + " MODE :Not enough parameters\r\n");
	else if (command_parsed.size() >= 3 && command_parsed.size() < 5) {
		
		if (command_parsed[1][0] != '#')
			Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n");
		else if (!check_existing_channel(command_parsed[1].erase(0, 1)))
			Client::send_message(client->get_client_fd(), ":IRC 403 " + client->get_client_nickname() + " #" + command_parsed[1] + " :No such channel\r\n");
		else {

			Channel *channel= get_channel(command_parsed[1]);

			if (!channel->check_existing_client(client))
				{Client::send_message(client->get_client_fd(), ":IRC 442 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :You're not on that channel\r\n"); return;}
			if (!channel->check_operator_status(client))
				{Client::send_message(client->get_client_fd(), ":IRC 482 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :You're not channel operator\r\n"); return;}
			else {

				if (command_parsed[2] == "+k") {

					if (command_parsed.size() == 4) {

						channel->set_channel_password(command_parsed[3]);
						channel->set_key_status(true);
						std::cout << "Client [" << client->get_client_nickname() << "] set a password to Channel [" << channel->get_channel_name() << "]." << std::endl;
						channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " +k\r\n");
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> +k <password>}.\n");
				}
				else if (command_parsed[2] == "-k") {

					if (command_parsed.size() == 3) {

						channel->set_key_status(false);
						std::cout << "Client [" << client->get_client_nickname() << "] removed the password from Channel [" << channel->get_channel_name() << "]." << std::endl;
						channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " -k\r\n");
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> -k}.\n");
				}
				else if (command_parsed[2] == "+i") {

					if (command_parsed.size() == 3) {

						channel->set_invite_status(true);
						std::cout << "Client [" << client->get_client_nickname() << "] set Invite-Only to Channel [" << channel->get_channel_name() << "]." << std::endl;
						channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " +i\r\n");
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> +i}.\n");
				}
				else if (command_parsed[2] == "-i") {

					if (command_parsed.size() == 3) {

						channel->set_invite_status(false);
						std::cout << "Client [" << client->get_client_nickname() << "] removed Invite-Only from Channel [" << channel->get_channel_name() << "]." << std::endl;
						channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " -i\r\n");
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> -i}.\n");
				}
				else if (command_parsed[2] == "+t") {

					if (command_parsed.size() == 3) {

						channel->set_topic_status(true);
						std::cout << "Client [" << client->get_client_nickname() << "] set Topic rights to Channel [" << channel->get_channel_name() << "]." << std::endl;
						channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " +t\r\n");
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> +t}.\n");
				}
				else if (command_parsed[2] == "-t") {

					if (command_parsed.size() == 3) {

						channel->set_topic_status(false);
						std::cout << "Client [" << client->get_client_nickname() << "] removed Topic rights to Channel [" << channel->get_channel_name() << "]." << std::endl;
						channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " -t\r\n");
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> -t}.\n");
				}
				else if (command_parsed[2] == "+o") {

					if (command_parsed.size() == 4) {

						if (!get_client(command_parsed[3]))
							Client::send_message(client->get_client_fd(), ":IRC 401 " + client->get_client_nickname() + " " + command_parsed[1] + " :No such nickname\r\n");
						else if (!channel->check_existing_client(get_client(command_parsed[3])))
							Client::send_message(client->get_client_fd(), "This client isn't in this channel.\n");
						else if (channel->check_operator_status(get_client(command_parsed[3])))
							Client::send_message(client->get_client_fd(), "This client already got operator rights.\n");
						else {
						
							channel->add_operator(get_client(command_parsed[3]));
							std::cout << "Client [" << client->get_client_nickname() << "] give operator rights to Client [" << command_parsed[3] << "] in Channel [" << channel->get_channel_name() << "]." << std::endl;
							channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " +o\r\n");
						}
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> +o <user>}.\n");
				}
				else if (command_parsed[2] == "-o") {

					if (command_parsed.size() == 4) {

						if (!get_client(command_parsed[3]))
							Client::send_message(client->get_client_fd(), ":IRC 401 " + client->get_client_nickname() + " " + command_parsed[1] + " :No such nickname\r\n");
						else if (!channel->check_existing_client(get_client(command_parsed[3])))
							Client::send_message(client->get_client_fd(), "This client isn't in this channel.\n");
						else if (!channel->check_operator_status(get_client(command_parsed[3])))
							Client::send_message(client->get_client_fd(), "This client doesn't have operator rights.\n");
						else if (!channel->check_operator_status(get_client(client->get_client_nickname())))
							Client::send_message(client->get_client_fd(), "You can't remove your operator rights.\n");
						else {
						
							channel->remove_operator_status(*get_client(command_parsed[3]));
							std::cout << "Client [" << client->get_client_nickname() << "] removed operator rights to Client [" << command_parsed[3] << "] in Channel [" << channel->get_channel_name() << "]." << std::endl;
							channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " -o\r\n");
						}
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> -o <user>}.\n");
				}
				else if (command_parsed[2] == "+l") {

					std::cout << _client_register.size() << std::endl;
					std::cout << strtoul(command_parsed[3].c_str(), NULL, 0) << std::endl;
					if (command_parsed.size() == 4) {

						if (is_digit(command_parsed[3])) {

							std::cout << strtoul(command_parsed[3].c_str(), NULL, 0) << std::endl;
							if (strtoul(command_parsed[3].c_str(), NULL, 0) >= channel->get_channel_size())
								channel->set_user_limit(atoi(command_parsed[3].c_str()));
							else
								{Client::send_message(client->get_client_fd(), "Limit too low.\n"); return ;}
						}
						else
							{Client::send_message(client->get_client_fd(), "Invalid limit.\n"); return ;}
						channel->set_limit_status(true);
						std::cout << "Client [" << client->get_client_nickname() << "] set a limit to Channel [" << channel->get_channel_name() << "] of " << command_parsed[3] << " users." <<std::endl;
						channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " +l\r\n");
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> +l <limit>}.\n");
				}
				else if (command_parsed[2] == "-l") {

					if (command_parsed.size() == 3) {

						channel->set_limit_status(false);
						std::cout << "Client [" << client->get_client_nickname() << "] removed limit to Channel [" << channel->get_channel_name() << "]." << std::endl;
						channel->send_message_to_client(":IRC 324 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " -l\r\n");
					}
					else
						Client::send_message(client->get_client_fd(), "Try {MODE <channel> -l}.\n");
				}
				else
					Client::send_message(client->get_client_fd(), ":IRC 502 " + client->get_client_nickname() + " :Unknown MODE flag\r\n");
			}
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Try {MODE <channel> {[+|-]k|i|t|o|l} [<limit>] [<user>]}.\n");
}

void	Server::kick_command( std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() < 3)
        Client::send_message(client->get_client_fd(), ":IRC 461 " + client->get_client_nickname() + " KICK :Not enough parameters\r\n");
	else if (command_parsed.size() == 4)
		command_parsed.erase(command_parsed.begin()+1);
	else if (command_parsed.size() == 3) {

		if (command_parsed[2][0] == ':')
			command_parsed[2].erase(0, 1);
		if (command_parsed[1][0] != '#')
			Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n");
		else if (!check_existing_channel(command_parsed[1].erase(0, 1)))
			Client::send_message(client->get_client_fd(), ":IRC 403 " + client->get_client_nickname() + " #" + command_parsed[1] + " :No such channel\r\n");
		else if (!check_existing_client(command_parsed[2]))
			Client::send_message(client->get_client_fd(), ":IRC 401 " + client->get_client_nickname() + " " + command_parsed[2] + " :No such nickname\r\n");
		else if (!strcmp(client->get_client_nickname().c_str(), command_parsed[2].c_str()))
			Client::send_message(client->get_client_fd(), "You can't kick yourself.\n");
		else {

			Channel *channel = get_channel(command_parsed[1]);
			Client	*client_kicked = get_client(command_parsed[2]);

			if (!channel->check_existing_client(client))
				{Client::send_message(client->get_client_fd(), ":IRC 442 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :You're not on that channel\r\n"); return;}
			if (!channel->check_operator_status(client))
				{Client::send_message(client->get_client_fd(), ":IRC 482 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :You're not channel operator\r\n"); return;}
			if (channel->check_existing_client(client_kicked)) {
				
				channel->client_clear(client_kicked->get_client_fd());
				std::cout << "Client [" << client->get_client_nickname() << "] kicked Client [" << client_kicked->get_client_nickname() << "] from Channel [" << channel->get_channel_name() << "]." << std::endl;
				Client::send_message(client_kicked->get_client_fd(), ":KICK #" + channel->get_channel_name() + " " + client_kicked->get_client_nickname() + "\r\n");
				Client::send_message(client_kicked->get_client_fd(), ":" + client_kicked->get_client_nickname() + "!" + client_kicked->get_client_username() + "@localhost PART #" + channel->get_channel_name() + "\r\n");
				channel->send_message_to_client(":" + client_kicked->get_client_nickname() + "!" + client_kicked->get_client_username() + "@localhost PART #" + channel->get_channel_name() + "\r\n");
			}
			else
				Client::send_message(client->get_client_fd(), ":IRC 441 " + client->get_client_nickname() + " " + client_kicked->get_client_nickname() + " #" + channel->get_channel_name() + " :They aren't on that channel\r\n");
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Try {KICK <channel> <username>}.\n");
}

void	Server::invite_command( std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() < 3)
        Client::send_message(client->get_client_fd(), ":IRC 461 " + client->get_client_nickname() + " INVITE :Not enough parameters\r\n");
	else if (command_parsed.size() == 3) {

		if (command_parsed[2][0] != '#')
			Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n");
		else if (!check_existing_channel(command_parsed[2].erase(0, 1)))
			Client::send_message(client->get_client_fd(), ":IRC 403 " + client->get_client_nickname() + " #" + command_parsed[2] + " :No such channel\r\n");
		else if (!check_existing_client(command_parsed[1]))
			Client::send_message(client->get_client_fd(), ":IRC 401 " + client->get_client_nickname() + " " + command_parsed[1] + " :No such nickname\r\n");
		else if (!strcmp(client->get_client_nickname().c_str(), command_parsed[1].c_str()))
			Client::send_message(client->get_client_fd(), "You can't invite yourself.\n");
		else {

			Channel *channel = get_channel(command_parsed[2]);
			Client	*client_invited = get_client(command_parsed[1]);

			if (!channel->check_existing_client(client))
				{Client::send_message(client->get_client_fd(), ":IRC 442 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :You're not on that channel\r\n"); return;}
			if (!channel->check_operator_status(client))
				{Client::send_message(client->get_client_fd(), ":IRC 482 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :You're not channel operator\r\n"); return;}
			if (!channel->check_existing_client(client_invited)) {
				
				if (channel->get_limit_status()) {

					if (channel->get_user_limit() == channel->get_channel_size())
						{Client::send_message(client->get_client_fd(), ":IRC 471 " + client_invited->get_client_nickname() + " #" + channel->get_channel_name() + " :Cannot join channel (+l)\r\n"); return;}
				}
				channel->add_new_client(client_invited);
				std::cout << "Client [" << client->get_client_nickname() << "] invited Client [" << client_invited->get_client_nickname() << "] in Channel [" << channel->get_channel_name() << "]." << std::endl;
				Client::send_message(client->get_client_fd(), ":IRC 341 " + client->get_client_nickname() + " " + client_invited->get_client_nickname() + " #" + channel->get_channel_name() + "\r\n");
				Client::send_message(client_invited->get_client_fd(), ":INVITE " + client_invited->get_client_nickname() + " #" + channel->get_channel_name() + "\r\n");
				channel->send_message_to_client(":" + client_invited->get_client_nickname() + "!" + client_invited->get_client_username() + "@localhost JOIN :#" + channel->get_channel_name() + "\r\n");
				Client::send_message(client_invited->get_client_fd(), ":IRC 332 " + client_invited->get_client_nickname() + " #" + channel->get_channel_name()  + "\r\n");
				Client::send_message(client_invited->get_client_fd(), channel->list_of_users(client));
				Client::send_message(client_invited->get_client_fd(), ":IRC 366 " + client_invited->get_client_nickname() + " #" + channel->get_channel_name()  + " :END of /NAMES list\r\n");
			}
			else
				Client::send_message(client->get_client_fd(), ":IRC 443 " + client->get_client_nickname() + " " + client_invited->get_client_nickname() + " #" + channel->get_channel_name() + " :is already on channel\r\n");
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Try {INVITE <nickname> <channel>}.\n");
}

void	Server::topic_command(std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() < 2)
			Client::send_message(client->get_client_fd(), ":IRC 461 " + client->get_client_nickname() + " TOPIC :Not enough parameters\r\n");
	if (command_parsed.size() >= 2) {

		if (command_parsed.size() > 2 && command_parsed[2][0] == ':')
			command_parsed[2].erase(0, 1);
		if (command_parsed[1][0] != '#')
			{Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n"); return;}
		else if (!check_existing_channel(command_parsed[1].erase(0, 1)))
			{Client::send_message(client->get_client_fd(), ":IRC 403 " + client->get_client_nickname() + " #" + command_parsed[1] + " :No such channel\r\n"); return;}

		Channel *channel = get_channel(command_parsed[1]);
		std::string	message;
		std::string new_topic;

		if (command_parsed.size() == 2 && channel->get_channel_topic().size() <= 0)
			Client::send_message(client->get_client_fd(), ":IRC 331 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :No topic is set\r\n");
		else if (command_parsed.size() == 2) {

			message += ":IRC 332 #" + channel->get_channel_name() + " :" + channel->get_channel_topic() + "\r\n";
			std::cout << "Client [" << client->get_client_nickname() << "] asked about #"<< channel->get_channel_name() << "'s topic." << std::endl;
			Client::send_message(client->get_client_fd(), message);
		}
		else if (command_parsed.size() >= 3 && channel->get_topic_status() && channel->check_operator_status(client)) {

			new_topic += command_parsed[2];
			for (unsigned long int i = 3; i < command_parsed.size(); i++)
				new_topic += " " + command_parsed[i];
			channel->set_channel_topic(new_topic);
			message += "You successfully changed #" + channel->get_channel_name() + "'s topic\n";
			std::cout << "Client [" << client->get_client_nickname() << "] changed "<< channel->get_channel_name() << "'s topic." << std::endl;
			Client::send_message(client->get_client_fd(), message);
		}
		else if (!channel->check_operator_status(client))
			Client::send_message(client->get_client_fd(), ":IRC 482 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :You're not channel operator\n");
		else if (!channel->get_topic_status())
			Client::send_message(client->get_client_fd(), ":IRC 477 " + client->get_client_nickname() + " #" + channel->get_channel_name() + " :Channel doesn't support modes\n");
	}
	else
			Client::send_message(client->get_client_fd(), "Try {TOPIC <channel> [<topic>]}.\n");
}

void	Server::quit_command( Client *client ) {

	std::cout << red << "Client [" << client->get_client_fd() << "] quit the IRC server." << white << std::endl;
	Client::send_message(client->get_client_fd(), "You exited the server.\n");
	client_clear(client->get_client_fd());
	close(client->get_client_fd());
}