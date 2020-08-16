//

#include "packetcreator.hpp"

#include "equip_data.hpp"
#include "item_data_provider.hpp"
#include "item_data.hpp"
#include "item.hpp"
#include "send_packet_opcodes.hpp"
#include "constants.hpp"

void PacketCreator::ItemInfo(Item *item, bool show_position)
{
	int item_id = item->get_item_id();
	ItemData *item_data = ItemDataProvider::get_instance()->get_item_data(item_id);
	if (!item_data)
	{
		return;
	}

	enum ItemTypes
	{
		kEquip = 1,
		kItem = 2,
		kPet = 3
	};

	signed char item_type = (item->is_pet() ? kPet : item->is_equip() ? kEquip : kItem);

	bool is_cash = false;

	if (item_type == kPet)
	{
		is_cash = true;
	}

	if (item_id / 1000000 == 5)
	{
		is_cash = true;
	}

	// some checks for cash items
	// though if it's a pet, it already has the correct values due to the check above
	if (item_data->is_cash && item_type != kPet)
	{
		if (item->get_inventory_id() == kInventoryConstantsTypesEquip)
		{
			item_type = kEquip;
		}

		if (item->get_inventory_id() == kInventoryConstantsTypesCash)
		{
			item_type = kItem;
		}

		is_cash = true;
	}

	if (show_position)
	{
		signed char pos = item->get_slot();

		if (item->is_equip())
		{
			if (pos < 0)
			{
				pos *= -1;
			}
			if (pos > 100)
			{
				pos -= 100;
			}
		}

		write<signed char>(pos);

		if (item->is_equip())
		{
			write<signed char>(0);
		}
	}

	write<signed char>(item_type);
	write<int>(item_id);

	write<bool>(is_cash);

	if (is_cash)
	{
		write<long long>(item->get_unique_id());
	}

	write<long long>(item->get_expiration_time());

	switch (item_type)
	{
	case kEquip:
	{
		write<signed char>(item->get_free_slots());
		write<signed char>(item->get_used_scrolls());
		write<short>(item->get_str());
		write<short>(item->get_dex());
		write<short>(item->get_int());
		write<short>(item->get_luk());
		write<short>(item->get_hp());
		write<short>(item->get_mp());
		write<short>(item->get_weapon_attack());
		write<short>(item->get_magic_attack());
		write<short>(item->get_weapon_defense());
		write<short>(item->get_magic_defense());
		write<short>(item->get_acc());
		write<short>(item->get_avoid());
		write<short>(item->get_hand());
		write<short>(item->get_speed());
		write<short>(item->get_jump());
		write<std::string>(item->get_owner_name());
		write<short>(item->get_flag());
		write<signed char>(0); // item level up type? (from v0.95 GMS: _ZtlSecureTear_nLevelUpType_CS)
		write<signed char>(0); // item level (usually starts with 120) - only has relevance for timeless equips
		write<int>(0); // item exp (like this ?: 0 * 100000; // 10000000 = 100% ? - apparently not entirely correct) - only has relevance for timeless equips
		write<int>(item->get_hammers_used());

		if (!is_cash)
		{
			write<long long>(0); // might be -1
		}

		write<long long>(kZeroTime);
		write<int>(-1);

		break;
	}

	case kItem:
	{
		write<short>(item->get_amount());
		write<std::string>(item->get_owner_name());
		write<short>(item->get_flag());

		if (item->is_star())
		{
			write<long long>(-1);
		}

		break;
	}

	case kPet:
	{
		write_string(item->get_name(), 13);
		write<signed char>(item->get_pet_level());
		write<short>(item->get_pet_closeness());
		write<signed char>(item->get_pet_fullness());
		write<long long>(item->get_expiration_time());
		write<int>(0);
		write<short>(0);
		write<int>(0);

		break;
	}
	}
}

void PacketCreator::UpdateSlot(std::shared_ptr<Item> item)
{
	write<short>(send_headers::kMODIFY_INVENTORY_ITEM);
	write<bool>(true); // sets wether to unstuck the client and update tick
	write<signed char>(1); // how many items to upgrade

	write<signed char>(1); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(item->get_inventory_id());
	write<short>(item->get_slot());

	write<short>(item->get_amount());
}

void PacketCreator::MoveItem(signed char inventory_id, short source_slot, short destination_slot)
{
	write<short>(send_headers::kMODIFY_INVENTORY_ITEM);
	write<bool>(true); // sets wether to unstuck the client and update tick
	write<signed char>(1); // how many items to update

	write<signed char>(2); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(inventory_id);
	write<short>(source_slot);

	write<short>(destination_slot);

	// add movement
	if (destination_slot < 0 || source_slot < 0)
	{
		write<signed char>(source_slot < 0 ? 1 : 2);
	}
}

void PacketCreator::NewItem(std::shared_ptr<Item> item, bool from_drop)
{
	write<short>(send_headers::kMODIFY_INVENTORY_ITEM);
	write<bool>(from_drop); // sets wether to unstuck the client and update tick
	write<signed char>(1); // how many items to add

	write<signed char>(0); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(item->get_inventory_id());
	write<short>(item->get_slot());

	ItemInfo(item.get(), false);
}

void PacketCreator::remove_item(signed char inventory_id, int slot, bool from_drop)
{
	write<short>(send_headers::kMODIFY_INVENTORY_ITEM);
	write<bool>(from_drop); // sets wether to unstuck the client and update tick
	write<signed char>(1); // how many items to remove

	write<signed char>(3); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(inventory_id);
	write<short>(slot);

	// add movement
	if (slot < 0)
	{
		write<signed char>(2);
	}
}

void PacketCreator::MoveItemMerge(signed char inventory_id, short source_slot, short destination_slot, short amount)
{
	write<short>(send_headers::kMODIFY_INVENTORY_ITEM);
	write<bool>(true); // sets wether to unstuck the client and update tick
	write<signed char>(2); // how many items to update

	write<signed char>(3); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(inventory_id);
	write<short>(source_slot);

	write<signed char>(1); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(inventory_id);
	write<short>(destination_slot);

	write<short>(amount);

	write<signed char>(2); // add movement
}

void PacketCreator::MoveItemMergeTwo(signed char inventory_id, short source_slot, short source_amount, short destination_slot, short destination_amount)
{
	write<short>(send_headers::kMODIFY_INVENTORY_ITEM);
	write<bool>(true); // sets wether to unstuck the client and update tick
	write<signed char>(2); // how many items to update

	write<signed char>(1); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(inventory_id);
	write<short>(source_slot);

	write<short>(source_amount);

	write<signed char>(1); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(inventory_id);
	write<short>(destination_slot);

	write<short>(destination_amount);
}

void PacketCreator::ScrolledItem(std::shared_ptr<Item> scroll, std::shared_ptr<Item> equip, bool destroyed)
{
	write<short>(send_headers::kMODIFY_INVENTORY_ITEM);
	write<bool>(true); // sets wether to unstuck the client and update tick
	write<signed char>(destroyed ? 2 : 3); // how many items to update

	write<signed char>((scroll->get_amount() > 1) ? 1 : 3); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(scroll->get_inventory_id());
	write<short>(scroll->get_slot());

	if (scroll->get_amount() > 1)
	{
		write<short>(scroll->get_amount() - 1);
	}

	write<signed char>(3); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(kInventoryConstantsTypesEquip);
	write<short>(equip->get_slot());

	if (!destroyed)
	{
		write<signed char>(0); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
		write<signed char>(kInventoryConstantsTypesEquip);
		write<short>(equip->get_slot());
		ItemInfo(equip.get(), false);
	}

	if (scroll->get_amount() > 1 && scroll->get_slot() < 0 || equip->get_slot() < 0)
	{
		write<signed char>(2); // add movement
	}
}

void PacketCreator::InventoryUpdatePet(Item *pet)
{
	write<short>(send_headers::kMODIFY_INVENTORY_ITEM);
	write<bool>(false); // sets wether to unstuck the client and update tick
	write<signed char>(1); // how many items to upgrade

	write<signed char>(0); // mode (0 = add item, 1 = update quantity, 2 = move, 3 = remove)
	write<signed char>(kInventoryConstantsTypesCash);
	write<short>(pet->get_slot());

	ItemInfo(pet, false);
}

void PacketCreator::get_inventory_full()
{
	write<short>(send_headers::kMODIFY_INVENTORY_ITEM);
	write<bool>(true); // sets wether to unstuck the client and update tick
	write<signed char>(0); // how many items to upgrade
}
