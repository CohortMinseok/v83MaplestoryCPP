//

#include "packetcreator.hpp"
#include "send_packet_opcodes.hpp"

void PacketCreator::StartCarnivalPartyQuest(unsigned char team)
{
	write<short>(send_headers::kMONSTER_CARNIVAL_START);
	write<unsigned char>(team);
	write<short>(0); // Obtained CP - Used CP
	write<short>(0); // Total Obtained CP
	write<short>(0); // Obtained CP - Used CP of the team
	write<short>(0); // Total Obtained CP of the team
	write<short>(0); // Obtained CP - Used CP of the enemy team
	write<short>(0); // Total Obtained CP of the enemy team
	
	// write one byte for each cpq mob?
	for (int i = 0; i < 10; ++i)
	{
		write<signed char>(0);
	}
}

void PacketCreator::obtain_cp(short cp, short total_cp)
{
	write<short>(send_headers::kMONSTER_CARNIVAL_OBTAINED_CP);
	write<short>(cp); // Obtained CP - Used CP
	write<short>(total_cp); // Total Obtained CP
}

void PacketCreator::obtain_party_cp(unsigned char team, short cp, short total_cp)
{
	write<short>(send_headers::kMONSTER_CARNIVAL_PARTY_CP);
	write<unsigned char>(team); // Team where the points are given to.
	write<short>(cp); // Obtained CP - Used CP
	write<short>(total_cp); // Total Obtained CP
}

void PacketCreator::carnival_pq_message(signed char message)
{
	write<short>(send_headers::kMONSTER_CARNIVAL_MESSAGE);
	write<signed char>(message);
}

void PacketCreator::carnival_pq_summon(signed char tab, signed char summon_number, std::string player_name)
{
	write<short>(send_headers::kMONSTER_CARNIVAL_SUMMON);
	write<signed char>(tab);
	write<signed char>(summon_number); // each mob in cpq has a specific summon number
	write<std::string>(player_name);
}

void PacketCreator::carnival_pq_died(signed char lost_cp, unsigned char team, std::string player_name)
{
	write<short>(send_headers::kMONSTER_CARNIVAL_DIED);
	write<unsigned char>(team);
	write<std::string>(player_name);
	write<signed char>(lost_cp);
}

// player_is_leader != 6: [teamname] of Team [playername] has quit the Monster Carnival.
// player_is_leader == 6: Since the leader of the Team [teamname] quit the Monster Carnival, [playername] has been appointed as the new leader of the team.

void PacketCreator::leave_carnival_pq(bool player_is_leader, unsigned char team, std::string player_name)
{
	write<short>(send_headers::kMONSTER_CARNIVAL_LEAVE);
	write<signed char>(player_is_leader ? 6 : 0);
	write<unsigned char>(team);
	write<std::string>(player_name);
}

// cpq_show_game_result
// 8: You have won the Monster Carnival. Please wait as you'll be transported out of here shortly.
// 9: Unfortunately, you have lost the Monster Carnival. Please wait as you'll be transported out of here shortly.
// 10: Despite the Overtime, the carnival ended in a draw. Please wait as you'll be transported out of here shortly.
// 11: Monster Carnival has ended abruptly due to the opposing team leaving the game too early. Please wait as you'll be transported out of here shortly.

void PacketCreator::cpq_show_game_result(signed char result)
{
	write<short>(send_headers::kMONSTER_CARNIVAL_SHOW_GAME_RESULT);
	write<signed char>(result);
}
