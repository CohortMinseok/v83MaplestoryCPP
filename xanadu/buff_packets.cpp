//

#include "packetcreator.hpp"

#include "send_packet_opcodes.hpp"
#include "buffvalues.hpp"
#include "buffstat_constants.hpp"

void PacketCreator::GiveEnergyCharge(int player_id, int bar, int time)
{
	if (player_id > 0)
	{
		write<short>(send_headers::kGIVE_FOREIGN_BUFF);
		write<int>(player_id);
	}
	else
	{
		write<short>(send_headers::kGIVE_BUFF);
	}

	write<long long>(0);
	write<long long>(buffstat_constants_position_2::kEnergyCharge);

	write_null(13);

	if (player_id > 0)
	{
		write_null(14);
	}

	if (bar < 10000)
	{
		write<int>(bar);
	}
	else
	{
		write<int>(10000);
	}

	write<long long>(0);
	write<signed char>(0);
	write<int>(bar >= 10000 ? time : 0);
	write<short>(0);
	write<signed char>(6);
}

void PacketCreator::ShowMonsterRiding(int player_id, int item_id, int skill_id)
{
	write<short>(send_headers::kGIVE_FOREIGN_BUFF);
	write<int>(player_id);

	write<long long>(buffstat_constants_position_2::kMonsterRiding);
	write<long long>(0);

	write<short>(0);
	write<int>(item_id);
	write<int>(skill_id);

	write<int>(0);
	write<short>(0);
	write<signed char>(0);
}

void PacketCreator::ShowForeignDashOrInfusionBuff(int player_id, Values *values, int skill_id, int time, bool is_dash)
{
	write<short>(send_headers::kGIVE_FOREIGN_BUFF);
	write<int>(player_id);

	values->sort();

	WriteBuffMask(values);

	write<short>(0);

	auto values2 = values->get_values();

	for (Value &value : *values2)
	{
		write<int>(value.get_value());
		write<int>(skill_id);
		write<int>(0);
		write<signed char>(0);
		if (!is_dash) // speed infusion
		{
			write<int>(0);
			write<signed char>(0);
		}
		write<short>(time);
	}

	write<short>(0);
	write<signed char>(is_dash ? 2 : 0);
}

void PacketCreator::GiveDashOrInfusionBuff(Values *values, int skill_id, int time, bool is_dash)
{
	write<short>(send_headers::kGIVE_BUFF);

	values->sort();

	WriteBuffMask(values);

	write<short>(0);

	auto values2 = values->get_values();

	for (auto &value : *values2)
	{
		write<int>(value.get_value());
		write<int>(skill_id);
		write<int>(0);
		write<signed char>(0);
		if (!is_dash) // speed infusion
		{
			write<int>(0);
			write<signed char>(0);
		}
		write<short>(time);
	}

	write<short>(0);
	write<signed char>(0);
}

void PacketCreator::ShowPlayerBuff(Values *values, int skill_id, int time)
{
	write<short>(send_headers::kGIVE_BUFF);

	values->sort();

	WriteBuffMask(values);

	auto values2 = values->get_values();

	for (auto &value : *values2)
	{
		write<short>(value.get_value());
		write<int>(skill_id);
		write<int>(time);
	}

	write<int>(0);
	write<short>(0);
	write<int>(0);
	write<signed char>(0);
	write<signed char>(0);
	write<signed char>(0);
}

void PacketCreator::CancelPlayerBuff(Values *values)
{
	write<short>(send_headers::kCANCEL_BUFF);

	WriteBuffMask(values);

	write<signed char>(1);
}

void PacketCreator::ShowMapBuff(int player_id, Values *values)
{
	write<short>(send_headers::kGIVE_FOREIGN_BUFF);
	write<int>(player_id);

	values->sort();

	WriteBuffMask(values);

	auto values2 = values->get_values();

	for (Value &value : *values2)
	{
		// morph is 8 bit here except for pirate morph?
		write<short>(value.get_value());
	}

	write<int>(0);
	write<short>(0);
}

void PacketCreator::CancelMapBuff(int player_id, Values *values)
{
	write<short>(send_headers::kCANCEL_FOREIGN_BUFF);
	write<int>(player_id);

	WriteBuffMask(values);
}

void PacketCreator::WriteBuffMask(Values *values)
{
	unsigned long long mask1 = 0;
	unsigned long long mask2 = 0;
	auto values2 = values->get_values();

	for (Value &value : *values2)
	{
		switch (value.get_position())
		{
		case 1:
			mask1 |= value.get_stat();
			break;
		case 2:
			mask2 |= value.get_stat();
			break;
		}
	}

	write<unsigned long long>(mask2);
	write<unsigned long long>(mask1);
}
