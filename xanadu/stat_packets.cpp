//

#include "packetcreator.hpp"
#include "send_packet_opcodes.hpp"
#include "constants.hpp"
#include "player.hpp"

void PacketCreator::EnableAction(bool unstuck)
{
	write<short>(send_headers::kUPDATE_STATS);
	write<bool>(unstuck); // 1 / true = fix client lock and update client time/tick
	write<int>(kCharacterStatsNone);
}

void PacketCreator::UpdateLevel(unsigned char level)
{
	write<short>(send_headers::kUPDATE_STATS);
	write<bool>(true); // 1 / true = fix client lock and update client time/tick
	write<int>(kCharacterStatsLevel);

	write<unsigned char>(level);
}

void PacketCreator::UpdateSp(Player * player, short value)
{
	write<short>(send_headers::kUPDATE_STATS);
	write<bool>(true); // 1 / true = fix client lock and update client time/tick
	write<int>(kCharacterStatsSp);

	write<short>(value); // remaining sp
}

void PacketCreator::UpdateApStats(short str, short dex, short intt, short luk, short ap)
{
	write<short>(send_headers::kUPDATE_STATS);
	write<bool>(true); // 1 / true = fix client lock and update client time/tick
	write<int>(kCharacterStatsStr | kCharacterStatsDex | kCharacterStatsInt | kCharacterStatsLuk | kCharacterStatsAp); // stat: update mask

	// values
	write<short>(str);
	write<short>(dex);
	write<short>(intt);
	write<short>(luk);
	write<short>(ap);
}

// skin, job, str, dex, int, luk, ap, sp (has own packet)
void PacketCreator::UpdateStatShort(int stat, short value)
{
	write<short>(send_headers::kUPDATE_STATS);
	write<bool>(true); // 1 / true = fix client lock and update client time/tick
	write<int>(stat);

	write<short>(value);
}

// face, hair, hp, maxhp, mp, maxmp, fame, exp, mesos
void PacketCreator::UpdateStatInt(int stat, int value)
{
	write<short>(send_headers::kUPDATE_STATS);
	write<bool>(true); // 1 / true = fix client lock and update client time/tick
	write<int>(stat);

	write<int>(value);
}

void PacketCreator::PetStatUpdate(Player *player)
{
	write<short>(send_headers::kUPDATE_STATS);
	write<bool>(true); // 1 / true = fix client lock and update client time/tick
	write<int>(kCharacterStatsPet);

	auto pets = *player->get_pets();
	write<long long>(pets.size() > 0 ? pets[0]->get_unique_id() : 0);
	write<long long>(pets.size() > 1 ? pets[1]->get_unique_id() : 0);
	write<long long>(pets.size() > 2 ? pets[2]->get_unique_id() : 0);
	write<signed char>(0);
}
