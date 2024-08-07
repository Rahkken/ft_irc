#include "Server.hpp"

// COMMANDS FUNCTIONS //

void	Server::pass_command( std::vector<std::string> command_parsed, Client *client ) {

	if (command_parsed.size() == 2) {

		if (!strcmp(command_parsed[1].c_str(), get_password().c_str())) {

			client->set_connected_status(true);
			Client::send_message(client->get_client_fd(), "You are now connected. Welcome to IRC world !\n");
			Client::send_message(client->get_client_fd(), "Create your account {NICK <nickname> | USER <username> <hostname> <servername> <realname>}.\n");
			std::cout << green << "Client [" << client->get_client_fd() << "] is now connected." << white << std::endl;
		}
		else {

			Client::send_message(client->get_client_fd(), "Bad password... Bye-bye !\n");
			std::cout << red << "Client [" << client->get_client_fd() << "] entered the wrong password." << white << std::endl;
			client_clear(client->get_client_fd());
			close(client->get_client_fd());
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Try {PASS <password>}.\n");
}

void	Server::user_command( std::vector<std::string> command_parsed, Client *client ) {

	if (command_parsed.size() == 5) {

		if (!client->get_user_status()) {

			if (get_client_by_username(command_parsed[1]))
				{Client::send_message(client->get_client_fd(), "Username already used. Try another !\n"); return;}
			client->set_client_username(command_parsed[1]);
			client->set_client_hostname(command_parsed[2]);
			client->set_client_servername(command_parsed[3]);
			client->set_client_realname(command_parsed[4]);
			std::cout << "Client [" << client->get_client_fd() << "] :" << std::endl;
			std::cout << "<username> : " << client->get_client_username() << std::endl;
			std::cout << "<hostname> : " << client->get_client_hostname() << std::endl;
			std::cout << "<servname> : " << client->get_client_servername() << std::endl;
			std::cout << "<realname> : " << client->get_client_realname() << std::endl;
			std::cout << "Username, hostname, servname and realname setted." << std::endl;
			Client::send_message(client->get_client_fd(), "Username, hostname, servname and realname setted !\n");
			client->set_user_status(true);
		}
		else
			Client::send_message(client->get_client_fd(), "Username settings already done.\n");
	}
	else
		Client::send_message(client->get_client_fd(), "Bad parameters {USER <username> <hostname> <servername> <realname>}.\n");
}

void	Server::nick_command( std::vector<std::string> command_parsed, Client *client ) {

	if (command_parsed.size() == 2) {

		if (!client->get_nick_status()) {

			if (get_client_by_nickname(command_parsed[1]))
				{Client::send_message(client->get_client_fd(), "Nickname already used. Try another !\n"); return;}
			client->set_client_nickname(command_parsed[1]);
			std::cout << "Client [" << client->get_client_fd() << "] :" << std::endl;
			std::cout << "<nickname> : " << client->get_client_nickname() << std::endl;
			std::cout << "Nickname setted." << std::endl;
			Client::send_message(client->get_client_fd(), "Nickname setted !\n");
			client->set_nick_status(true);
		}
		else
			Client::send_message(client->get_client_fd(), "Nickname settings already done.\n");
	}
	else
		Client::send_message(client->get_client_fd(), "Bad parameters {NICK <nickname>}.\n");
}

void	Server::quit_command( std::vector<std::string> command_parsed, Client *client ) {

	if (command_parsed.size() == 1) {

		std::cout << red << "Client [" << client->get_client_username() << "] quit the IRC server." << white << std::endl;
		Client::send_message(client->get_client_fd(), "You exited the server.\n");
		client_clear(client->get_client_fd());
		close(client->get_client_fd());
	}
	Client::send_message(client->get_client_fd(), "No parameters needed.\n");
}

void	Server::oper_command( std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() == 3) {

		if (!check_existing_client_by_username(command_parsed[1]))
			{Client::send_message(client->get_client_fd(), "This client doesn't exist.\n"); return;}
		if (!strcmp(client->get_client_username().c_str(), command_parsed[1].c_str())) {

			if (!strcmp(command_parsed[2].c_str(), get_oper_password().c_str())) {

				std::cout << "Client [" << client->get_client_username() << "] is now an operator." << std::endl;
				Client::send_message(client->get_client_fd(), "You are now an operator.\n");
				client->set_oper_status(true);
			}
			else
				Client::send_message(client->get_client_fd(), "Wrong password.\n");
		}
		else
			Client::send_message(client->get_client_fd(), "You aren't this client.\n");
	}
	else
		Client::send_message(client->get_client_fd(), "Bad parameters {OPER <username> <password>}.\n");
}

void	Server::privmsg_command( std::vector<std::string> command_parsed,  Client *client ) {

	if (command_parsed.size() >= 3) {

		std::string	message;

		message += ":" + client->get_client_username() + "@hostname PRIVMSG " + command_parsed[1] + " :" + command_parsed[2];
		for (unsigned long int i = 3; i < command_parsed.size(); i++)
			message += " " + command_parsed[i];
		message += "\n";

		if (check_existing_client_by_username(command_parsed[1])) {

			Client *client_receiver = get_client_by_username(command_parsed[1]);

			Client::send_message(client_receiver->get_client_fd(), message);
			Client::send_message(client->get_client_fd(), "Message sent to this user.\n");
			std::cout << "Client [" << client->get_client_username() << "] send a message to Client [" << client_receiver->get_client_username() << "]." << std::endl;
		}
		else if (command_parsed[1][0] == '#' && check_existing_channel(command_parsed[1].erase(0, 1))) {

			Channel *channel = get_channel(command_parsed[1]);

			if (channel->check_existing_client(client)) {
				
				channel->send_message_to_client(message);
				Client::send_message(client->get_client_fd(), "Message sent to this channel.\n");
				std::cout << "Client [" << client->get_client_username() << "] send a message to Channel [" << channel->get_channel_name() << "]." << std::endl;
			}
			else
				Client::send_message(client->get_client_fd(), "You are not in this channel.\n");
		}
		else
			Client::send_message(client->get_client_fd(), "Receiver doesn't exist.\n");
	}
	else
		Client::send_message(client->get_client_fd(), "Bad parameters {PRIVMSG <user/channel> <message>}.\n");
}

void	Server::join_command( std::vector<std::string> command_parsed, Client *client ) {

	if (command_parsed.size() == 2 || command_parsed.size() == 3) {

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
			std::cout << "Channel [" << channel.get_channel_name() << "] created by Client [" << client->get_client_username() << "]." << std::endl;
			
			// Ici --------------
			std::string message;

			message += ":" + client->get_client_username() + " JOIN #" + channel.get_channel_name() + "\n";
			channel.send_message_to_client(message);

			message.clear();
			message += ":IRC 353 " + client->get_client_username() + "  = #" + channel.get_channel_name() + " :@" + client->get_client_username() + "\n";
			Client::send_message(client->get_client_fd(), message);

			message.clear();
			message += ":IRC 366 " + client->get_client_username() + "  #" + channel.get_channel_name() + " :END of /NAMES list\n";
			Client::send_message(client->get_client_fd(), message);
			// ------------------
		}
		else {

			Channel *channel = get_channel(command_parsed[1]);

			if (channel->get_key_status()) {

				if (command_parsed.size() != 3)
					{Client::send_message(client->get_client_fd(), "You have to enter a password {JOIN <channel> <password>}.\n"); return;}
				else if (strcmp(channel->get_channel_password().c_str(), command_parsed[2].c_str()))
					{Client::send_message(client->get_client_fd(), "Bad password try again.\n"); return;}
			}
			if (channel->get_invite_status())
				{Client::send_message(client->get_client_fd(), "It's an invite only channel.\n"); return;}
			if (channel->get_limit_status()) {

				if (channel->get_user_limit() >= channel->get_channel_size())
					{Client::send_message(client->get_client_fd(), "Limit is already reached.\n"); return;}
			}
			if (!channel->check_existing_client(client)) {

				channel->add_new_client(client);
				std::cout << "Client [" << client->get_client_username() << "] entered the Channel [" << channel->get_channel_name() << "]." << std::endl;
				Client::send_message(client->get_client_fd(), "You entered this channel.\n");

				// Ici ----------------
				std::string message;

				message += ":" + client->get_client_username() + " JOIN #" + channel->get_channel_name() + "\n";
				channel->send_message_to_client(message);

				message.clear();
				message += ":IRC 353 " + client->get_client_username() + "  = #" + channel->get_channel_name() + " :@" + client->get_client_username() + "\n";
				Client::send_message(client->get_client_fd(), message);

				message.clear();
				message += ":IRC 366 " + client->get_client_username() + "  #" + channel->get_channel_name() + " :END of /NAMES list\n";
				Client::send_message(client->get_client_fd(), message);
				// --------------------
			}
			else
				Client::send_message(client->get_client_fd(), "You are already in this channel.\n");
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Bad parameters {JOIN <channel>}.\n");
}

void	Server::part_command( std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() >= 2) {

		if (command_parsed[1][0] != '#')
			Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n");
		else if (!check_existing_channel(command_parsed[1].erase(0, 1)))
			Client::send_message(client->get_client_fd(), "This channel doesn't exist.\n");
		else {

			Channel *channel = get_channel(command_parsed[1]);
			if (!channel->check_existing_client(client))
				Client::send_message(client->get_client_fd(), "You aren't in this channel.\n");
			else {
				
				channel->client_clear(client->get_client_fd());
				std::cout << "Client [" << client->get_client_username() << "] quitted the Channel [" << channel->get_channel_name() << "]." << std::endl;
				Client::send_message(client->get_client_fd(), "You are now removed from this channel.\n");
			}
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Bad parameters {PART <channel>}.\n");
}

void	Server::mode_command( std::vector<std::string> command_parsed, Client *client ) {
	
	if (command_parsed.size() >= 3 && command_parsed.size() < 5) {
		
		if (command_parsed[1][0] != '#')
			Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n");
		else if (!check_existing_channel(command_parsed[1].erase(0, 1)))
			Client::send_message(client->get_client_fd(), "This channel doesn't exist.\n");
		else {

			Channel *channel= get_channel(command_parsed[1]);

			if (!channel->check_operator_status(client))
				Client::send_message(client->get_client_fd(), "You don't have rights to use MODE command here.\n");
			else {

				if (command_parsed[2] == "+k") {

					if (command_parsed.size() == 4) {

						channel->set_channel_password(command_parsed[3]);
						channel->set_key_status(true);
						std::cout << "Client [" << client->get_client_username() << "] set a password to Channel [" << channel->get_channel_name() << "]." << std::endl;
						Client::send_message(client->get_client_fd(), "Password added successfully.\n");

						std::string	message;

						message += "Client [" + client->get_client_username() + "] added a password to Channel [" + channel->get_channel_name() + "] : <" + command_parsed[3] + ">.\n";
						channel->send_message_to_client(message);
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> +k <password>}.\n");
				}
				else if (command_parsed[2] == "-k") {

					if (command_parsed.size() == 3) {

						channel->set_key_status(false);
						std::cout << "Client [" << client->get_client_username() << "] removed the password from Channel [" << channel->get_channel_name() << "]." << std::endl;
						Client::send_message(client->get_client_fd(), "Password removed successfully.\n");

						std::string	message;

						message += "Client [" + client->get_client_username() + "] removed the password from Channel [" + channel->get_channel_name() + "].\n";
						channel->send_message_to_client(message);
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> -k}.\n");
				}
				else if (command_parsed[2] == "+i") {

					if (command_parsed.size() == 3) {

						channel->set_invite_status(true);
						std::cout << "Client [" << client->get_client_username() << "] set Invite-Only to Channel [" << channel->get_channel_name() << "]." << std::endl;
						Client::send_message(client->get_client_fd(), "Invite-Only mode set successfully.\n");

						std::string	message;

						message += "Client [" + client->get_client_username() + "] set Invite-Only to Channel [" + channel->get_channel_name() + "].\n";
						channel->send_message_to_client(message);
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> +i}.\n");
				}
				else if (command_parsed[2] == "-i") {

					if (command_parsed.size() == 3) {

						channel->set_invite_status(false);
						std::cout << "Client [" << client->get_client_username() << "] removed Invite-Only from Channel [" << channel->get_channel_name() << "]." << std::endl;
						Client::send_message(client->get_client_fd(), "Invite-Only mode removed successfully.\n");

						std::string	message;

						message += "Client [" + client->get_client_username() + "] removed Invite-Only from Channel [" + channel->get_channel_name() + "].\n";
						channel->send_message_to_client(message);
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> -i}.\n");
				}
				else if (command_parsed[2] == "+t") {

					if (command_parsed.size() == 3) {

						channel->set_topic_status(true);
						std::cout << "Client [" << client->get_client_username() << "] set Topic rights to Channel [" << channel->get_channel_name() << "]." << std::endl;
						Client::send_message(client->get_client_fd(), "Topic rights set successfully.\n");

						std::string	message;

						message += "Client [" + client->get_client_username() + "] set Topic rights to Channel [" + channel->get_channel_name() + "].\n";
						channel->send_message_to_client(message);
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> +t}.\n");
				}
				else if (command_parsed[2] == "-t") {

					if (command_parsed.size() == 3) {

						channel->set_topic_status(false);
						std::cout << "Client [" << client->get_client_username() << "] removed Topic rights to Channel [" << channel->get_channel_name() << "]." << std::endl;
						Client::send_message(client->get_client_fd(), "Topic rights removed successfully.\n");

						std::string	message;

						message += "Client [" + client->get_client_username() + "] removed Topic rights to Channel [" + channel->get_channel_name() + "].\n";
						channel->send_message_to_client(message);
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> -t}.\n");
				}
				else if (command_parsed[2] == "+o") {

					if (command_parsed.size() == 4) {

						if (!get_client_by_username(command_parsed[3]))
							Client::send_message(client->get_client_fd(), "This client doesn't exist.\n");
						else if (!channel->check_existing_client(get_client_by_username(command_parsed[3])))
							Client::send_message(client->get_client_fd(), "This client isn't in this channel.\n");
						else if (channel->check_operator_status(get_client_by_username(command_parsed[3])))
							Client::send_message(client->get_client_fd(), "This client already got operator rights.\n");
						else {
						
							channel->add_operator(get_client_by_username(command_parsed[3]));
							std::cout << "Client [" << client->get_client_username() << "] give operator rights to Client [" << command_parsed[3] << "] in Channel [" << channel->get_channel_name() << "]." << std::endl;
							Client::send_message(client->get_client_fd(), "Operator rights successfully given.\n");

							std::string	message;

							message += "Client [" + client->get_client_username() + "] give you operator rights in Channel [" + channel->get_channel_name() + "].\n";
							channel->send_message_to_client(message);
						}
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> +o <user>}.\n");
				}
				else if (command_parsed[2] == "-o") {

					if (command_parsed.size() == 4) {

						if (!get_client_by_username(command_parsed[3]))
							Client::send_message(client->get_client_fd(), "This client doesn't exist.\n");
						else if (!channel->check_existing_client(get_client_by_username(command_parsed[3])))
							Client::send_message(client->get_client_fd(), "This client isn't in this channel.\n");
						else if (!channel->check_operator_status(get_client_by_username(command_parsed[3])))
							Client::send_message(client->get_client_fd(), "This client doesn't have operator rights.\n");
						else if (!channel->check_operator_status(get_client_by_username(client->get_client_username())))
							Client::send_message(client->get_client_fd(), "You can't remove your operator rights.\n");
						else {
						
							channel->remove_operator_status(*get_client_by_username(command_parsed[3]));
							std::cout << "Client [" << client->get_client_username() << "] removed operator rights to Client [" << command_parsed[3] << "] in Channel [" << channel->get_channel_name() << "]." << std::endl;
							Client::send_message(client->get_client_fd(), "Operator rights successfully removed.\n");

							std::string	message;

							message += "Client [" + client->get_client_username() + "] removed your operator rights in Channel [" + channel->get_channel_name() + "].\n";
							channel->send_message_to_client(message);
						}
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> -o <user>}.\n");
				}
				else if (command_parsed[2] == "+l") {

					if (command_parsed.size() == 4) {

						if (is_digit(command_parsed[3])) {

							std::cout << strtoul(command_parsed[3].c_str(), NULL, 0) << std::endl;
							if (strtoul(command_parsed[3].c_str(), NULL, 0) >= _client_register.size())
								channel->set_user_limit(atoi(command_parsed[3].c_str()));
							else
								{Client::send_message(client->get_client_fd(), "Limit too low.\n"); return ;}
						}
						else
							{Client::send_message(client->get_client_fd(), "Invalid limit.\n"); return ;}
						channel->set_limit_status(true);
						std::cout << "Client [" << client->get_client_username() << "] set a limit to Channel [" << channel->get_channel_name() << "] of " << command_parsed[3] << " users." <<std::endl;
						Client::send_message(client->get_client_fd(), "Limit set successfully.\n");

						std::string	message;

						message += "Client [" + client->get_client_username() + "] set a limit to Channel [" + channel->get_channel_name() + "] of " + command_parsed[3] + " users.\n";
						channel->send_message_to_client(message);
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> +l <limit>}.\n");
				}
				else if (command_parsed[2] == "-l") {

					if (command_parsed.size() == 3) {

						channel->set_limit_status(false);
						std::cout << "Client [" << client->get_client_username() << "] removed limit to Channel [" << channel->get_channel_name() << "]." << std::endl;
						Client::send_message(client->get_client_fd(), "Limit removed successfully.\n");

						std::string	message;

						message += "Client [" + client->get_client_username() + "] removed limit to Channel [" + channel->get_channel_name() + "].\n";
						channel->send_message_to_client(message);
					}
					else
						Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> -l}.\n");
				}
				else
					Client::send_message(client->get_client_fd(), "You tried to change a non existant mode.\n");
			}
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Bad parameters {MODE <channel> {[+|-]k|i|t|o|l} [<limit>] [<user>]}.\n");
}

void	Server::kick_command( std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() == 4)
		command_parsed.erase(command_parsed.begin()+1);
	if (command_parsed.size() == 3) {

		if (command_parsed[2][0] == ':')
			command_parsed[2].erase(0, 1);
		if (command_parsed[1][0] != '#')
			Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n");
		else if (!check_existing_channel(command_parsed[1].erase(0, 1)))
			Client::send_message(client->get_client_fd(), "This channel doesn't exist.\n");
		else if (!check_existing_client_by_username(command_parsed[2]))
			Client::send_message(client->get_client_fd(), "This client doesn't exist.\n");
		else if (!strcmp(client->get_client_username().c_str(), command_parsed[2].c_str()))
			Client::send_message(client->get_client_fd(), "You can't kick yourself.\n");
		else {

			Channel *channel = get_channel(command_parsed[1]);
			Client	*client_kicked = get_client_by_username(command_parsed[2]);

			if (!channel->check_operator_status(client))
				{Client::send_message(client->get_client_fd(), "You don't have rights to do it.\n"); return;}
			if (channel->check_existing_client(client_kicked)) {
				
				channel->client_clear(client_kicked->get_client_fd());
				std::cout << "Client [" << client->get_client_username() << "] kicked Client [" << client_kicked->get_client_username() << "] from Channel [" << channel->get_channel_name() << "]." << std::endl;
				
				std::string message;
				message += "You are kicked from Channel [" + channel->get_channel_name() + "] by Client [" + client->get_client_username() + "].\n";
				Client::send_message(client_kicked->get_client_fd(), message);
				Client::send_message(client->get_client_fd(), "You kicked this user.\n");
			}
			else
				Client::send_message(client->get_client_fd(), "This client isn't in this channel.\n");
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Bad parameters {KICK <channel> <username>}.\n");
}

void	Server::invite_command( std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() == 3) {

		if (command_parsed[2][0] != '#')
			Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n");
		else if (!check_existing_channel(command_parsed[2].erase(0, 1)))
			Client::send_message(client->get_client_fd(), "This channel doesn't exist.\n");
		else if (!check_existing_client_by_nickname(command_parsed[1]))
			Client::send_message(client->get_client_fd(), "This client doesn't exist.\n");
		else if (!strcmp(client->get_client_nickname().c_str(), command_parsed[1].c_str()))
			Client::send_message(client->get_client_fd(), "You can't invite yourself.\n");
		else {

			Channel *channel = get_channel(command_parsed[2]);
			Client	*client_invited = get_client_by_nickname(command_parsed[1]);

			if (!channel->check_operator_status(client))
				{Client::send_message(client->get_client_fd(), "You don't have rights to do it.\n"); return;}
			if (!channel->check_existing_client(client_invited)) {
				
				channel->add_new_client(client_invited);
				std::cout << "Client [" << client->get_client_username() << "] invited Client [" << client_invited->get_client_username() << "] in Channel [" << channel->get_channel_name() << "]." << std::endl;
				
				std::string message;
				message += "You are invited in Channel [" + channel->get_channel_name() + "] by Client [" + client->get_client_username() + "].\n";
				Client::send_message(client_invited->get_client_fd(), message);
				Client::send_message(client->get_client_fd(), "You invited this user.\n");
			}
			else
				Client::send_message(client->get_client_fd(), "This client is already in this channel.\n");
		}
	}
	else
		Client::send_message(client->get_client_fd(), "Bad parameters {INVITE <nickname> <channel>}.\n");
}

void	Server::topic_command(std::vector<std::string> command_parsed, Client *client) {

	if (command_parsed.size() >= 2) {

		if (command_parsed.size() > 2 && command_parsed[2][0] == ':')
			command_parsed[2].erase(0, 1);
		if (command_parsed[1][0] != '#')
			{Client::send_message(client->get_client_fd(), "Try with '#' in front the channel name.\n"); return;}
		else if (!check_existing_channel(command_parsed[1].erase(0, 1)))
			{Client::send_message(client->get_client_fd(), "This channel doesn't exist.\n"); return;}

		Channel *channel = get_channel(command_parsed[1]);
		std::string	message;
		std::string new_topic;

		if (command_parsed.size() == 2 && channel->get_channel_topic().size() <= 0)
			Client::send_message(client->get_client_fd(), "This channel doesn't have any topic.\n");
		else if (command_parsed.size() == 2) {

			message += "Topic : " + channel->get_channel_topic() + "\n";
			std::cout << "Client [" << client->get_client_username() << "] asked about "<< command_parsed[1] << "'s topic." << std::endl;
			Client::send_message(client->get_client_fd(), message);
		}
		else if (command_parsed.size() >= 3 && channel->get_topic_status() && channel->check_operator_status(client)) {

			new_topic += command_parsed[2];
			for (unsigned long int i = 3; i < command_parsed.size(); i++)
				new_topic += " " + command_parsed[i];
			channel->set_channel_topic(new_topic);
			message += "You successfully changed " + command_parsed[1] + "'s topic\n";
			std::cout << "Client [" << client->get_client_username() << "] changed "<< command_parsed[1] << "'s topic." << std::endl;
			Client::send_message(client->get_client_fd(), message);
		}
		else if (!channel->check_operator_status(client))
			Client::send_message(client->get_client_fd(), "You don't have the rights to change the topic.\n");
		else if (!channel->get_topic_status())
			Client::send_message(client->get_client_fd(), "Topic mode isn't active, try to change that with {MODE <channel> +t}.\n");
		else
			Client::send_message(client->get_client_fd(), "Bad parameters {TOPIC <channel> [<topic>]}.\n");
	}
	else
			Client::send_message(client->get_client_fd(), "Bad parameters {TOPIC <channel> [<topic>]}.\n");
}
