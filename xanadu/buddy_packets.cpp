//

#include "packetcreator.hpp"

#include "buddy.hpp"
#include "player.hpp"
#include "send_packet_opcodes.hpp"
#include "buddylist_constants.hpp"

void PacketCreator::BuddyList(Player *player)
{
	auto buddylist = player->get_buddylist();
	signed char buddylist_size = static_cast<signed char>(buddylist->size());
	
	write<short>(send_headers::kBUDDY_LIST);
	write<signed char>(BuddylistSendPacketActions::kUpdate);
	write<signed char>(buddylist_size);

	for (auto &it : *buddylist)
	{
		auto buddy = it.second;
		WriteBuddyData(buddy->get_player_id(), buddy->get_player_name(), buddy->get_opposite_status(), buddy->get_channel_id(), std::string("Default Group"));
	}

	for (signed char i = 0; i < buddylist_size; ++i)
	{
		write<int>(0);
	}
}

void PacketCreator::BuddyListInvite(Player *player)
{
	write<short>(send_headers::kBUDDY_LIST);
	write<signed char>(BuddylistSendPacketActions::kInvite);
	write<int>(player->get_id());
	write<std::string>(player->get_name());
	WriteBuddyData(player->get_id(), player->get_name(), Buddylist::kOppositeStatusRequested, player->get_channel_id(), std::string("Default Group"));
	write<signed char>(0);
}

void PacketCreator::WriteBuddyData(int id, std::string &name, signed char opposite_status, int channel_id, std::string &group_name)
{
	write<int>(id);
	write_string(name, 13);
	write<signed char>(opposite_status);
	write<int>(channel_id);
	write_string(group_name, 17);
}

void PacketCreator::UpdateBuddyChannel(int player_id, int channel_id)
{
	write<short>(send_headers::kBUDDY_LIST);
	write<signed char>(BuddylistSendPacketActions::kUpdateChannel);
	write<int>(player_id);
	write<signed char>(0);
	write<int>(channel_id);
}

void PacketCreator::UpdateBuddyListCapacity(Player *player)
{
	write<short>(send_headers::kBUDDY_LIST);
	write<signed char>(BuddylistSendPacketActions::kUpdateCapacity);
	write<signed char>(player->get_buddy_list_capacity());
}
