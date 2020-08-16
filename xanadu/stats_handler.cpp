//

#include "player.hpp"

#include "packetcreator.hpp"
#include "constants.hpp"

void Player::handle_add_stat()
{
	if (ap_ < 1)
	{
		return;
	}

	skip_bytes(4);

	int stat = read<int>();

	switch (stat)
	{
	    case kCharacterStatsStr:
			++str_;
			break;
		case kCharacterStatsDex:
			++dex_;
			break;
		case kCharacterStatsInt:
			++int_;
			break;
		case kCharacterStatsLuk:
			++luk_;
			break;
	}

	--ap_;
	{
		PacketCreator packet;
		packet.UpdateApStats(str_, dex_, int_, luk_, ap_);
		send_packet(&packet);
	}
}

void Player::handle_auto_ap()
{
	skip_bytes(8);
	int stat1 = read<int>();
	int value1 = read<int>();
	int stat2 = read<int>();
	int value2 = read<int>();

	if (value1 < 0)
	{
		return;
	}

	if (value2 < 0)
	{
		return;
	}

	if (ap_ < (value1 + value2))
	{
		return;
	}

	switch (stat1)
	{
	case kCharacterStatsStr:
		str_ += value1;
		break;
	case kCharacterStatsDex:
		dex_ += value1;
		break;
	case kCharacterStatsInt:
		int_ += value1;
		break;
	case kCharacterStatsLuk:
		luk_ += value1;
		break;
	}

	switch (stat2)
	{
	case kCharacterStatsStr:
		str_ += value2;
		break;
	case kCharacterStatsDex:
		dex_ += value2;
		break;
	case kCharacterStatsInt:
		int_ += value2;
		break;
	case kCharacterStatsLuk:
		luk_ += value2;
		break;
	}

	ap_ -= value1;
	ap_ -= value2;
	{
		PacketCreator packet;
		packet.UpdateApStats(str_, dex_, int_, luk_, ap_);
		send_packet(&packet);
	}
}
