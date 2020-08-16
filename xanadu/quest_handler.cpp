//

#include "player.hpp"

#include "quest_constants.hpp"

void Player::handle_quest_action()
{
	signed char action = read<signed char>();
	int quest_id = read<unsigned short>();

	switch (action)
	{
	case QuestReceivePacketActions::kStart:
	{
		give_quest(quest_id);
		break;
	}
	case QuestReceivePacketActions::kComplete:
	{
		int npc_id = read<int>();
		complete_quest(quest_id, npc_id);
		break;
	}
	case QuestReceivePacketActions::kForfeit:
	{
		remove_quest(quest_id);
		break;
	}
	case QuestReceivePacketActions::kStartScripted:
	{
		break;
	}
	case QuestReceivePacketActions::kEndScripted:
	{
		break;
	}
	}
}
