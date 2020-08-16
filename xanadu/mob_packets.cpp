//

#include "packetcreator.hpp"

#include "mob.hpp"
#include "send_packet_opcodes.hpp"
#include "mob_constants.hpp"

void PacketCreator::ControlMob(Mob *mob, signed char spawn_type)
{
	write<short>(send_headers::kSPAWN_MONSTER_CONTROL);
	write<signed char>(1); // aggro?
	write<int>(mob->get_object_id());
	write<signed char>(mob_constants::kControlStatusControlNormal);
	write<int>(mob->get_monster_id());

	// start of mob status
	write_null(15);
	write<unsigned char>(0x88);
	write_null(6);
	// end of mob status

	write<short>(mob->get_position_x());
	write<short>(mob->get_position_y());
	write<signed char>(mob->get_stance());
	write<short>(0); // foothold
	write<short>(mob->get_original_foothold());
	write<signed char>(spawn_type);

	write<signed char>(-1); // monster carnival team
	write<int>(0);
}

void PacketCreator::EndControlMob(int mob_object_id)
{
	write<short>(send_headers::kSPAWN_MONSTER_CONTROL);
	write<signed char>(0);
	write<int>(mob_object_id);
}

void PacketCreator::SpawnMonster(Mob *mob, signed char spawn_type, int from)
{
	write<short>(send_headers::kSPAWN_MONSTER);
	write<int>(mob->get_object_id());
	write<signed char>(mob_constants::kControlStatusControlNormal);
	write<int>(mob->get_monster_id());

	// start of mob status
	write_null(15);
	write<unsigned char>(0x88);
	write_null(6);
	// end of mob status

	write<short>(mob->get_position_x());
	write<short>(mob->get_position_y());
	write<signed char>(mob->get_stance());
	write<short>(0); // foothold
	write<short>(mob->get_original_foothold());
	write<signed char>(spawn_type);

	if (spawn_type == -3 || from >= 0)
	{
		// object id of the summoner mob
		write<int>(from);
	}

	write<signed char>(-1); // monster carnival team
	write<int>(0);
}

void PacketCreator::MoveMob(int mob_object_id, bool use_skill, signed char action, signed char skill_id, signed char skill_level, short option, short start_position_x, short start_position_y, unsigned char *buffer, int buffer_size)
{
	write<short>(send_headers::kMOVE_MONSTER);
	write<int>(mob_object_id);
	write<signed char>(0);
	write<bool>(use_skill);
	write<signed char>(action);
	write<signed char>(skill_id);
	write<signed char>(skill_level);
	write<short>(option);

	// begin of movement data
	
	write<short>(start_position_x);
	write<short>(start_position_y);
	
	// copy the movement data into the packet buffer
	memcpy(buffer_ + length_, buffer, buffer_size);
	length_ += buffer_size;
	
	// end of movement data
}

void PacketCreator::MoveMobResponse(Mob *mob, short move_id, bool use_skill, signed char skill_id, signed char skill_level)
{
	write<short>(send_headers::kMOVE_MONSTER_RESPONSE);
	write<int>(mob->get_object_id());
	write<short>(move_id);
	write<bool>(use_skill);
	write<short>(mob->get_mp());
	write<signed char>(skill_id);
	write<signed char>(skill_level);
}

void PacketCreator::ShowMobHp(int mob_object_id, signed char hp_percent)
{
	write<short>(send_headers::kSHOW_MONSTER_HP);
	write<int>(mob_object_id);
	write<signed char>(hp_percent);
}

void PacketCreator::KillMob(int mob_object_id)
{
	write<short>(send_headers::kKILL_MONSTER);
	write<int>(mob_object_id);
	write<signed char>(1);
}
