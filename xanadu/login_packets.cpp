//

#include "packetcreator.hpp"

#include "character.hpp"
#include "world.hpp"
#include "tools.hpp"
#include "send_packet_opcodes.hpp"
#include "server_constants.hpp"
#include "game_constants.hpp"
#include "constants.hpp"

void PacketCreator::get_handshake(unsigned char *iv_recv, unsigned char *iv_send)
{
	write<short>(kPacketHandshakeServerLength + static_cast<short>(kGameMinorVersion.length()));
	write<short>(kGameVersion);
	write<std::string>(kGameMinorVersion);
	write<int>(*(int*)iv_recv);
	write<int>(*(int*)iv_send);
	write<signed char>(kGameLocale);
}

void PacketCreator::ConnectToChannel(int player_id)
{
	write<short>(send_headers_login::kSERVER_IP);

	// 6 = "Connection failed due to system error Please try again later." popup and login not happening
	// 12 = popup when used in combination with the next byte
	// 14 or 17 = "You have either selected the wrong gateway, or you have yet to change your personal information." popup and login not happening
	// 15 = "We're still processing your request at this time, so you don't have access to this game for now. ..." popup and login not happening
	// 16 or 21 = "Please verify your account via email in order to play the game." popup and login not happening
	write<signed char>(0);
	// in combination with 12 from the byte before this one:
	// 0 and higher than 3 = "Connection failed due to system error Please try again later." popup and login not happening
	// 1 = "You have entered incorrect an LOGIN ID. Please try again" popup and throws client back to login screen
	// 2 = "You have entered an incorrect form of ID, or your account info hasn't been changed yet. Please try again." popup and throws client back to login screen
	// 3 = "Unverified account will be blocked after 7 days from the date of registration. To verify your account immediately, press OK" popup and throws client back to login screen
	write<signed char>(0);
	// ip adress
	World *world = World::get_instance();
	if (world->is_dedicated_server())
	{
		// the server ip
		write<unsigned char>(0);
		write<signed char>(0);
		write<unsigned char>(0);
		write<unsigned char>(0);
	}
	else
	{
		// 127.0.0.1 is localhost
		write<signed char>(127);
		write<signed char>(0);
		write<signed char>(0);
		write<signed char>(1);
	}

	write<unsigned short>(kServerPort);
	write<int>(player_id);
	write<signed char>(0);
	write<int>(0);
}

/*
values for success_or_failure_reason:
* 0 means login successfull, otherwise: reason for login failure
* 1 means login doesn't happen
* 2: failure reason: "Your account has been blocked... You can login after xx/xx/xxxx xx:xx am/pm"
* 3: failure reason: ID deleted or blocked
* 4: failure reason: Incorrect password
* 5: failure reason: Not a registered id
* 6: failure reason: Connect failed due to system error
* 7: ID already logged in or server under inspection popup
*/
void PacketCreator::LoginRequest(signed char success_or_failure_reason, int user_id, std::string account_name)
{
	write<short>(send_headers_login::kLoginStatus);
	write<signed char>(success_or_failure_reason);
	write<signed char>(0);
	write<int>(0);
	if (success_or_failure_reason != 0)
	{
		return;
	}
	write<int>(user_id);
	write<signed char>(kGenderConstantsMale); // gender byte 0 = male, 1 = female, number 10/0x0A triggers gender select
	write<signed char>(0); // nGradeCode (admin byte: bool, true/1 for GMs/admins, false/0 for normal), for GM commands and maybe other stuff
	/*
	info for the byte below:
	short toWrite = (short) (c.gmLevel() * 32);
	mplew.write(toWrite > 0x80 ? 0x80 : toWrite);
	*/
	write<signed char>(0); // nSubGradeCode (0x80 is admin, 0x20 and 0x40 = subgm), for GM commands and maybe other stuff
	write<signed char>(0); // nCountryID
	write<std::string>(account_name);
	write<signed char>(0); // nPurchaseExp
	write<signed char>(0); // quiet ban/chat block reason
	write<long long>(0); // quiet ban/chat unblock time/date
	write<long long>(0); // register date
	write<int>(8); // nNumOfCharacter
	write<signed char>(1);
	write<signed char>(1); // PIC modes: 0 = Register PIC | 1 = Ask for PIC | 2 = Disable PIC
}

/*
values for success_or_failure_reason:
same as for LoginRequest
*/
void PacketCreator::GetViewAllCharactersLoginError(signed char success_or_failure_reason)
{
	write<short>(send_headers_login::kSELECT_CHARACTER_BY_VAC);
	write<signed char>(success_or_failure_reason);
	write<signed char>(0);
}

void PacketCreator::ShowAllCharacter(int chars, int unk)
{
	write<short>(send_headers_login::kVIEW_ALL_CHAR);
	write<signed char>(1); // mode: 0 = view info, 1 = set chars amount
	write<int>(chars);
	write<int>(unk);
}

void PacketCreator::ShowAllCharacterInfo(int world_id, std::unordered_map<int, Character *> *characters)
{
	write<short>(send_headers_login::kVIEW_ALL_CHAR);
	write<signed char>(0); // mode: 0 = view info, 1 = set chars amount
	write<signed char>(world_id);
	write<unsigned char>(static_cast<unsigned char>(characters->size()));
	for (auto &it : *characters)
	{
		Character *character = it.second;
		ShowCharacter(character, true);
	}
}

/*
* mode values:
* 0 - PIN was accepted
* 1 - Register a new PIN
* 2 - Invalid pin / Reenter
* 3 - Connection failed due to system error
* 4 - Enter the pin
*/
void PacketCreator::LoginProcess()
{
	write<short>(send_headers_login::kPIN_CHECK_OPERATION);
	write<signed char>(0); // mode
}

void PacketCreator::ShowWorld()
{
	World *world = World::get_instance();
	write<short>(send_headers_login::kSERVER_LIST);
	write<signed char>(world->get_id()); // kServerListPacketNoWorlds = no worlds, >= 0 is world id with data
	write<std::string>(world->get_name());
	write<signed char>(kWorld1Flag);
	write<std::string>(kWorld1EventMessage);
	write<short>(100);
	write<short>(100);
	write<signed char>(0);
	// channel data
	signed char channels_count = world->get_channels_count();
	write<signed char>(channels_count);

	for (signed char channel_id = 0; channel_id < channels_count; ++channel_id)
	{
		std::string channel_name = (world->get_name() + "-" + std::to_string(channel_id + 1));
		write<std::string>(channel_name);
		write<int>(0); // channel population
		write<signed char>(world->get_id());
		write<signed char>(channel_id);
		write<signed char>(0);
	}
	// login screen balloons (credits to Eric from RageZone)
	//constexpr short balloon_size = 1;
	write<short>(0); // balloon size
	/*for (int i = 0; i < balloon_size; ++i)
	{
		write<short>(236); // X position
		write<short>(122); // Y position
		write<std::string>("test"); // message
	}*/
}

const constexpr signed char kServerListPacketNoWorlds = -1;

void PacketCreator::EndWorlds()
{
	write<short>(send_headers_login::kSERVER_LIST);
	write<signed char>(kServerListPacketNoWorlds); // kServerListPacketNoWorlds = no worlds, >= 0 is world id with data
}

/*
Possible values for status:
* 0 - Normal
* 1 - Highly populated (a 2-sentence message pops up at world select as a warning of high population)
* 2 - Full (a 2-sentence message pops up at world select and the client does not request channel loading)
*/
void PacketCreator::ShowChannels()
{
	write<short>(send_headers_login::kSERVER_STATUS);
	write<signed char>(0); // status
	write<signed char>(0);
}

void PacketCreator::AddCharStats(Character *character)
{
	write<int>(character->get_id());
	write_string(character->get_name(), 13);
	write<signed char>(character->get_gender());
	write<signed char>(character->get_skin());
	write<int>(character->get_face());
	write<int>(character->get_hair());
	write<long long>(0); // pet unique id 1
	write<long long>(0); // pet unique id 2
	write<long long>(0); // pet unique id 3
	write<signed char>(character->get_level());
	write<short>(character->get_job());
	write<short>(character->get_str());
	write<short>(character->get_dex());
	write<short>(character->get_int());
	write<short>(character->get_luk());
	write<short>(600); // hp
	write<short>(600); // maxhp
	write<short>(300); // mp
	write<short>(300); // maxmp
	write<short>(0); // ap
	write<short>(0); // sp
	write<int>(0); // exp
	write<short>(character->get_fame());
	write<int>(0); // gachapon exp? (nTempEXP_CS)
	write<int>(0); // map id
	write<signed char>(0); // map spawn point
	write<int>(0); // Playtime
}

void PacketCreator::AddCharLook(Character *character, bool megaphone)
{
	write<signed char>(character->get_gender());
	write<signed char>(character->get_skin());
	write<int>(character->get_face());
	write<bool>(megaphone);
	write<int>(character->get_hair());

	std::unordered_map<signed char, int> visible_equips;
	std::unordered_map<signed char, int> invisible_equips;
	int cash_weapon_id = 0;

	auto equips = character->get_equips();
	for (auto it : *equips)
	{
		int item_id = it.first;
		signed char pos = -it.second;

		// cash weapon
		if (pos == 111)
		{
			cash_weapon_id = item_id;

			continue;
		}

		if (pos < 100)
		{
			if (visible_equips.find(pos) == visible_equips.end())
			{
				visible_equips[pos] = item_id;
			}
			else
			{
				invisible_equips[pos] = item_id;
			}
		}
		else
		{
			// cash equips
			pos -= 100;

			if (visible_equips.find(pos) != visible_equips.end())
			{
				invisible_equips[pos] = visible_equips[pos];
			}

			visible_equips[pos] = item_id;
		}
	}

	constexpr const signed char kEndEquipInfo = -1;

	// visible equips info
	for (auto it : visible_equips)
	{
		write<signed char>(it.first);
		write<int>(it.second);
	}
	write<signed char>(kEndEquipInfo);

	// invisible equips info
	for (auto it : invisible_equips)
	{
		write<signed char>(it.first);
		write<int>(it.second);
	}
	write<signed char>(kEndEquipInfo);

	// cash weapon itemid
	write<int>(cash_weapon_id);
	// show pets
	write<int>(0); // pet 1 itemid
	write<int>(0); // pet 2 itemid
	write<int>(0); // pet 3 itemid
}

void PacketCreator::ShowCharacter(Character *character, bool view_all)
{
	AddCharStats(character);
	AddCharLook(character);

	if (!view_all)
	{
		write<signed char>(0);
	}

	// rankings
	bool enable_rankings = false;
	write<bool>(enable_rankings);
	if (enable_rankings)
	{
		write<int>(0); // world rank
		write<int>(0); // world rank move
		write<int>(0); // job rank
		write<int>(0); // job rank move
	}
}

// success or error value:
// 0 = success
// 1 = no login
// 2 or 3 = ID has been deleted or blocked from connection
// 4 = "This is an incorrect password." popup
// 5 = "This is not a registered ID". popup
// 6 = "Connection failed due to system error" popup
// 7 = ID already logged in or server under inspection popup
void PacketCreator::ShowCharacters(std::unordered_map<int, Character *> *characters, int character_slots)
{
	write<short>(send_headers_login::kCHARACTER_LIST);
	write<signed char>(0); // success or error value
	write<unsigned char>(static_cast<unsigned char>(characters->size()));
	for (auto &it : *characters)
	{
		Character *character = it.second;
		ShowCharacter(character, false);
	}
	write<signed char>(1); // PIC modes: 0 = Register PIC | 1 = Ask for PIC | 2 = Disable PIC
	write<int>(character_slots);
}

void PacketCreator::CheckName(std::string name, bool name_used)
{
	write<short>(send_headers_login::kCHECK_CHARACTER_NAME);
	write<std::string>(name);
	write<bool>(name_used);
}

// success or error value:
// 0 = success
// 10 = too many connections, could not process
// 26 = "You cannot create a new character under that account that has requested for a transfer."
void PacketCreator::AddCharacter(Character *character)
{
	write<short>(send_headers_login::kCREATE_NEW_CHARACTER);
	write<signed char>(0); // success or error value
	ShowCharacter(character, false);
}

// success or error value:
// 0 = success
// 6 = connection failed due to system error
// 9 = failed due to unknown reason
// 10 = too many connections, could not process
// 18 = incorrect birthday code was entered
// 22 = cannot delete guild master character
// 24 = wedding character cannot be deleted
// 26 = cannot delete character which is being transfered currently
void PacketCreator::RemoveCharacter(int characterid)
{
	write<short>(send_headers_login::kDELETE_CHARACTER);
	write<int>(characterid);
	write<signed char>(0); // success or error value
}

void PacketCreator::RelogResponse()
{
	write<short>(send_headers_login::kRELOG_RESPONSE);
	write<signed char>(1);
}
