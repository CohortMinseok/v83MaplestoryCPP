//

#include "packetcreator.hpp"

#include "player.hpp"
#include "send_packet_opcodes.hpp"
#include "inventory.hpp"
#include "item.hpp"
#include "constants.hpp"

void PacketCreator::ShowPet(int owner_player_id, std::shared_ptr<Item> pet, bool show)
{
	write<short>(send_headers::kPET_SPAWN);
	write<int>(owner_player_id);
	write<signed char>(pet->get_pet_slot());
	write<bool>(show);
	write<signed char>(0);

	if (show)
	{
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
}

void PacketCreator::MovePet(int owner_player_id, signed char pet_slot, short start_position_x, short start_position_y, unsigned char *buffer, int size)
{
	write<short>(send_headers::kPET_MOVE);
	write<int>(owner_player_id);
	write<signed char>(pet_slot);

	// begin of movement data

	write<short>(start_position_x);
	write<short>(start_position_y);

	// copy the movement data into the packet buffer
	memcpy(buffer_ + length_, buffer, size);
	length_ += size;

	// end of movement data
}

void PacketCreator::ShowPetChat(int owner_player_id, std::shared_ptr<Item> pet, signed char act, const std::string& message)
{
	write<short>(send_headers::kPET_CHAT);
	write<int>(owner_player_id);
	write<signed char>(pet->get_pet_slot());
	write<signed char>(0);
	write<signed char>(act);
	write<std::string>(message);
	write<signed char>(0);
}

void PacketCreator::PetCommandReplay(int owner_player_id, std::shared_ptr<Item> pet, signed char animation)
{
	write<short>(send_headers::kPET_COMMAND);
	write<int>(owner_player_id);
	write<signed char>(pet->get_pet_slot());
	write<signed char>(0); // if not food, this byte is needed
	write<bool>(animation == 1);
	write<signed char>(animation);
	write<signed char>(0); // success byte
	write<signed char>(0);
}
