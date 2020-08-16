//

#include "player.hpp"

#include "packetcreator.hpp"
#include "inventory.hpp"
#include "channel.hpp"
#include "world.hpp"
#include "effect.hpp"
#include "map.hpp"
#include "item.hpp"
#include "item_data.hpp"
#include "equip_data.hpp"
#include "item_data_provider.hpp"
#include "constants.hpp"

void Player::handle_use_skill_book()
{

}

void Player::handle_player_use_chair()
{
	int item_id = read<int>();
	Inventory *inventory = get_inventory(kInventoryConstantsTypesSetup);

	if (!inventory)
	{
		return;
	}

	if (inventory->get_item_amount(item_id) == 0)
	{
		return;
	}

	chair_ = item_id;

	if (map_->get_players()->size() > 1)
	{
		{
			PacketCreator packet;
			packet.ShowChair(id_, item_id);
			map_->send_packet(&packet, this);
		}
	}
}

void Player::handle_player_cancel_chair()
{
	chair_ = 0;
	{
		PacketCreator packet;
		packet.CancelChair();
		send_packet(&packet);
	}
	if (map_->get_players()->size() > 1)
	{
		{
			PacketCreator packet;
			packet.ShowChair(id_, 0);
			map_->send_packet(&packet, this);
		}
	}
}

void Player::handle_use_item_effect()
{
	int item_id = read<int>();
	if (item_id != 0)
	{
		signed char inventory_id = tools::get_inventory_id_from_item_id(item_id);
		Inventory *inventory = get_inventory(inventory_id);
		if (!inventory)
		{
			return;
		}
		if (inventory->get_item_amount(item_id) == 0)
		{
			return;
		}
	}

	item_effect_ = item_id;

	if (map_->get_players()->size() > 1)
	{
		{
			PacketCreator packet;
			packet.UseItemEffect(id_, item_id);
			map_->send_packet(&packet, this);
		}
	}
}

void Player::handle_close_chalkboard()
{
	chalk_board_text_ = "";
	{
		PacketCreator packet;
		packet.HandleCloseChalkboard(id_);
		map_->send_packet(&packet);
	}
}

void Player::handle_cancel_item_buff()
{
	int buff_id = read<int>();
	cancel_buff(buff_id, false);
}

void Player::handle_use_cash_item()
{
	short item_slot = read<short>();
	int item_id = read<int>();
	signed char inventory_id = tools::get_inventory_id_from_item_id(item_id);

	Inventory *cash_inventory = get_inventory(inventory_id);
	if (!cash_inventory)
	{
		return;
	}

	std::shared_ptr<Item> item(cash_inventory->get_item_by_slot(static_cast<signed char>(item_slot)));
	if (!item)
	{
		return;
	}

	if (item->get_item_id() != item_id)
	{
		return;
	}

	switch (item_id)
	{
	case 5071000: // Megaphone
	{
		std::string message = (name_ + " : " + read<std::string>());
		{
			PacketCreator packet;
			packet.ShowMessage(message, 2);
			map_->send_packet(&packet);
		}
		cash_inventory->remove_item(item_id, 1);

		break;
	}
	case 5072000: // Super Megaphone
	{
		std::string message = (name_ + " : " + read<std::string>());
		signed char whisper = read<signed char>();
		{
			PacketCreator packet;
			packet.ShowMessage(message, 3, channel_id_, whisper);
			World::get_instance()->send_packet(&packet);
		}
		cash_inventory->remove_item(item_id, 1);
		break;
	}
	case 5076000: // Item Megaphone
	{
		std::string message = (get_name() + " : " + read<std::string>());
		signed char whisper = read<signed char>();
		signed char has_item = read<signed char>();

		if (has_item)
		{
			int inventory_id = read<int>();
			Inventory *inventory = get_inventory(inventory_id);

			if (!inventory)
			{
				return;
			}

			int slot = read<int>();

			std::shared_ptr<Item> item = inventory->get_item_by_slot(slot);

			if (!item)
			{
				return;
			}
		}
		{
			PacketCreator packet;
			packet.ShowMessage(message, 8, channel_id_, whisper, item);
			World::get_instance()->send_packet(&packet);
		}
		cash_inventory->remove_item(item_id, 1);

		break;
	}
	case 5077000: // Triple Megaphone
	{
		signed char lines = read<signed char>();

		if (lines < 1 || lines > 3)
		{
			return;
		}

		std::string text[] = { "", "", "" };

		for (unsigned char i = 0; i < lines; ++i)
		{
			std::string line = (name_ + " : " + read<std::string>());

			text[i] = line;
		}

		signed char whisper = read<signed char>();

		for (unsigned char x = 0; x < lines; ++x)
		{
			{
				PacketCreator packet;
				packet.ShowMessage(text[x], 3, channel_id_, whisper);
				World::get_instance()->send_packet(&packet);
			}
		}

		cash_inventory->remove_item(item_id, 1);

		break;
	}
	case 5202000: // Rare Meso Sack
	{
		int gain = tools::random_int(10000, 5000000);
		if (!add_mesos(gain))
		{
			{
				PacketCreator packet;
				packet.ShowMessage("Failed to receive mesos.", 1);
				send_packet(&packet);
			}
			return;
		}
		cash_inventory->remove_item(item_id, 1);
		break;
	}
	case 5202001: // Unique Meso Sack
	{
		int gain = tools::random_int(1000000, 7000000);
		if (!add_mesos(gain))
		{
			{
				PacketCreator packet;
				packet.ShowMessage("Failed to receive mesos.", 1);
				send_packet(&packet);
			}
			return;
		}
		cash_inventory->remove_item(item_id, 1);
		break;
	}
	case 5202002: // Epic Meso Sack
	{
		int gain = tools::random_int(1000000, 20000000);
		if (!add_mesos(gain))
		{
			{
				PacketCreator packet;
				packet.ShowMessage("Failed to receive mesos.", 1);
				send_packet(&packet);
			}
			return;
		}
		cash_inventory->remove_item(item_id, 1);
		break;
	}
	case 5370000: // Chalk Board
	{
		chalk_board_text_ = read<std::string>();
		{
			PacketCreator packet;
			packet.UseChalkBoard(id_, chalk_board_text_);
			map_->send_packet(&packet);
		}
		{
			PacketCreator packet;
			packet.EnableAction();
			send_packet(&packet);
		}
		break;
	}
	case 5390000: // DiabloMessenger
	case 5390001: // Cloud9Messenger
	case 5390002: // LoveholicMessenger
	{
		std::string msg = read<std::string>();
		std::string msg2 = read<std::string>();
		std::string msg3 = read<std::string>();
		std::string msg4 = read<std::string>();
		signed char whisper = read<signed char>();

		{
			PacketCreator packet;
			packet.ShowAvatarMega(this, whisper, item_id, msg, msg2, msg3, msg4);
			World *world = World::get_instance();
			world->send_packet(&packet);
		}
		cash_inventory->remove_item(item_id, 1);
		break;
	}

	case 5520000: // Scissors of Karma
	{
		int inventory_id = read<int>();
		Inventory *inventory = get_inventory(inventory_id);
		if (!inventory)
		{
			return;
		}
		int slot = read<int>();
		std::shared_ptr<Item> item = inventory->get_item_by_slot(slot);
		if (!item)
		{
			return;
		}
		item->set_flag(item->get_flag() | kItemConstantsFlagsKarma);
		{
			PacketCreator packet;
			packet.NewItem(item, false);
			send_packet(&packet);
		}
		cash_inventory->remove_item(item_id, 1);
		break;
	}
	case 5570000: // Vicious Hammer
		{
			int inventory_id = read<int>();
			int equip_slot = read<int>();
			Inventory *inventory = get_inventory(inventory_id);
			if (!inventory)
			{
				return;
			}

			std::shared_ptr<Item> equip = inventory->get_item_by_slot(equip_slot);
			if (!equip)
			{
				return;
			}
			if (equip->get_hammers_used() == 2)
			{
				return;
			}

			equip->set_hammers_used(equip->get_hammers_used() + 1);
			equip->set_free_slots(equip->get_free_slots() + 1);
			{
				PacketCreator packet;
				packet.EnableAction();
				send_packet(&packet);
			}
			{
				PacketCreator packet;
				packet.SendHammerData(equip->get_hammers_used());
				send_packet(&packet);
			}
			cash_inventory->remove_item(item_id, 1);

			break;
		}
	}
}

void Player::handle_move_item()
{
	skip_bytes(4);
	signed char inventory_id1 = read<signed char>();
	signed char inventory_id2 = inventory_id1;
	signed char slot1 = static_cast<signed char>(read<short>());
	signed char slot2 = static_cast<signed char>(read<short>());
	bool is_drop = (slot2 == 0);

	if (slot1 < 0)
	{
		inventory_id1 = kInventoryConstantsTypesEquipped;
	}

	if (slot2 < 0)
	{
		inventory_id2 = kInventoryConstantsTypesEquipped;
	}

	Inventory *inventory1 = get_inventory(inventory_id1);

	if (!inventory1)
	{
		return;
	}
	std::shared_ptr<Item> item1 = inventory1->get_item_by_slot(slot1);

	if (!item1)
	{
		return;
	}

	if (is_drop)
	{
		short amount = read<short>();

		if ((amount > item1->get_amount() || amount < 1) && !item1->is_star())
		{
			return;
		}

		if (item1->is_star())
		{
			amount = item1->get_amount();
		}

		if (amount != item1->get_amount())
		{
			std::shared_ptr<Item> item(new Item(item1->get_item_id()));
			item->set_amount(amount);

			item1->set_amount(item1->get_amount() - amount);

			{
				PacketCreator packet;
				packet.UpdateSlot(item1);
				send_packet(&packet);
			}
			map_->drop_from_player(item, this);
		}
		else
		{
			inventory1->remove_item(slot1, true, false);

			{
				PacketCreator packet;
				packet.MoveItem((inventory_id1 == kInventoryConstantsTypesEquipped
					|| inventory_id2 == kInventoryConstantsTypesEquipped) ? kInventoryConstantsTypesEquip : inventory_id1, slot1, slot2);
				send_packet(&packet);
			}
			map_->drop_from_player(item1, this);
		}
	}
	else
	{
		Inventory *inventory2 = get_inventory(inventory_id2);
		if (!inventory2)
		{
			return;
		}

		std::shared_ptr<Item> item2 = inventory2->get_item_by_slot(slot2);

		// move to empty pos

		if (!item2)
		{
			item1->set_slot(slot2);
			inventory1->remove_item(slot1, false, false);
			inventory2->add_item_no_find_slot(item1, false);
			{
				PacketCreator packet;
				packet.MoveItem((inventory_id1 == 0 || inventory_id2 == 0) ? 1 : inventory_id1, slot1, slot2);
				send_packet(&packet);
			}
		}

		// switch positions

		else if (item2->get_item_id() != item1->get_item_id()
			|| inventory_id1 <= 1
			|| inventory_id2 <= 1
			|| item1->is_star()
			|| item2->is_star())
		{
			item1->set_slot(slot2);
			item2->set_slot(slot1);
			inventory1->remove_item(slot1, false, false);
			inventory2->remove_item(slot2, false, false);
			inventory2->add_item_no_find_slot(item1, false);
			inventory1->add_item_no_find_slot(item2, false);
			{
				PacketCreator packet;
				packet.MoveItem((inventory_id1 == kInventoryConstantsTypesEquipped
					|| inventory_id2 == kInventoryConstantsTypesEquipped) ? kInventoryConstantsTypesEquip : inventory_id1, slot1, slot2);
				send_packet(&packet);
			}
		}
		else
		{
			// merge

			ItemData *data = ItemDataProvider::get_instance()->get_item_data(item1->get_item_id());
			if (!data)
			{
				return;
			}

			short max_per_slot = data->max_per_slot;

			if (item1->get_amount() + item2->get_amount() <= max_per_slot)
			{
				item2->set_amount(item1->get_amount() + item2->get_amount());
				{
					PacketCreator packet;
					packet.MoveItemMerge(inventory_id2, slot1, slot2, item2->get_amount());
					send_packet(&packet);
				}
				inventory1->remove_item(slot1, true, false);
			}
			else
			{
				short change = max_per_slot - item2->get_amount();
				item2->set_amount(max_per_slot);
				item1->set_amount(item1->get_amount() - change);
				{
					PacketCreator packet;
					packet.MoveItemMergeTwo(inventory_id1, slot1, item1->get_amount(), slot2, item2->get_amount());
					send_packet(&packet);
				}
			}
		}
	}

	if (inventory_id1 == kInventoryConstantsTypesEquipped ||
		inventory_id2 == kInventoryConstantsTypesEquipped)
	{
		PacketCreator packet;
		packet.UpdatePlayer(this);
		map_->send_packet(&packet, this);
	}
}

void Player::handle_use_item()
{
	skip_bytes(4);
	signed char item_slot = static_cast<signed char>(read<short>());
	int item_id = read<int>();
	signed char inventory_id = tools::get_inventory_id_from_item_id(item_id);
	Inventory *inventory = get_inventory(inventory_id);
	if (!inventory)
	{
		return;
	}

	auto item = inventory->get_item_by_slot(item_slot);

	if (!item)
	{
		return;
	}

	if (item->get_item_id() != item_id)
	{
		return;
	}

	Effect *effect = Effect::get_item_effect(item_id);

	if (effect)
	{
		effect->use_item(this);
	}

	inventory->remove_item_by_slot(item_slot, 1);
}

void Player::handle_use_return_scroll()
{
	skip_bytes(4);
	signed char item_slot = static_cast<signed char>(read<short>());

	Inventory *inventory = get_inventory(kInventoryConstantsTypesUse);
	if (!inventory)
	{
		return;
	}

	auto item = inventory->get_item_by_slot(item_slot);

	if (!item)
	{
		return;
	}

	Channel *channel = World::get_instance()->GetChannel(channel_id_);

	auto item_data = ItemDataProvider::get_instance()->get_item_data(item->get_item_id());

	if (!item_data)
	{
		return;
	}

	int tomapid = item_data->move_to;

	if (tomapid == 999999999)
	{
		int mapid = map_->get_return_map();

		if (mapid == 999999999)
		{
			change_map(map_);
		}
		else
		{
			Map *destination = channel->get_map(mapid);

			if (!destination)
			{
				change_map(map_);
			}
			else
			{
				change_map(destination);
			}
		}
	}
	else
	{
		Map *destination = channel->get_map(tomapid);

		if (destination)
		{
			change_map(destination);
		}
	}

	inventory->remove_item_by_slot(item_slot, 1);
}

void Player::handle_use_scroll()
{
	skip_bytes(4);
	signed char scroll_slot = static_cast<signed char>(read<short>());
	signed char equip_slot = static_cast<signed char>(read<short>());
	bool white_scroll = (read<short>() == 2);
	bool legendary_spirit = false;

	Inventory *use_inventory = get_inventory(kInventoryConstantsTypesUse);
	if (!use_inventory)
	{
		return;
	}

	std::shared_ptr<Item> scroll = use_inventory->get_item_by_slot(scroll_slot);
	if (!scroll)
	{
		return;
	}

	Inventory *equip_inventory = get_inventory(kInventoryConstantsTypesEquip);
	if (!equip_inventory)
	{
		return;
	}

	Inventory *equipped_inventory = get_inventory(kInventoryConstantsTypesEquipped);
	if (!equipped_inventory)
	{
		return;
	}

	std::shared_ptr<Item> equip = equip_inventory->get_item_by_slot(equip_slot);
	if (!equip)
	{
		equip = equipped_inventory->get_item_by_slot(equip_slot);

		if (!equip)
		{
			return;
		}
	}

	if (equip->get_free_slots() < 1)
	{
		return;
	}

	ItemData *item_data = ItemDataProvider::get_instance()->get_item_data(scroll->get_item_id());
	if (!item_data)
	{
		return;
	}

	bool success = false;
	bool cursed = false;

	if (tools::random_int(0, 99) < item_data->success)
	{
		success = true;

		switch (scroll->get_item_id())
		{
		case 2049100: // Chaos Scroll 60%
		{
			if (equip->get_str() > 0)
			{
				equip->set_str(equip->get_str() + tools::random_int(-5, 5));
			}
			if (equip->get_dex() > 0)
			{
				equip->set_dex(equip->get_dex() + tools::random_int(-5, 5));
			}
			if (equip->get_int() > 0)
			{
				equip->set_int(equip->get_int() + tools::random_int(-5, 5));
			}
			if (equip->get_luk() > 0)
			{
				equip->set_luk(equip->get_luk() + tools::random_int(-5, 5));
			}
			if (equip->get_hp() > 0)
			{
				equip->set_hp(equip->get_hp() + tools::random_int(-5, 5));
			}
			if (equip->get_mp() > 0)
			{
				equip->set_mp(equip->get_mp() + tools::random_int(-5, 5));
			}
			if (equip->get_weapon_attack() > 0)
			{
				equip->set_weapon_attack(equip->get_weapon_attack() + tools::random_int(-5, 5));
			}
			if (equip->get_magic_attack() > 0)
			{
				equip->set_magic_attack(equip->get_magic_attack() + tools::random_int(-5, 5));
			}
			if (equip->get_weapon_defense() > 0)
			{
				equip->set_weapon_defense(equip->get_weapon_defense() + tools::random_int(-5, 5));
			}
			if (equip->get_magic_defense() > 0)
			{
				equip->set_magic_defense(equip->get_magic_defense() + tools::random_int(-5, 5));
			}
			if (equip->get_acc() > 0)
			{
				equip->set_acc(equip->get_acc() + tools::random_int(-5, 5));
			}
			if (equip->get_avoid() > 0)
			{
				equip->set_avoid(equip->get_avoid() + tools::random_int(-5, 5));
			}
			if (equip->get_hand() > 0)
			{
				equip->set_hand(equip->get_hand() + tools::random_int(-5, 5));
			}
			if (equip->get_jump() > 0)
			{
				equip->set_jump(equip->get_jump() + tools::random_int(-5, 5));
			}
			if (equip->get_speed() > 0)
			{
				equip->set_speed(equip->get_speed() + tools::random_int(-5, 5));
			}

			break;
		}
		case 2049122: // Chaos Scroll of Goodness 50%
		case 2049129: // Chaos Scroll of Goodness 50%
		case 2049130: // Chaos Scroll of Goodness 30%
		case 2049131: // Chaos Scroll of Goodness 20%
		case 2049135: // Incredible Chaos Scroll of Goodness 20%
		case 2049136: // Incredible Chaos Scroll of Goodness 20%
		case 2049137: // Incredible Chaos Scroll of Goodness 40%
		{
			if (equip->get_str() > 0)
			{
				equip->set_str(equip->get_str() + tools::random_int(1, 5));
			}
			if (equip->get_dex() > 0)
			{
				equip->set_dex(equip->get_dex() + tools::random_int(1, 5));
			}
			if (equip->get_int() > 0)
			{
				equip->set_int(equip->get_int() + tools::random_int(1, 5));
			}
			if (equip->get_luk() > 0)
			{
				equip->set_luk(equip->get_luk() + tools::random_int(1, 5));
			}
			if (equip->get_hp() > 0)
			{
				equip->set_hp(equip->get_hp() + tools::random_int(1, 5));
			}
			if (equip->get_mp() > 0)
			{
				equip->set_mp(equip->get_mp() + tools::random_int(1, 5));
			}
			if (equip->get_weapon_attack() > 0)
			{
				equip->set_weapon_attack(equip->get_weapon_attack() + tools::random_int(1, 5));
			}
			if (equip->get_magic_attack() > 0)
			{
				equip->set_magic_attack(equip->get_magic_attack() + tools::random_int(1, 5));
			}
			if (equip->get_weapon_defense() > 0)
			{
				equip->set_weapon_defense(equip->get_weapon_defense() + tools::random_int(1, 5));
			}
			if (equip->get_magic_defense() > 0)
			{
				equip->set_magic_defense(equip->get_magic_defense() + tools::random_int(1, 5));
			}
			if (equip->get_acc() > 0)
			{
				equip->set_acc(equip->get_acc() + tools::random_int(1, 5));
			}
			if (equip->get_avoid() > 0)
			{
				equip->set_avoid(equip->get_avoid() + tools::random_int(1, 5));
			}
			if (equip->get_hand() > 0)
			{
				equip->set_hand(equip->get_hand() + tools::random_int(1, 5));
			}
			if (equip->get_jump() > 0)
			{
				equip->set_jump(equip->get_jump() + tools::random_int(1, 5));
			}
			if (equip->get_speed() > 0)
			{
				equip->set_speed(equip->get_speed() + tools::random_int(1, 5));
			}

			break;
		}
		case 2049132: // Miraculous Chaos Scroll 30%
		case 2049133: // Miraculous Chaos Scroll 50%
		case 2049134: // Miraculous Chaos Scroll 70%
		{
			if (equip->get_str() > 0)
			{
				equip->set_str(equip->get_str() + tools::random_int(-10, 10));
			}
			if (equip->get_dex() > 0)
			{
				equip->set_dex(equip->get_dex() + tools::random_int(-10, 10));
			}
			if (equip->get_int() > 0)
			{
				equip->set_int(equip->get_int() + tools::random_int(-10, 10));
			}
			if (equip->get_luk() > 0)
			{
				equip->set_luk(equip->get_luk() + tools::random_int(-10, 10));
			}
			if (equip->get_hp() > 0)
			{
				equip->set_hp(equip->get_hp() + tools::random_int(-10, 10));
			}
			if (equip->get_mp() > 0)
			{
				equip->set_mp(equip->get_mp() + tools::random_int(-10, 10));
			}
			if (equip->get_weapon_attack() > 0)
			{
				equip->set_weapon_attack(equip->get_weapon_attack() + tools::random_int(-10, 10));
			}
			if (equip->get_magic_attack() > 0)
			{
				equip->set_magic_attack(equip->get_magic_attack() + tools::random_int(-10, 10));
			}
			if (equip->get_weapon_defense() > 0)
			{
				equip->set_weapon_defense(equip->get_weapon_defense() + tools::random_int(-10, 10));
			}
			if (equip->get_magic_defense() > 0)
			{
				equip->set_magic_defense(equip->get_magic_defense() + tools::random_int(-10, 10));
			}
			if (equip->get_acc() > 0)
			{
				equip->set_acc(equip->get_acc() + tools::random_int(-10, 10));
			}
			if (equip->get_avoid() > 0)
			{
				equip->set_avoid(equip->get_avoid() + tools::random_int(-10, 10));
			}
			if (equip->get_hand() > 0)
			{
				equip->set_hand(equip->get_hand() + tools::random_int(-10, 10));
			}
			if (equip->get_jump() > 0)
			{
				equip->set_jump(equip->get_jump() + tools::random_int(-10, 10));
			}
			if (equip->get_speed() > 0)
			{
				equip->set_speed(equip->get_speed() + tools::random_int(-10, 10));
			}

			break;
		}
		default: // normal scrolls
		{
			equip->set_str(equip->get_str() + item_data->str);
			equip->set_dex(equip->get_dex() + item_data->dex);
			equip->set_int(equip->get_int() + item_data->intt);
			equip->set_luk(equip->get_luk() + item_data->luk);
			equip->set_hp(equip->get_hp() + item_data->hp);
			equip->set_mp(equip->get_mp() + item_data->mp);
			equip->set_weapon_attack(equip->get_weapon_attack() + item_data->watk);
			equip->set_magic_attack(equip->get_magic_attack() + item_data->matk);
			equip->set_weapon_defense(equip->get_weapon_defense() + item_data->wdef);
			equip->set_magic_defense(equip->get_magic_defense() + item_data->mdef);
			equip->set_acc(equip->get_acc() + item_data->acc);
			equip->set_avoid(equip->get_avoid() + item_data->avo);
			equip->set_jump(equip->get_jump() + item_data->jump);
			equip->set_speed(equip->get_speed() + item_data->speed);

			if (item_data->recover)
			{
				equip->set_free_slots(equip->get_free_slots() + 1);
			}
			else
			{
				equip->set_used_scrolls(equip->get_used_scrolls() + 1);
			}
		}
		}
	}
	else
	{
		if (tools::random_int(0, 99) < item_data->cursed)
		{
			cursed = true;
		}
	}

	if (!(white_scroll && !success) && item_data->recover == false)
	{
		equip->set_free_slots(equip->get_free_slots() - 1);
	}
	{
		PacketCreator packet;
		packet.ScrolledItem(scroll, equip, cursed);
		send_packet(&packet);
	}
	{
		PacketCreator packet;
		packet.UseScroll(get_id(), success, cursed, legendary_spirit);
		map_->send_packet(&packet);
	}
	get_inventory(kInventoryConstantsTypesUse)->remove_item_by_slot(scroll_slot, 1, false);

	if (cursed)
	{
		get_inventory(kInventoryConstantsTypesEquipped)->remove_item_by_slot(equip_slot, 1, false);
	}
}

void Player::handle_inventory_sort()
{
	skip_bytes(4);
	signed char inventory_id = read<signed char>();

	Inventory *inventory = get_inventory(inventory_id);
	if (!inventory)
	{
		return;
	}

	std::vector<std::shared_ptr<Item>> items;
	auto hitems = inventory->get_items();

	for (auto it : *hitems)
	{
		items.push_back(it.second);
	}

	for (signed char slot2 = 1; slot2 <= static_cast<signed char>(items.size()); ++slot2)
	{
		std::shared_ptr<Item> item1 = items[slot2 - 1];
		std::shared_ptr<Item> item2 = inventory->get_item_by_slot(slot2);

		if (item1 == item2)
		{
			continue;
		}

		signed char slot1 = item1->get_slot();

		item1->set_slot(slot2);

		if (item2)
		{
			item2->set_slot(slot1);
			inventory->remove_item(slot1, false, false);
			inventory->remove_item(slot2, false, false);
			inventory->add_item_no_find_slot(item1, false);
			inventory->add_item_no_find_slot(item2, false);
		}
		else
		{
			inventory->remove_item(slot1, false, false);
			inventory->add_item_no_find_slot(item1, false);
		}
		{
			PacketCreator packet;
			packet.MoveItem(inventory_id, slot1, slot2);
			send_packet(&packet);
		}
	}
	{
		PacketCreator packet;
		packet.EnableAction();
		send_packet(&packet);
	}
}
