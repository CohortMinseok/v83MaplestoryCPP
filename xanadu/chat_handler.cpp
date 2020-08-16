//

#include "player.hpp"

#include "packetcreator.hpp"
#include "channel.hpp"
#include "world.hpp"
#include "map.hpp"
#include "item.hpp"
#include "mob_data_provider.hpp"
#include "shop_data_provider.hpp"
#include "inventory.hpp"
#include "skill_data_provider.hpp"
#include "skill_data.hpp"
#include "job_constants.hpp"
#include "tools.hpp"
#include "constants.hpp"
#include "pq_constants.hpp"

void Player::handle_chat_command()
{
	signed char type = read<signed char>();
	std::string name = read<std::string>();

	Player *receiver_player = World::get_instance()->GetPlayerByName(name);

	switch (type)
	{
	case chat_command_handler_type_constants::kFind:
	{
		if (!receiver_player || receiver_player->get_is_gm() && !get_is_gm())
		{
			{
				PacketCreator packet;
				packet.FindPlayerReply(name, false);
				send_packet(&packet);
			}

			return;
		}
		if (receiver_player->get_channel_id() == channel_id_)
		{
			if (receiver_player->get_is_in_cash_shop())
			{
				{
					PacketCreator packet;
					packet.FindPlayer(receiver_player->get_name(), find_player_packet_mode2_constants::kCashshop, -1);
					send_packet(&packet);
				}
			}
			else
			{
				{
					PacketCreator packet;
					packet.FindPlayer(receiver_player->get_name(), find_player_packet_mode2_constants::kMap, receiver_player->get_map()->get_id());
					send_packet(&packet);
				}
			}
		}
		else
		{
			{
				PacketCreator packet;
				packet.FindPlayer(receiver_player->get_name(), find_player_packet_mode2_constants::kChannel, receiver_player->get_channel_id());
				send_packet(&packet);
			}
		}
		break;
	}
	case chat_command_handler_type_constants::kWhisper:
	{
		if (receiver_player)
		{
			std::string message = read<std::string>();

			if (message.empty())
			{
				return;
			}
			if (message.length() > 70)
			{
				return;
			}
			{
				PacketCreator packet;
				packet.WhisperPlayer(this, message);
				receiver_player->send_packet(&packet);
			}
			{
				PacketCreator packet;
				packet.FindPlayerReply(receiver_player->get_name(), true);
				send_packet(&packet);
			}
		}
		else
		{
			{
				PacketCreator packet;
				packet.FindPlayerReply(name, false);
				send_packet(&packet);
			}
		}
		break;
	}
	}
}

void Player::handle_use_chat()
{
	World *world = World::get_instance(); // this pointer is often used in this function so declare it right away

	std::string message = read<std::string>();

	if (message.empty())
	{
		PacketCreator packet;
		packet.ShowMessage("Please enter a command.", 5);
		send_packet(&packet);
		return;
	}

	if (message.length() > 70)
	{
		return;
	}

	bool bubble_only = read<bool>(); // Skill macros only display chat bubble

	if (get_is_gm() && message[0] == '!')
	{
		// GM Commands

		std::string command = message.substr(1, message.find(" ") - 1);

		if (command == "w")
		{
			int map_id = stoi(message.substr(message.find(" ") + 1));
			set_map(map_id);
		}

		else if (command == "wt")
		{
			Player *target_player = world->GetPlayerByName(message.substr(message.find(" ") + 1));
			if (target_player)
			{
				change_map(target_player->get_map());
			}
		}

		else if (command == "wh")
		{
			Player *target_player = world->GetPlayerByName(message.substr(message.find(" ") + 1));
			if (target_player)
			{
				target_player->change_map(map_);
			}
		}

		else if (command == "pos")
		{
			{
				// packet
				PacketCreator packet;
				packet.ShowMessage("x: " + std::to_string(position_x_) + " y: " + std::to_string(position_y_) + " foothold: " + std::to_string(foothold_) + " map: " + std::to_string(map_->get_id()), 5);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posa")
		{
			{
				// door skill test
				PacketCreator packet;
				packet.SpawnDoor(1, true, position_x_, position_y_);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posb")
		{
			{
				// packet
				PacketCreator packet;
				packet.ShowMessage("x: " + std::to_string(position_x_) + " y: " + std::to_string(position_y_) + " foothold: " + std::to_string(foothold_) + " map: " + std::to_string(map_->get_id()), 5);
				send_packet(&packet);
			}

			{
				// door skill test
				PacketCreator packet;
				packet.RemoveDoor(1, true);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posc")
		{
			{
				// packet
				PacketCreator packet;
				packet.ShowMessage("x: " + std::to_string(position_x_) + " y: " + std::to_string(position_y_) + " foothold: " + std::to_string(foothold_) + " map: " + std::to_string(map_->get_id()), 5);
				send_packet(&packet);
			}

			{
				// poison mist skill test
				int object_id = 1;
				int skill_id = 2111003;
				signed char skill_level = 10;
				int mist_position_width = 300;
				int mist_position_height = 300;
				PacketCreator packet;
				packet.SpawnMist(object_id, id_, skill_id, position_x_, position_y_, mist_position_width, mist_position_height, skill_level);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posd")
		{
			{
				// packet
				PacketCreator packet;
				packet.ShowMessage("x: " + std::to_string(position_x_) + " y: " + std::to_string(position_y_) + " foothold: " + std::to_string(foothold_) + " map: " + std::to_string(map_->get_id()), 5);
				send_packet(&packet);
			}

			{
				// poison mist skill test
				int object_id = 1;
				PacketCreator packet;
				packet.RemoveMist(object_id);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "pose")
		{
			{
				// packet
				PacketCreator packet;
				packet.obtain_cp(100, 100);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posf")
		{
			{
				// packet
				PacketCreator packet;
				packet.obtain_party_cp(mcpq_constants::kTeamRed, 200, 200);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posg")
		{
			{
				// packet
				PacketCreator packet;
				packet.carnival_pq_message(5);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posh")
		{
			{
				// packet
				PacketCreator packet;
				packet.carnival_pq_summon(0, 2, name_);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posi")
		{
			{
				// packet
				PacketCreator packet;
				packet.carnival_pq_died(10, mcpq_constants::kTeamRed, name_);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posj")
		{
			{
				// packet
				PacketCreator packet;
				packet.leave_carnival_pq(false, mcpq_constants::kTeamBlue, name_);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posk")
		{
			{
				// packet
				PacketCreator packet;
				packet.cpq_show_game_result(11);
				send_packet(&packet);
			}
		}

		// for development purposes only
		else if (command == "posl")
		{
			{
				// packet
				PacketCreator packet;
				packet.ShowMonsterRiding(id_, 1902001, 1004);
				map_->send_packet(&packet, this);
			}
		}

		else if (command == "maxskills")
		{
			// max skills for 4th jobs

			switch (job_)
			{
			case job_ids::kHero:
			case job_ids::kPaladin:
			case job_ids::kDarkKnight:
			case job_ids::kFpArchMage:
			case job_ids::kIlArchMage:
			case job_ids::kBishop:
			case job_ids::kBowmaster:
			case job_ids::kMarksman:
			case job_ids::kNightLord:
			case job_ids::kShadower:
			case job_ids::kBuccaneer:
			case job_ids::kCorsair:
			{
				auto skills_data = SkillDataProvider::get_instance()->get_data();

				for (auto &it : *skills_data)
				{
					int skill_id = it.first;

					if (tools::get_job_id_from_skill_id(skill_id) == job_)
					{
						Skill skill;
						skill.level_ = 20;
						skill.master_level_ = 20;
						skills_[skill_id] = skill;
					}
				}

				{
					// send a packet
					PacketCreator packet;
					packet.UpdateSkills(this);
					send_packet(&packet);
				}
			}
			}
		}

		else if (command == "gmshop")
		{
			int shopid = (10000000 - stoi(message.substr(message.find(" ") + 1)));
			ShopData *gmshop = ShopDataProvider::get_instance()->get_shop_data(shopid);
			if (gmshop)
			{
				shop_ = gmshop;

				{
					// packet
					PacketCreator packet;
					packet.ShowNpcShop(shop_);
					send_packet(&packet);
				}
			}
		}

		else if (command == "item")
		{
			size_t p = message.find(" ");

			if (p == std::string::npos)
			{
				return;
			}

			size_t ap = message.substr(p + 1).find(" ");
			int item_id = 0;
			int amount = 1;

			if (ap == std::string::npos)
			{
				item_id = stoi(message.substr(p));
			}
			else
			{
				item_id = stoi(message.substr(p + 1, ap));
				amount = stoi(message.substr(ap + p + 2));
			}

			give_item(item_id, amount);
		}

		else if (command == "mesos")
		{
			mesos_ = stoi(message.substr(message.find(" ") + 1));
			if (mesos_ < 0)
			{
				mesos_ = 0;
			}

			{
				// packet
				PacketCreator packet;
				packet.UpdateStatInt(kCharacterStatsMesos, mesos_);
				send_packet(&packet);
			}
		}

		else if (command == "level")
		{
			int level_var = stoi(message.substr(message.find(" ") + 1));
			if (level_var > 0)
			{
				set_exp(0);
				set_level(stoi(message.substr(message.find(" ") + 1)));
			}

		}

		else if (command == "levelup")
		{/*
			int loop_var = stoi(message.substr(message.find(" ") + 1));

			if (loop_var <= 1)
			{
				level_up();
			}
			else if (loop_var > 1)
			{
				for (int i = 0; i < loop_var; i++)
				{
					level_up();
				}
			}*/

			int loopvar;
			size_t p = message.find(" ");
			if (p == std::string::npos)
			{
				loopvar = 1;
			}
			else
			{
				loopvar = stoi(message.substr(message.find(" ") + 1));
			}
			for (int i = 0; i < loopvar; i++)
			{
				level_up();
			}
		}

		else if (command == "job")
		{
			set_job(stoi(message.substr(message.find(" ") + 1)));
		}

		else if (command == "sp")
		{
			int sp_var = stoi(message.substr(message.find(" ") + 1));
			if (sp_var <= 0)
			{
				sp_var = 0;
			}
			set_sp(sp_var);
		}

		else if (command == "ap")
		{
			int ap_var = stoi(message.substr(message.find(" ") + 1));
			if (ap_var <= 0)
			{
				ap_var = 0;
			}
			set_ap(ap_var);
		}

		else if (command == "str")
		{
			int str_var = stoi(message.substr(message.find(" ") + 1));
			if (str_var <= 0)
			{
				str_var = 0;
			}
			set_str(str_var);
		}

		else if (command == "dex")
		{
			int dex_var = stoi(message.substr(message.find(" ") + 1));
			if (dex_var <= 0)
			{
				dex_var = 0;
			}
			set_dex(dex_var);
		}

		else if (command == "int")
		{
			int int_var = stoi(message.substr(message.find(" ") + 1));
			if (int_var <= 0)
			{
				int_var = 0;
			}
			set_int(int_var);
		}

		else if (command == "luk")
		{
			int luk_var = stoi(message.substr(message.find(" ") + 1));
			if (luk_var <= 0)
			{
				luk_var = 0;
			}
			set_luk(luk_var);
		}

		else if (command == "hp")
		{
			short hp = stoi(message.substr(message.find(" ") + 1));
			set_max_hp(hp);
			set_hp(hp);
		}

		else if (command == "mp")
		{
			short mp = stoi(message.substr(message.find(" ") + 1));
			set_max_mp(mp);
			set_mp(mp);
		}

		else if (command == "heal")
		{
			set_hp(get_max_hp());
			set_mp(get_max_mp());
		}

		else if (command == "spawn")
		{
			size_t p = message.find(" ");
			if (p == std::string::npos)
			{
				return;
			}
			size_t ap = message.substr(p + 1).find(" ");
			int mob_id = 0;
			int amount = 1;

			if (ap == std::string::npos)
			{
				mob_id = stoi(message.substr(p));
			}
			else
			{
				mob_id = stoi(message.substr(p + 1, ap));
				amount = stoi(message.substr(ap + p + 2));
			}

			if (!MobDataProvider::get_instance()->get_data_by_id(mob_id))
			{
				return;
			}
			if (amount > 100)
			{
				amount = 100;
			}
			if (amount < 1)
			{
				amount = 1;
			}
			for (int i = 0; i < amount; ++i)
			{
				map_->spawn_monster(mob_id, position_x_, position_y_, 0, mob_constants::kSpawnTypesNewSpawn);
			}
		}

		else if (command == "header")
		{
			std::string header_message("");
			if (message.find(" ") != -1)
			{
				header_message = message.substr(message.find(" "));
			}
			world->set_header_message(header_message);

			{
				PacketCreator packet;
				packet.ShowMessage(header_message, 4);
				world->send_packet(&packet);
			}
		}

		else if (command == "notice")
		{
			std::string notice = message.substr(message.find(" ") + 1);
			{
				PacketCreator packet;
				packet.ShowMessage(notice, 0);
				world->send_packet(&packet);
			}
		}

		else if (command == "popup")
		{
			std::string notice = message.substr(message.find(" ") + 1);
			{
				PacketCreator packet;
				packet.ShowMessage(notice, 1);
				world->send_packet(&packet);
			}
		}

		else if (command == "rednotice")
		{
			std::string notice = message.substr(message.find(" ") + 1);
			{
				PacketCreator packet;
				packet.ShowMessage(notice, 5);
				world->send_packet(&packet);
			}
		}

		else if (command == "say")
		{
			std::string notice = message.substr(message.find(" ") + 1);
			{
				PacketCreator packet;
				packet.ShowMessage("[" + get_name() + "] " + notice, 6);
				world->send_packet(&packet);
			}
		}

		else if (command == "hide")
		{
			hide();
		}

		else if (command == "unhide")
		{
			unhide();
		}

		else if (command == "meeting")
		{
			set_map(180000000);
		}
		else if (command == "online")
		{
			auto players = world->get_players();

			for (auto it : *players)
			{
				Player *player = it.second;

				if (player)
				{
					std::string name = player->get_name();

					{
						// packet
						PacketCreator packet;
						packet.ShowMessage(name, 5);
						send_packet(&packet);
					}
				}
			}

			int online_amount = static_cast<int>(players->size());

			{
				// packet
				PacketCreator packet;
				packet.ShowMessage("There are " + std::to_string(online_amount) + " players online.", 5);
				send_packet(&packet);
			}
		}

		else if (command == "shutdown")
		{
			world->ShutdownServer();
		}

		else if (command == "dc")
		{
			Player *target = world->GetPlayerByName(message.substr(message.find(" ") + 1));
			if (target)
			{
				target->disconnect();
			}
		}

		else if (command == "ban")
		{
			std::string player_name = message.substr(message.find(" ") + 1);
			world->ban_account(player_name);

			// disconnect the target player
			Player *target = world->GetPlayerByName(player_name);
			if (target)
			{
				target->disconnect();
			}
		}

		else if (command == "unban")
		{
			std::string player_name = message.substr(message.find(" ") + 1);
			world->unban_account(player_name);
		}

		else
		{
			PacketCreator packet;
			packet.ShowMessage("Enter a VALID command", 5);
			send_packet(&packet);
		}
	}

	else if (message.substr(0, 1) == "@")
	{
		std::string command = message.substr(1, message.find(" ") - 1);

		if (command == "gm")
		{
			unsigned long long ticks = GetTickCount64();

			// "cooldown" 3 minutes

			if (ticks < (last_gm_call_ticks_ + (3 * 60 * 1000)))
			{
				{
					// packet
					PacketCreator packet;
					packet.ShowMessage("Contacting the staff has a time limit of 3 minutes.", 1);
					send_packet(&packet);
				}

				return;
			}

			last_gm_call_ticks_ = ticks;

			std::string text = message.substr(message.find(" ") + 1);

			auto players = world->get_players();

			for (auto &it : *players)
			{
				Player *player = it.second;
				if (player->get_is_gm())
				{
					{
						PacketCreator packet;
						packet.ShowMessage("[HELP CALL] player: " + name_ + " message: " + text, 6);
						player->send_packet(&packet);
					}
				}
			}

			{
				PacketCreator packet;
				packet.ShowMessage("A staff member will get to you as soon as possible.", 0);
				send_packet(&packet);
			}
		}

		else if (command == "fm")
		{
			set_map(910000000);
		}

		else if (command == "check")
		{
			{
				PacketCreator packet;
				packet.ShowMessage("NX Cash: " + std::to_string(nx_cash_credit_), 5);
				send_packet(&packet);
			}
		}

		else if (command == "str")
		{
			short add_str = stoi(message.substr(message.find(" ") + 1));
			if (add_str < 1)
			{
				return;
			}
			if (ap_ >= add_str)
			{
				set_str(str_ + add_str);
				set_ap(ap_ - add_str);
			}
		}

		else if (command == "dex")
		{
			short add_dex = stoi(message.substr(message.find(" ") + 1));
			if (add_dex < 1)
			{
				return;
			}
			if (ap_ >= add_dex)
			{
				set_dex(dex_ + add_dex);
				set_ap(ap_ - add_dex);
			}
		}

		else if (command == "int")
		{
			short add_int = stoi(message.substr(message.find(" ") + 1));
			if (add_int < 1)
			{
				return;
			}
			if (ap_ >= add_int)
			{
				set_int(int_ + add_int);
				set_ap(ap_ - add_int);
			}
		}

		else if (command == "luk")
		{
			short add_luk = stoi(message.substr(message.find(" ") + 1));
			if (add_luk < 1)
			{
				return;
			}
			if (ap_ >= add_luk)
			{
				set_luk(luk_ + add_luk);
				set_ap(ap_ - add_luk);
			}
		}

		else
		{
		PacketCreator packet;
		packet.ShowMessage("Enter a VALID command", 5);
		send_packet(&packet);
		}

	}
	else
	{
		{
			PacketCreator packet;
			packet.ShowChatMessage(this, message, bubble_only);
			map_->send_packet(&packet);
		}
	}
}

void Player::handle_use_group_chat()
{
	std::unordered_map<int, Player *> players;
	World *world = World::get_instance();

	signed char type = read<signed char>();

	switch (type)
	{
	case group_chat_handler_and_packet_type_constants::kBuddy:
	case group_chat_handler_and_packet_type_constants::kParty:
	case group_chat_handler_and_packet_type_constants::kGuild:
		break;
	default:
		return;
	}

	signed char count = read<signed char>();

	for (signed char i = 0; i < count; ++i)
	{
		int player_id = read<int>();
		Player *target_player = world->GetPlayerById(player_id);

		if (target_player)
		{
			players[player_id] = target_player;
		}
	}

	const std::string &message = read<std::string>();

	if (message.empty())
	{
		return;
	}

	if (message.length() > 70)
	{
		return;
	}

	for (auto it : players)
	{
		Player *target_player = it.second;
		{
			PacketCreator packet;
			packet.ShowGroupChat(type, name_, message);
			target_player->send_packet(&packet);
		}
	}
}
