//

#include "player.hpp"

#include "inventory.hpp"
#include "map.hpp"
#include "session.hpp"
#include "packetcreator.hpp"
#include "constants.hpp"

void Player::handle_use_pet()
{
	skip_bytes(4);
	signed char slot = read<signed char>();

	std::shared_ptr<Item> pet = get_inventory(kInventoryConstantsTypesCash)->get_item_by_slot(slot);

	if (!pet || !pet->is_pet())
	{
		return;
	}

	std::shared_ptr<Item> pet_test = get_pet(pet->get_unique_id());

	if (pet_test)
	{
		// remove pet
		for (size_t i = 0; i < pets_.size(); ++i)
		{
			if (pets_[i] == pet)
			{
				{
					PacketCreator packet;
					packet.ShowPet(id_, pet, false);
					map_->send_packet(&packet);
				}
				pet->set_pet_slot(-1);
				pets_.erase(pets_.begin() + i);

				break;
			}
		}

		for (size_t i = 0; i < pets_.size(); ++i)
		{
			pets_[i]->set_pet_slot(static_cast<signed char>(i));
		}
		{
			PacketCreator packet;
			packet.EnableAction();
			send_packet(&packet);
		}
	}
	else
	{
		// add pet
		if (pets_.size() == 3)
		{
			// instead of the first pet

			pets_[0]->set_pet_slot(-1);
			pet->set_pet_slot(0);
			pets_[0] = pet;
		}
		else
		{
			pet->set_pet_slot(static_cast<signed char>(pets_.size()));
			pets_.push_back(pet);
		}

		pet->set_position(position_x_, position_y_, 0);

		{
			PacketCreator packet;
			packet.ShowPet(id_, pet, true);
			map_->send_packet(&packet);
		}
		{
			PacketCreator packet;
			packet.PetStatUpdate(this);
			send_packet(&packet);
		}
	}
}

void Player::handle_pet_movement()
{
	long long pet_object_id = read<long long>();
	std::shared_ptr<Item> pet = get_pet(pet_object_id);
	if (!pet)
	{
		return;
	}

	short start_position_x = read<short>();
	short start_position_y = read<short>();

	pet->set_position(start_position_x, start_position_y, 0);

	short pet_position_x = 0;
	short pet_position_y = 0;
	short pet_foothold = 0;
	signed char pet_stance = 0;

	movement_handler(pet_position_x, pet_position_y, pet_foothold, pet_stance);

	if (pet_position_x != 0 || pet_position_y != 0 || pet_foothold != 0 || pet_stance != 0)
	{
		pet->set_position(pet_position_x, pet_position_y, 0);
	}

	// only send if there are other players in the map
	if (map_->get_players()->size() > 1)
	{
		// exclude the first 2 bytes (header) + 12 bytes = 14 and from the packet that is to be sent
		constexpr const int excluded_bytes = (12 + 2);
		int size = (recv_length_ - excluded_bytes);
		if (size < 1)
		{
			return;
		}
		{
			PacketCreator packet;
			packet.MovePet(id_, pet->get_pet_slot(), start_position_x, start_position_y, session_->get_receive_buffer() + excluded_bytes, recv_length_ - excluded_bytes);
			map_->send_packet(&packet, this);
		}
	}
}

void Player::handle_pet_command()
{
	long long pet_object_id = read<long long>();
	std::shared_ptr<Item> pet = get_pet(pet_object_id);

	if (!pet)
	{
		return;
	}

	skip_bytes(1);
	signed char command = read<signed char>();
	pet->set_closeness(pet->get_pet_closeness() + 1, this);
	{
		PacketCreator packet;
		packet.PetCommandReplay(id_, pet, command);
		send_packet(&packet);
	}
}

void Player::handle_pet_chat()
{
	long long pet_object_id = read<long long>();
	std::shared_ptr<Item> pet = get_pet(pet_object_id);

	if (!pet)
	{
		return;
	}

	skip_bytes(1);
	signed char act = read<signed char>();
	std::string message = read<std::string>();
	{
		PacketCreator packet;
		packet.ShowPetChat(id_, pet, act, message);
		map_->send_packet(&packet, this);
	}
}

void Player::handle_pet_loot()
{
	long long pet_object_id = read<long long>();
	std::shared_ptr<Item> pet = get_pet(pet_object_id);

	if (!pet)
	{
		return;
	}
	skip_bytes(9);

	int drop_object_id = read<int>();

	std::shared_ptr<Drop> drop = map_->get_drop(drop_object_id);

	if (drop)
	{
		map_->loot_drop(this, drop, pet->get_pet_slot());
	}
	{
		PacketCreator packet;
		packet.EnableAction();
		send_packet(&packet);
	}
}
