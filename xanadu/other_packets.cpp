//

#include "packetcreator.hpp"

#include <string>

#include "buddy.hpp"
#include "effect.hpp"
#include "send_packet_opcodes.hpp"
#include "inventory.hpp"
#include "item.hpp"
#include "map.hpp"
#include "mob.hpp"
#include "mapnpc.hpp"
#include "party.hpp"
#include "party_member.hpp"
#include "attackinfo.hpp"
#include "quest.hpp"
#include "summon.hpp"
#include "skill_data_provider.hpp"
#include "skill_data.hpp"
#include "shop_data.hpp"
#include "shop_item_data.hpp"
#include "item_data.hpp"
#include "hiredmerchant_item.hpp"
#include "item_data_provider.hpp"
#include "channel.hpp"
#include "player.hpp"
#include "key.hpp"
#include "hiredmerchant.hpp"
#include "guild.hpp"
#include "guild_member.hpp"
#include "world.hpp"
#include "character.hpp"
#include "tools.hpp"
#include "buffstat_constants.hpp"
#include "tools.hpp"
#include "constants.hpp"

// info for SHOW_STATUS_INFO:
// it has the following modes:

// 0 = droppickup
// 1 = questrecord
// 3 = increase exp
// 4 = increase fame
// 5 = increase money
// 6 = increase guild points
// 9 = System Message

void PacketCreator::GainItem(int itemid, short amount)
{
	write<short>(send_headers::kSHOW_STATUS_INFO);
	write<signed char>(0); // mode: 0 = drop pickup, 1 = quest message, 3 = increase exp, 4 = increase fame, 5 = increase mesos, there are also much other types
	write<signed char>(0); // mode2 for drop pickup: -1 = can't pickup, 0 = item, 1 = mesos
	write<int>(itemid);
	write<int>(amount);
}

void PacketCreator::GainMesos(int amount)
{
	write<short>(send_headers::kSHOW_STATUS_INFO);
	write<signed char>(0); // mode: 0 = drop pickup, 1 = quest message, 3 = increase exp, 4 = increase fame, 5 = increase mesos, there are also much other types
	write<signed char>(1); // mode2 for drop pickup: -1 = can't pickup, 0 = item, 1 = mesos
	write<signed char>(0);
	write<int>(amount);
	write<short>(0);
}

/*
* mode can be as follows:
* 0xFF = "You can't get anymore items."
* 0xFE = "This item is unavailable for the pick-up."
*/
void PacketCreator::CantGetAnymoreItems()
{
	write<short>(send_headers::kSHOW_STATUS_INFO);
	write<signed char>(0); // mode: 0 = drop pickup, 1 = quest message, 3 = increase exp, 4 = increase fame, 5 = increase mesos, there are also much other types
	write<unsigned char>(0xFF); // mode2 for drop pickup: -1 = can't pickup, 0 = item, 1 = mesos
}

void PacketCreator::UpdateQuestInfo(signed char mode, Quest *quest)
{
	write<short>(send_headers::kSHOW_STATUS_INFO);
	write<signed char>(1); // mode: 0 = drop pickup, 1 = quest message, 3 = increase exp, 4 = increase fame, 5 = increase mesos, there are also much other types
	write<short>(quest->get_id());
	write<signed char>(mode); // 0 = forfeit, 1 = update, 2 = completed

	switch (mode)
	{
	case 0:
		write<signed char>(0);
		break;
	case 1:
		write<std::string>(quest->get_killed_mobs1());
		break;
	case 2:
		write<long long>(quest->get_completion_time());
		break;
	}
}

void PacketCreator::GainExp(int exp, bool in_chat, bool white, int party_bonus)
{
	write<short>(send_headers::kSHOW_STATUS_INFO);
	write<signed char>(3); // mode: 0 = drop pickup, 1 = quest message, 3 = increase exp, 4 = increase fame, 5 = increase mesos, there are also much other types
	write<bool>(white); // white or yellow
	write<int>(exp); // amount of exp
	write<bool>(in_chat); // in chat or on screen
	write<int>(0); // Bonus Event EXP (+value)
	write<signed char>(0);
	write<signed char>(0);
	write<int>(0); // Bonus Wedding EXP (+value)
	if (in_chat)
	{
		write<signed char>(0);
	}
	write<signed char>(0); // 0 = enable Bonus EXP for PARTY (+value), 1 = enable Bonus Event Party EXP (+value) x0
	write<int>(party_bonus); // Bonus EXP for PARTY (+value)
	write<int>(0); // Equip Item Bonus EXP (+value)
	write<int>(0); // Internet Cafe EXP Bonus (+value)
	write<int>(0); // Rainbow Week Bonus EXP (+value)
}

void PacketCreator::FameGainChat(int amount)
{
	write<short>(send_headers::kSHOW_STATUS_INFO);
	write<signed char>(4); // mode: 0 = drop pickup, 1 = quest message, 3 = increase exp, 4 = increase fame, 5 = increase mesos, there are also much other types
	write<int>(amount);
}

void PacketCreator::MesosGainChat(int amount)
{
	write<short>(send_headers::kSHOW_STATUS_INFO);
	write<signed char>(5); // mode: 0 = drop pickup, 1 = quest message, 3 = increase exp, 4 = increase fame, 5 = increase mesos, there are also much other types
	write<int>(amount);
}

void PacketCreator::ShowAvatarMega(Player *player, unsigned char ear, int item_id, std::string message, std::string message2, std::string message3, std::string message4)
{
	write<short>(send_headers::kSHOW_AVATAR_MEGA);
	write<int>(item_id);
	write<std::string>(player->get_name());
	write<std::string>(message);
	write<std::string>(message2);
	write<std::string>(message3);
	write<std::string>(message4);
	write<int>(player->get_channel_id());
	write<signed char>(ear);
	AddCharLook(player, true);
}

void PacketCreator::HandleCloseChalkboard(int player_id)
{
	write<short>(send_headers::kCHALKBOARD);
	write<int>(player_id);
	write<signed char>(0); // mode: 0 = close, 1 = create
}

void PacketCreator::UseChalkBoard(int player_id, const std::string &message)
{
	write<short>(send_headers::kCHALKBOARD);
	write<int>(player_id);
	write<signed char>(1); // mode: 0 = close, 1 = create
	write<std::string>(message);
}

void PacketCreator::UseScroll(int player_id, bool success, bool cursed, bool legendary_spirit)
{
	write<short>(send_headers::kSHOW_SCROLL_EFFECT);
	write<int>(player_id);
	write<signed char>(success ? 1 : 0);
	write<signed char>(cursed ? 1 : 0);
	write<bool>(legendary_spirit);
	write<signed char>(0);
}

void PacketCreator::UseItemEffect(int player_id, int item_id)
{
	write<short>(send_headers::kSHOW_ITEM_EFFECT);
	write<int>(player_id);
	write<int>(item_id);
}

void PacketCreator::ShowChair(int player_id, int item_id)
{
	write<short>(send_headers::kSHOW_CHAIR);
	write<int>(player_id);
	write<int>(item_id);
}

void PacketCreator::CancelChair()
{
	write<short>(send_headers::kCANCEL_CHAIR);
	write<signed char>(0);
}

void PacketCreator::HiredMerchantBox()
{
	write<short>(send_headers::kREQUEST_HIRED_MERCHANT);
	write<signed char>(7); // action
}

void PacketCreator::ShowPlayer(Player *player)
{
	write<short>(send_headers::kSPAWN_PLAYER);
	write<int>(player->get_id());
	write<unsigned char>(player->get_level());
	write<std::string>(player->get_name());

	// guild info

	Guild *guild = player->get_guild();

	if (guild)
	{
		write<std::string>(guild->get_name());
		write<short>(guild->get_logo_background());
		write<signed char>(guild->get_logo_background_color());
		write<short>(guild->get_logo());
		write<signed char>(guild->get_logo_color());
	}
	else
	{
		write<std::string>("");
		write<short>(0);
		write<signed char>(0);
		write<short>(0);
		write<signed char>(0);
	}

	// end of guild info

	// buff info

	long long buff_mask_pos_1 = 0;
	long long buff_mask_pos_2 = 0;
	signed char buff_value = 0;

	if (player->is_buff_stat_active(buffstat_constants::kCombo))
	{
		buff_mask_pos_2 |= buffstat_constants::kCombo;
		buff_value = player->get_crusader_combo_value();
	}

	if (player->is_buff_stat_active(buffstat_constants::kShadowPartner))
	{
		int skill_id = 4111002;
		// 4211008
		// 14111000
		buff_mask_pos_2 |= buffstat_constants::kShadowPartner;
	}

	if (player->is_buff_stat_active(buffstat_constants::kDarksight))
	{
		buff_mask_pos_2 |= buffstat_constants::kDarksight;
	}

	// are these 8 bytes one long long buffstat instead of 4, 2, 1, 1?
	// probably, but maybe there is nothing in v0.83 that uses it
	// though, it seems to have a standard value (not null) that is always sent, even if no buff stats are active

	write<int>(0);
	write<short>(0);
	write<unsigned char>(0xFC);
	write<signed char>(1);

	//write<long long>(buff_mask_pos_1);

	write<long long>(buff_mask_pos_2);

	// some are 1 bytes, some 2 and some 4
	// not sure atm if multiple or all are sent
	// or masked then

	if (buff_value != 0)
	{
		write<signed char>(buff_value);

		// morph has another byte?
	}

	// TwoStateTemporaryStat area

	// this is not done yet and experimental
	// TO-DO implement all or most buffs below

	write<signed char>(0); // nDefenseAtt
	write<signed char>(0); // nDefenseState

						   // 1. (index 0) is energy charge
	write<int>(0); // n option/value
	write<int>(0); // r option/reason
	write<signed char>(120); // time flag
	write<int>(565666546); // time duration
	write<short>(0); // dynamic

					 // 2. (index 1) is dash speed
	write<int>(0); // n option/value
	write<int>(0); // r option/reason
	write<signed char>(120); // time flag
	write<int>(565666546); // time duration
	write<short>(0); // dynamic

					 // 3. (index 2) is dash jump
	write<int>(0); // n option/value
	write<int>(0); // r option/reason
	write<signed char>(120); // time flag
	write<int>(565666546); // time duration
	write<short>(0); // dynamic

					 // 4. (index 3) is ridevehicle = mount info
	if (player->get_mount_item_id() != 0)
	{
		write<int>(player->get_mount_item_id());
		write<int>(player->get_mount_skill_id());
	}
	else
	{
		write<int>(0);
		write<int>(0);
	}
	write<signed char>(120); // time flag
	write<int>(565666546); // time duration

						   // 5. (index 4) is PartyBooster (Speed Infusion for Buccaneers)
	write<int>(0); // n option/value
	write<int>(0); // r option/reason
	write<signed char>(120); // time flag (This one is tLastUpdated)
	write<int>(565666546); // time duration
	write<signed char>(120); // time flag (This one is tCurrentTime)
	write<int>(565666546); // time duration
	write<short>(0); // usExpireTerm

					 // 6. (index 5) is GuidedBullet (Gaviota for Corsairs)
	write<int>(0); // n option/value
	write<int>(0); // r option/reason
	write<signed char>(120); // time flag
	write<int>(565666546); // time duration
	write<int>(0); // mob id

				   // 7. (index 6) is Undead (MobSkill 133, undead (aka zombify) debuff?)
	write<int>(0); // n option/value
	write<int>(0); // r option/reason
	write<signed char>(120); // time flag
	write<int>(565666546); // time duration
	write<short>(0); // dynamic

	// end of TwoStateTemporaryStat area

	// end of buff info

	// player information, mainly looks

	write<short>(player->get_job());
	AddCharLook(player);
	write<int>(player->get_item_amount(5110000)); // hearts
	write<int>(player->get_item_effect());
	write<int>(player->get_chair());
	write<short>(player->get_position_x());
	write<short>(player->get_position_y());
	write<signed char>(player->get_stance());
	write<short>(0); // foothold
	write<signed char>(0);

	// pet info

	enum
	{
		kEndPetInfo = 0,
		kStartPetInfo = 1
	};

	auto pets = player->get_pets();
	for (auto &pet : *pets)
	{
		write<signed char>(kStartPetInfo);
		write<int>(pet->get_item_id());
		write<std::string>(pet->get_name());
		write<long long>(pet->get_unique_id());
		write<short>(pet->get_position_x());
		write<short>(pet->get_position_y());
		write<signed char>(pet->get_stance());
		write<short>(0); // foothold
		write<signed char>(0); // name tag?
		write<signed char>(0); // chat balloon?
	}

	write<signed char>(kEndPetInfo);

	// mount info

	write<int>(1); // level
	write<int>(0); // exp
	write<int>(0); // tiredness/fatigue

	// playershop/minigame info

	bool has_minigame = false;

	write<bool>(has_minigame); // bool or miniroomtype?

	if (has_minigame)
	{
		write<int>(0); // map object id
		write<std::string>("hello"); // description text
		write<signed char>(0); // specific if game if private?
		write<signed char>(10); // type
		write<signed char>(1); // amount of players that are already inside the minigame
		write<signed char>(2); // max players inside the minigame
		write<signed char>(0); // determines wether joinable or not? 1/0? unsure
	}

	// end of playershop/minigame info

	// chalkboard info

	std::string chalkboard_text = player->get_chalk_board();
	bool has_chalkboard = (chalkboard_text != "");

	write<bool>(has_chalkboard);

	if (has_chalkboard)
	{
		write<std::string>(chalkboard_text);
	}

	// end of chalkboard info

	// rings info

	bool has_couple_ring = false; // to-do if check it has it actually

	write<bool>(has_couple_ring); // couple ring

	if (has_couple_ring)
	{
		// to-do write couple ring data
		// structure:
		// 8 bytes long long - ring id
		// 8 bytes long long - partner ring id
		// 4 bytes int - ring item id
	}

	bool has_friendship_ring = false; // to-do if check it has it actually

	write<bool>(has_friendship_ring); // friendship ring

	if (has_friendship_ring)
	{
		// to-do write friendship ring data
		// structure:
		// 8 bytes long long - ring id
		// 8 bytes long long - partner ring id
		// 4 bytes int - ring item id
	}

	bool has_marriage_ring = false; // to-do if check it has it actually

	write<bool>(has_marriage_ring); // marriage ring

	if (has_marriage_ring)
	{
		// to-do write marriage ring data
		// structure:
		// 4 bytes int - character id
		// 4 bytes int - partner character id
		// 4 bytes int - ring (item?) id
	}

	// end of rings info

	bool has_new_year_info = false;
	write<bool>(has_new_year_info);

	if (has_new_year_info)
	{
		// to-do write new year info data
		// structure:
		// 4 bytes int - amount/size
		// loop based on amount/size determined in the 4 bytes before, with the following data:
		// 4 bytes int - SN 
	}
	
	write<signed char>(0); // berserk skill darkforce effect - 1 or 0 probably (skillid: 1320006, job: dark knight - which is an explorer and warrior)
	write<signed char>(0);
	write<signed char>(0); // carnival party quest team for some fields maybe?
}

void PacketCreator::RemovePlayer(Player *player)
{
	write<short>(send_headers::kREMOVE_PLAYER_FROM_MAP);
	write<int>(player->get_id());
}

/*

needs to be tested

* type - (0:Light&Long 1:Heavy&Short)
* delay - seconds

void PacketCreator::tremble_map_effect(signed char type, int delay)
{
	write<short>(send_headers::kMAP_EFFECT);
	write<signed char>(1);
	write<signed char>(type);
	write<int>(delay);
}
*/

// this packet shows effects in the map
// mode that specifies the type of effect: 1 = tremble effect?, 2 = object, 3 = effect, 4 = sound, 5 = boss hp, 6 = map music
// except for 5 (boss hp), they follow the same structure
// name specifies the effect to be shown

void PacketCreator::MapEffect(signed char mode, std::string name)
{
	write<short>(send_headers::kMAP_EFFECT);
	write<signed char>(mode);
	write<std::string>(name);
}

void PacketCreator::ShowBossHp(int mob_id, int hp, int max_hp, signed char color, signed char background_color)
{
	write<short>(send_headers::kMAP_EFFECT);
	write<signed char>(5); // mode that specifies the type of effect: 2 = object, 3 = effect, 4 = sound, 5 = boss hp, 6 = map music
	write<int>(mob_id);
	write<int>(hp);
	write<int>(max_hp);
	write<signed char>(color);
	write<signed char>(background_color);
}

void PacketCreator::ShowTimer(int seconds)
{
	write<short>(send_headers::kCLOCK);
	write<signed char>(2);
	write<int>(seconds);
}

void PacketCreator::PlayerAttack(signed char attack_type, PlayerAttackInfo &attack)
{
	switch (attack_type)
	{
	case attack_type_constants::kCloseRange:
		write<short>(send_headers::kCLOSE_RANGE_ATTACK);
		break;
	case attack_type_constants::kRanged:
		write<short>(send_headers::kRANGED_ATTACK);
		break;
	case attack_type_constants::kMagic:
		write<short>(send_headers::kMAGIC_ATTACK);
		break;
	case attack_type_constants::kEnergy:
		write<short>(send_headers::kENERGY_ATTACK);
		break;
	}

	write<int>(attack.player_id_);
	write<signed char>(attack.info_byte_);
	write<signed char>(0x5B); // player level
	write<signed char>(attack.skill_level_);

	if (attack.skill_id_ > 0)
	{
		write<int>(attack.skill_id_);
	}

	write<signed char>(0); // display
	write<signed char>(attack.direction_);
	write<signed char>(attack.stance_);
	write<signed char>(attack.weapon_speed_);
	write<signed char>(10); // mastery
	write<int>(attack.item_id_);

	for (auto &it : attack.damages_)
	{
		int mob_object_id = it.first;

		write<int>(mob_object_id);
		write<unsigned char>(0xFF);

		auto &damage = attack.damages_[mob_object_id];

		for (auto &damage : damage)
		{
			write<int>(damage);
		}
	}

	if (attack_type == attack_type_constants::kMagic)
	{
		// mage Big Bang skills
		if (attack.skill_id_ == 2121001 ||
			attack.skill_id_ == 2221001 ||
			attack.skill_id_ == 2321001)
		{
			write<int>(attack.charge_);
		}
	}

	if (attack_type == attack_type_constants::kRanged)
	{
		write<short>(0); // x position - of item used?
		write<short>(0); // y position - of item used?
	}
}

void PacketCreator::UpdateQuest(Quest *quest, int npc_id)
{
	write<short>(send_headers::kUPDATE_QUEST_INFO);
	write<signed char>(10);
	write<short>(quest->get_id());
	write<int>(npc_id);
	write<short>(0);
	write<signed char>(0);
}

void PacketCreator::AddCharStats(Player *player)
{
	write<int>(player->get_id());
	write_string(player->get_name(), 13);
	write<signed char>(player->get_gender());
	write<signed char>(player->get_skin_color());
	write<int>(player->get_face());
	write<int>(player->get_hair());
	write<long long>(0); // pet unique id 1
	write<long long>(0); // pet unique id 2
	write<long long>(0); // pet unique id 3
	write<signed char>(player->get_level());
	write<short>(player->get_job());
	write<short>(player->get_str());
	write<short>(player->get_dex());
	write<short>(player->get_int());
	write<short>(player->get_luk());
	write<short>(player->get_hp());
	write<short>(player->get_max_hp());
	write<short>(player->get_mp());
	write<short>(player->get_max_mp());
	write<short>(player->get_ap());
	write<short>(player->get_sp());
	write<int>(player->get_exp());
	write<short>(player->get_fame());
	write<int>(0); // gachapon exp? (_nTempEXP_CS)
	write<int>(player->get_map()->get_id());
	write<signed char>(player->get_spawn_point());
	write<int>(0); // Playtime
}

void PacketCreator::writeCharacterData(Player *player)
{
	write<long long>(-1);
	write<signed char>(0);
	AddCharStats(player);
	write<signed char>(player->get_buddy_list_capacity());

	bool has_linked_name = false;
	write<bool>(has_linked_name);

	if (has_linked_name)
	{
		write<std::string>(""); // linked name
	}

	AddInventoryInfo(player);
	AddSkillInfo(player);
	AddCoolDownInfo(player);
	AddQuestInfo(player);

	// minigame data?
	// not sure if this one belongs to rings, and it might be size for something
	// extract from client:
	/*
	v64 = (unsigned __int16)CInPacket::Decode2(a2);
	if ( (signed int)(unsigned __int16)v64 > 0 )
	{
	v65 = v64;
	do
	{
	v134 = 0;
	LOBYTE(v142) = 14;
	sub_4B8B70(&v133);
	v66 = v134;
	*v66 = CInPacket::Decode4(a2);
	v67 = (int)v134;
	*(_DWORD *)(v67 + 4) = CInPacket::Decode4(a2);
	v68 = (int)v134;
	*(_DWORD *)(v68 + 8) = CInPacket::Decode4(a2);
	v69 = (int)v134;
	*(_DWORD *)(v69 + 12) = CInPacket::Decode4(a2);
	v70 = (int)v134;
	*(_DWORD *)(v70 + 16) = CInPacket::Decode4(a2);
	sub_4B93C8(v134, &v133);
	LOBYTE(v142) = 1;
	if ( v134 )
	{
	sub_4B8BD2(0);
	v134 = 0;
	}
	--v65;
	}
	while ( v65 );
	}
	*/
	write<short>(0); // amount?

	AddRingInfo();
	AddTeleportRockInfo();

	// monster book info?
	write<int>(0); // monster book cover?
	write<signed char>(0);
	write<short>(0); // cards size? loop follows if not 0

	// new year info
	write<short>(0); // size

	// area info?
	write<short>(0);

	write<short>(0);
}

void PacketCreator::AddInventoryInfo(Player *player)
{
	write<int>(player->get_mesos());
	write<signed char>(player->get_equip_slots());
	write<signed char>(player->get_use_slots());
	write<signed char>(player->get_setup_slots());
	write<signed char>(player->get_etc_slots());
	write<signed char>(player->get_cash_slots());
	write<long long>(kZeroTime);

	enum Constants
	{
		kEndInventory
	};

	// equipped Inventory (no cash)
	auto items = player->get_inventory(kInventoryConstantsTypesEquipped)->get_items();

	for (auto &it : *items)
	{
		std::shared_ptr<Item> item = it.second;

		if (item->get_slot() > -100)
		{
			ItemInfo(item.get());
		}
	}

	write<short>(kEndInventory);

	// equipped inventory (only cash)
	for (auto &it : *items)
	{
		std::shared_ptr<Item> item = it.second;

		if (item->get_slot() <= -100)
		{
			ItemInfo(item.get());
		}
	}

	write<short>(kEndInventory);

	// equip inventory
	items = player->get_inventory(kInventoryConstantsTypesEquip)->get_items();

	for (auto &it : *items)
	{
		ItemInfo(it.second.get());
	}

	write<int>(kEndInventory);

	// use inventory
	items = player->get_inventory(kInventoryConstantsTypesUse)->get_items();
	for (auto &it : *items)
	{
		ItemInfo(it.second.get());
	}

	write<signed char>(kEndInventory);

	// setup inventory
	items = player->get_inventory(kInventoryConstantsTypesSetup)->get_items();
	for (auto &it : *items)
	{
		ItemInfo(it.second.get());
	}

	write<signed char>(kEndInventory);

	// etc inventory
	items = player->get_inventory(kInventoryConstantsTypesEtc)->get_items();
	for (auto &it : *items)
	{
		ItemInfo(it.second.get());
	}

	write<signed char>(kEndInventory);

	// cash inventory
	items = player->get_inventory(kInventoryConstantsTypesCash)->get_items();
	for (auto &it : *items)
	{
		ItemInfo(it.second.get());
	}

	write<signed char>(kEndInventory);
}

void PacketCreator::AddSkillInfo(Player *player)
{
	auto skills = player->get_skills();

	write<short>(static_cast<short>(skills->size()));

	for (auto it : *skills)
	{
		int skill_id = it.first;
		Skill &skill = it.second;

		write<int>(skill_id);
		write<int>(skill.level_);
		write<long long>(kNoExpirationTime);

		if (tools::is_fourth_job_skill(skill_id))
		{
			write<int>(skill.master_level_);
		}
	}
}

void PacketCreator::AddCoolDownInfo(Player *player)
{
	write<short>(0); // size

	// (followed 1. by skillid (int)
	// and 2. time (short))
}

void PacketCreator::AddQuestInfo(Player *player)
{
	// quests in progress data

	auto quests_in_progress = player->get_quests_in_progress();

	write<short>(static_cast<short>(quests_in_progress->size()));

	for (auto &it : *quests_in_progress)
	{
		Quest *quest = it.second.get();
		write<short>(quest->get_id());
		write<std::string>(quest->get_killed_mobs1());
	}

	// completed quests data

	auto completed_quests = player->get_completed_quests();

	write<short>(static_cast<short>(completed_quests->size()));

	for (auto &it : *completed_quests)
	{
		Quest *quest = it.second.get();
		write<short>(quest->get_id());
		write<long long>(quest->get_completion_time());
	}
}

void PacketCreator::AddRingInfo()
{
	// crush rings
	write<short>(0); // amount

	// to-do crush rings info

	// friendship rings
	write<short>(0); // amount

	// to-do friendship rings info

	// marriage rings
	write<short>(0); // amount

	// to-do marriage rings info
}

void PacketCreator::AddTeleportRockInfo()
{
	int loop_counter = 0;
	// regular rock maps
	for (; loop_counter < 5; ++loop_counter)
	{
		write<int>(kNoMap);
	}
	// vip rock maps
	for (loop_counter = 0; loop_counter < 10; ++loop_counter)
	{
		write<int>(kNoMap);
	}
}

void PacketCreator::change_map(Player *player, bool is_connect_packet)
{
	write<short>(send_headers::kWARP_TO_MAP);
	write<int>(player->get_channel_id());
	write<signed char>(1);
	write<bool>(is_connect_packet);

	// messages on the screen that disappears after some seconds
	// maybe used in combination with quiet ban? (so upon login player is informed about it)
	// structure:
	// size: 2 bytes/short int
	// if more than 0:
	// write string title
	// and for each message line: write string message
	write<short>(0); // 0 = nothing, more than 0 = amount of message lines

	if (is_connect_packet)
	{
		writeRngSeeds();
		writeCharacterData(player);
	}
	else
	{
		write<signed char>(0);
		write<int>(player->get_map()->get_id());
		write<signed char>(player->get_spawn_point());
		write<short>(player->get_hp());
		write<signed char>(0);
	}

	write<long long>(tools::time_to_tick());
}

// random number generator seeds
void PacketCreator::writeRngSeeds()
{
	constexpr const int seed1 = 36556356;
	constexpr const int seed2 = 233868;
	constexpr const int seed3 = 98358998;
	write<int>(seed1);
	write<int>(seed2);
	write<int>(seed3);
}

void PacketCreator::AddCharLook(Player *player, bool megaphone)
{
	write<signed char>(player->get_gender());
	write<signed char>(player->get_skin_color());
	write<int>(player->get_face());
	write<bool>(megaphone);
	write<int>(player->get_hair());

	std::unordered_map<signed char, int> visible_equips;
	std::unordered_map<signed char, int> invisible_equips;
	int cash_weapon_id = 0;
	auto equips = player->get_inventory(kInventoryConstantsTypesEquipped)->get_items();

	for (auto it : *equips)
	{
		std::shared_ptr<Item> item = it.second;
		int item_id = item->get_item_id();
		signed char pos = -item->get_slot();

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

	enum
	{
		EndEquipInfo = -1
	};

	// visible equips info

	for (auto &it : visible_equips)
	{
		write<signed char>(it.first);
		write<int>(it.second);
	}

	write<signed char>(EndEquipInfo);

	// invisible equips info

	for (auto &it : invisible_equips)
	{
		write<signed char>(it.first);
		write<int>(it.second);
	}

	write<signed char>(EndEquipInfo);

	// cash weapon itemid
	write<int>(cash_weapon_id);

	{
		// show pets

		auto pets = *player->get_pets();
		write<int>(pets.size() > 0 ? pets[0]->get_item_id() : 0);
		write<int>(pets.size() > 1 ? pets[1]->get_item_id() : 0);
		write<int>(pets.size() > 2 ? pets[2]->get_item_id() : 0);
	}
}

void PacketCreator::UpdatePlayer(Player *player)
{
	write<short>(send_headers::kUPDATE_CHAR_LOOK);
	write<int>(player->get_id());
	write<signed char>(1);
	AddCharLook(player);

	// rings info

	bool has_couple_ring = false;

	write<bool>(has_couple_ring); // couple ring

	if (has_couple_ring)
	{
		// to-do
	}

	bool has_friendship_ring = false;

	write<bool>(has_friendship_ring); // friendship ring

	if (has_friendship_ring)
	{
		// to-do
	}

	bool has_marriage_ring = false;

	write<bool>(has_marriage_ring); // marriage ring

	if (has_marriage_ring)
	{
		// to-do
	}

	// end of rings info

	write<int>(player->get_chair());
}

void PacketCreator::ShowKeymap(Player *player)
{
	write<short>(send_headers::kKEYMAP);
	write<signed char>(0);

	for (int pos = kMinKeymapPos; pos < kMaxKeymapPos; ++pos)
	{
		Key &key = player->get_key(pos);
		write<signed char>(key.type);
		write<int>(key.action);
	}
}

void PacketCreator::ShowPlayerMovement(int player_id, short start_position_x, short start_position_y, unsigned char *buffer, int buffer_size)
{
	write<short>(send_headers::kMOVE_PLAYER);
	write<int>(player_id);

	// begin of movement data

	write<short>(start_position_x);
	write<short>(start_position_y);

	// copy the movement data into the packet buffer
	memcpy(buffer_ + length_, buffer, buffer_size);
	length_ += buffer_size;

	// end of movement data
}

void PacketCreator::FaceExpression(int player_id, int face)
{
	write<short>(send_headers::kFACIAL_EXPRESSION);
	write<int>(player_id);
	write<int>(face);
}

void PacketCreator::ShowInfo(Player *player)
{
	write<short>(send_headers::kCHAR_INFO);
	write<int>(player->get_id());
	write<signed char>(player->get_level());
	write<short>(player->get_job());
	write<short>(player->get_fame());
	write<bool>(player->is_married());

	// guild info

	Guild *guild = player->get_guild();

	if (guild)
	{
		write<std::string>(guild->get_name());
	}
	else
	{
		write<std::string>("-");
	}

	write<std::string>(""); // guild alliance name

	// end of guild info

	write<signed char>(0);

	// pets info

	enum Constants
	{
		EndPetInfo = 0,
		StartPetInfo = 1
	};

	auto pets = player->get_pets();
	signed char index = 0;

	for (auto pet : *pets)
	{
		write<signed char>(StartPetInfo);

		write<int>(pet->get_item_id());
		write<std::string>(pet->get_name());
		write<signed char>(pet->get_pet_level());
		write<short>(pet->get_pet_closeness());
		write<signed char>(pet->get_pet_fullness());
		write<short>(0);

		Inventory *inventory = player->get_inventory(kInventoryConstantsTypesEquipped);
		if (!inventory)
		{
			return;
		}

		signed char slot = (index == 0 ? -114 : (index == 1 ? -130 : -138));
		auto item = inventory->get_item_by_slot(slot);

		if (item)
		{
			write<int>(item->get_item_id());
		}
		else
		{
			write<int>(0);
		}

		++index;
	}

	write<signed char>(EndPetInfo);

	// end of pets info

	// mount info
	bool has_tamed_mob = (player->get_mount_item_id() != 0);
	write<bool>(has_tamed_mob);
	if (has_tamed_mob)
	{
		write<int>(1); // level
		write<int>(0); // exp
		write<int>(0); // tiredness
	}
	// end of mount info

	// wishlist info
	write<signed char>(0); // size

	// to-do write wishlist data here

	// end of wishlist info

	// monster book info
	write<int>(1); // book level?
	write<int>(0); // normal card?
	write<int>(0); // special card?
	write<int>(0); // total cards?
	write<int>(0); // monster book cover?
	// end of monster book info

	// equipped medal info
	Inventory *inventory = player->get_inventory(kInventoryConstantsTypesEquipped);
	if (!inventory)
	{
		write<int>(0);
	}
	else
	{
		auto medal = inventory->get_item_by_slot(kItemConstantsEquippedSlotsMedal);
		write<int>(medal ? medal->get_item_id() : 0);
	}

	// collected medals info
	write<short>(0); // size
	// to-do write collected medals info here

}

void PacketCreator::SendFame(std::string name, signed char type)
{
	write<short>(send_headers::kFAME);
	write<signed char>(5);
	write<std::string>(name);
	write<signed char>(type);
}

void PacketCreator::SendFamee(std::string name2, signed char type, int newFame)
{
	write<short>(send_headers::kFAME);
	write<signed char>(0);
	write<std::string>(name2);
	write<signed char>(type);
	write<int>(newFame);
}

void PacketCreator::ItemGainChat(int itemid, int amount, signed char items_size)
{
	write<short>(send_headers::kSHOW_ITEM_GAIN_INCHAT);
	write<signed char>(3); // actions: 3 = gain item, 4 = pet level up, there are also much other types
	write<signed char>(items_size);

	for (signed char i = 0; i < items_size; ++i)
	{
		write<int>(itemid);
		write<int>(amount);
	}
}

void PacketCreator::ShowOwnPetLevelUp(signed char pet_slot)
{
	write<short>(send_headers::kSHOW_ITEM_GAIN_INCHAT);
	write<signed char>(4); // actions: 3 = gain item, 4 = pet level up, there are also much other types
	write<signed char>(0);
	write<signed char>(pet_slot);
}

void PacketCreator::ShowForeignEffect(int player_id, signed char effect)
{
	write<short>(send_headers::kSHOW_FOREIGN_EFFECT);
	write<int>(player_id);
	write<signed char>(effect); // 0 = level up, 4 = pet level up (has custom structure), 8 = job change, others are (partially) buffs/skills?
}

void PacketCreator::ShowPetLevelUp(int owner_player_id, signed char pet_slot)
{
	write<short>(send_headers::kSHOW_FOREIGN_EFFECT);
	write<int>(owner_player_id);
	write<signed char>(4); // 0 = level up, 4 = pet level up (has custom structure), 8 = job change, others are (partially) buffs/skills?
	write<signed char>(0);
	write<signed char>(pet_slot);
}

void PacketCreator::ShowBuffEffect(int player_id, signed char effect_id, int skill_id, signed char skill_level)
{
	if (player_id > 0)
	{
		write<short>(send_headers::kSHOW_FOREIGN_EFFECT);
		write<int>(player_id);
	}
	else
	{
		write<short>(send_headers::kSHOW_ITEM_GAIN_INCHAT);
	}

	// actions for kSHOW_ITEM_GAIN_INCHAT: 3 = gain item, 4 = pet level up, there are also much other types
	write<signed char>(effect_id); // actions for kSHOW_FOREIGN_EFFECT: 0 = level up, 8 = job change, others are (partially) buffs/skills?
	write<int>(skill_id);
	write<signed char>(0); // direction?
	write<signed char>(skill_level);
}

void PacketCreator::SendHammerData(int hammer_used)
{
	write<short>(send_headers::kVICIOUS_HAMMER);
	write<signed char>(0x39);
	write<int>(0);
	write<int>(hammer_used);
}

void PacketCreator::SendHammerMessage()
{
	write<short>(send_headers::kVICIOUS_HAMMER);
	write<signed char>(0x3D);
	write<int>(0);
}
