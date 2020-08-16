//

#include "player.hpp"

#include "playernpc.hpp"
#include "packetcreator.hpp"
#include "constants.hpp"
#include "tools.hpp"

namespace StorageReceivePacketActions
{
	enum
	{
		kTakeOutItem = 4,
		kStoreItem = 5,
		kArrange = 6,
		kManageMesos = 7,
		kExit = 8
	};
}

void Player::handle_storage_reqest()
{
	npc_->set_state(1000);

	signed char action = read<signed char>();

	switch (action)
	{
	case StorageReceivePacketActions::kTakeOutItem:
	{
		signed char inventory_id = read<signed char>();

		// check if the target inventory is valid
		Inventory *inventory = get_inventory(inventory_id);
		if (!inventory)
		{
			return;
		}
		signed char slot = read<signed char>();
		if (slot >= storage_items_.size())
		{
			return;
		}
		auto item = storage_items_[slot];

		inventory->add_item_find_slot(item);

		auto iter = (storage_items_.begin() + slot);
		storage_items_.erase(iter);

		{
			PacketCreator packet;
			packet.StoreOrTakeOutStorage(false, storage_slots_, inventory_id, storage_items_);
			send_packet(&packet);
		}

		break;
	}
	case StorageReceivePacketActions::kStoreItem:
	{
		signed char item_slot = static_cast<signed char>(read<short>());
		int item_id = read<int>();
		short item_amount = read<short>();
		signed char inventory_id = tools::get_inventory_id_from_item_id(item_id);

		// check if the target inventory is valid
		Inventory *inventory = get_inventory(inventory_id);
		if (!inventory)
		{
			return;
		}

		// check if the target item is valid
		auto item = inventory->get_item_by_slot(static_cast<signed char>(item_slot));
		if (!item)
		{
			return;
		}

		// check if the target amount is valid
		if (item_amount < 1 || item_amount > item->get_amount())
		{
			return;
		}

		// create a copy of the original item
		std::shared_ptr<Item> copy(new Item(*item));
		copy->set_amount(item_amount);

		// add copied item to storage items
		signed char i;
		for (i = 0; i < storage_items_.size(); ++i)
		{
			if (storage_items_[i]->get_inventory_id() > inventory_id)
			{
				break;
			}
		}
		storage_items_.insert(storage_items_.begin() + i, copy);

		{
			// display item in storage
			PacketCreator packet;
			packet.StoreOrTakeOutStorage(true, storage_slots_, inventory_id, storage_items_);
			send_packet(&packet);
		}

		// remove the original item amount from the inventory
		inventory->remove_item_by_slot(item_slot, item_amount, true);

		break;
	}
	case StorageReceivePacketActions::kArrange:
	{
		// to-do

		break;
	}
	case StorageReceivePacketActions::kManageMesos:
	{
		int meso_value = read<int>();

		storage_mesos_ -= meso_value;
		add_mesos(meso_value);

		{
			PacketCreator packet;
			packet.MesoStorage(storage_slots_, storage_mesos_);
			send_packet(&packet);
		}

		break;
	}
	}
}

// handler action constants

constexpr signed char kFredrickHandlerActionTakeOut = 26;
constexpr signed char kFredrickHandlerActionExit = 28;

void Player::handle_merchant_storage_request()
{
	npc_->set_state(1000);

	signed char action = read<signed char>();

	switch (action)
	{
	case kFredrickHandlerActionTakeOut:
	{
		if (!add_mesos(merchant_storage_mesos_))
		{
			{
				PacketCreator packet;
				packet.FredrickMessage(31);
				send_packet(&packet);
			}
			return;
		}

		merchant_storage_mesos_ = 0;

		signed char free_equip_slots = get_inventory(kInventoryConstantsTypesEquip)->get_free_slots();
		signed char free_etc_slots = get_inventory(kInventoryConstantsTypesEtc)->get_free_slots();
		signed char free_use_slots = get_inventory(kInventoryConstantsTypesUse)->get_free_slots();
		signed char free_setup_slots = get_inventory(kInventoryConstantsTypesSetup)->get_free_slots();

		for (auto it : merchant_storage_items_)
		{
			auto item = it.second;
			signed char inventory_id = item->get_inventory_id();

			switch (inventory_id)
			{
			case kInventoryConstantsTypesEquip:
				--free_equip_slots;
				break;
			case kInventoryConstantsTypesEtc:
				--free_etc_slots;
				break;
			case kInventoryConstantsTypesUse:
				--free_use_slots;
				break;
			case kInventoryConstantsTypesSetup:
				--free_setup_slots;
				break;
			}
		}

		if (free_equip_slots < 0 || free_etc_slots < 0 || free_use_slots < 0 || free_setup_slots < 0)
		{
			{
				PacketCreator packet;
				packet.FredrickMessage(34);
				send_packet(&packet);
			}

			return;
		}

		for (auto it : merchant_storage_items_)
		{
			auto item = it.second;
			signed char inventory_id = item->get_inventory_id();

			// check if the target inventory is valid
			Inventory *inventory = get_inventory(inventory_id);
			if (!inventory)
			{
				return;
			}

			if (!inventory->add_item_find_slot(item))
			{
				{
					PacketCreator packet;
					packet.FredrickMessage(34);
					send_packet(&packet);
				}

				return;
			}
		}

		merchant_storage_items_.clear();

		{
			PacketCreator packet;
			packet.FredrickMessage(30);
			send_packet(&packet);
		}

		break;
	}
	case kFredrickHandlerActionExit:
	{
		break;
	}
	}
}
