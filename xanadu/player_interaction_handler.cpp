//

#include "player.hpp"

#include "map.hpp"
#include "hiredmerchant.hpp"
#include "hiredmerchant_item.hpp"
#include "world.hpp"
#include "item.hpp"
#include "item_data.hpp"
#include "item_data_provider.hpp"
#include "inventory.hpp"
#include "packetcreator.hpp"
#include "constants.hpp"

void Player::handle_item_transportation()
{
	signed char action = read<signed char>();

	switch (action)
	{
	case 0: // Create
	{
		signed char creation_type = read<signed char>();
		switch (creation_type)
		{
		case 3: // Trade
		{
			if (trade_partner_)
			{
				return;
			}
			{
				PacketCreator packet;
				packet.ShowTrade(this, nullptr, 0);
				send_packet(&packet);
			}
			break;
		}
		case 5: // Hired Merchant
		{
			if (merchant_)
			{
				return;
			}
			if (!map_->can_open_store(this))
			{
				return;
			}
			if (merchant_storage_items_.size() > 0)
			{
				{
					PacketCreator packet;
					packet.ShowMessage("Please retrieve your stored items and mesos from NPC Fredrick in the Free Market Entrance first.", 1);
					send_packet(&packet);
				}
				return;
			}

			std::string description = read<std::string>();
			skip_bytes(1);
			short item_slot = read<short>();
			int item_id = read<int>();

			Inventory *inventory = get_inventory(kInventoryConstantsTypesCash);
			if (!inventory)
			{
				return;
			}

			auto store = inventory->get_item_by_slot(static_cast<signed char>(item_slot));

			if (!store || store->get_item_id() != item_id)
			{
				return;
			}

			merchant_.reset(new HiredMerchant(this, item_id, description));
			{
				PacketCreator packet;
				packet.GetHiredMerchant(this, merchant_, true);
				send_packet(&packet);
			}
			break;
		}
		}
		break;
	}
	case 2: // Send trade request
	{
		int target_player_id = read<int>();
		Player *invited = World::get_instance()->GetPlayerById(target_player_id);

		if (!invited || invited->get_trade_partner())
		{
			{
				PacketCreator packet;
				packet.TradeError(0, 1);
				send_packet(&packet);
			}
			return;
		}

		invited->set_trade_partner(this);
		trade_partner_ = invited;
		{
			PacketCreator packet;
			packet.InviteTrade(this);
			trade_partner_->send_packet(&packet);
		}
		break;
	}
	case 3: // Deny request
	{
		trade_partner_ = nullptr;

		break;
	}
	case 4: // Enter Miniroom
	{
		if (trade_partner_)
		{
			{
				PacketCreator packet;
				packet.JoinTrade(this);
				trade_partner_->send_packet(&packet);
			}
			{
				PacketCreator packet;
				packet.ShowTrade(this, trade_partner_, 1);
				send_packet(&packet);
			}
		}

		// a merchant can either be entered as a visitor
		// or as the owner (maintenance)

		else if (!merchant_)
		{
			int merchant_id = read<int>();
			auto merchant = map_->get_hired_merchant(merchant_id);

			if (merchant)
			{
				if (merchant->is_owner(id_))
				{
					merchant_ = merchant;
					merchant->set_open(false);
					merchant->remove_all_visitors();
					{
						PacketCreator packet;
						packet.GetHiredMerchant(this, merchant, false);
						send_packet(&packet);
					}
				}
				else if (!merchant->is_open())
				{
					{
						PacketCreator packet;
						packet.PlayerShopMessage(18);
						send_packet(&packet);
					}
				}
				else if (merchant->get_empty_visitor_slot() == -1)
				{
					{
						PacketCreator packet;
						packet.PlayerShopMessage(2);
						send_packet(&packet);
					}
				}
				else
				{
					merchant_ = merchant;
					merchant->add_visitor(this);
					{
						PacketCreator packet;
						packet.GetHiredMerchant(this, merchant, false);
						send_packet(&packet);
					}
				}
			}
		}

		break;
	}
	case 6: // Miniroom Chat
	{
		std::string message = (name_ + " : " + read<std::string>());

		if (trade_partner_)
		{
			{
				PacketCreator packet;
				packet.ShowTradeChat(1, message);
				send_packet(&packet);
			}
			{
				PacketCreator packet;
				packet.ShowTradeChat(0, message);
				trade_partner_->send_packet(&packet);
			}
		}

		else if (merchant_)
		{
			merchant_->room_chat(merchant_->get_playerslot(id_), message);
		}

		break;
	}
	case 0x0A: // Exit Miniroom
	{
		if (trade_partner_)
		{
			if (trade_partner_->get_trade_partner() == this)
			{
				add_mesos(trade_mesos);

				for (auto item : trade_items_)
				{
					Inventory *inventory = get_inventory(item->get_inventory_id());

					if (!inventory)
					{
						continue;
					}

					inventory->add_item_find_slot(item);
				}

				// manage trade stuff for the other trader

				trade_partner_->add_mesos(trade_partner_->get_trade_mesos());

				auto trade_partner__items = trade_partner_->get_trade_items();

				for (auto item : *trade_partner__items)
				{
					Inventory *inventory = trade_partner_->get_inventory(item->get_inventory_id());

					if (!inventory)
					{
						continue;
					}

					inventory->add_item_find_slot(item);
				}
				{
					PacketCreator packet;
					packet.TradeError(0, 3);
					trade_partner_->send_packet(&packet);
				}
				trade_partner_->reset_trade();
			}

			reset_trade();
			{
				PacketCreator packet;
				packet.EnableAction();
				send_packet(&packet);
			}
		}
		else if (merchant_)
		{
			if (merchant_->is_owner(id_))
			{
				merchant_->set_open(true);
			}
			else
			{
				merchant_->remove_visitor(this);
			}

			merchant_.reset();
		}
		break;
	}
	case 0x0B: // Open a Miniroom / Open up a hired merchant
	{
		if (merchant_ && map_->can_open_store(this))
		{
			merchant_->set_open(true);
			map_->add_hired_merchant(merchant_);
			{
				PacketCreator packet;
				packet.SpawnHiredMerchant(merchant_);
				map_->send_packet(&packet);
			}
		}
		break;
	}
	case 0x0F: // Add item to trade
	{
		// check if the trade partner is valid
		if (!trade_partner_ || trade_partner_->get_trade_partner() != this || trade_partner_->get_map() != map_)
		{
			return;
		}

		// read the data from the packet
		signed char inventory_id = read<signed char>();
		short item_slot = read<short>();
		short item_amount = read<short>();
		signed char trade_slot = read<signed char>();

		// check if the target inventory is valid
		Inventory *inventory = get_inventory(inventory_id);
		if (!inventory)
		{
			return;
		}

		// check if the target item is valid
		std::shared_ptr<Item> item = inventory->get_item_by_slot(static_cast<signed char>(item_slot));
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

		// adjust copied item values
		copy->set_slot(trade_slot);
		copy->set_amount(item_amount);

		// add copied item to trade items
		trade_items_.push_back(copy);

		// display item in thread
		{
			PacketCreator packet;
			packet.TradeItem(copy, 0);
			send_packet(&packet);
		}
		{
			PacketCreator packet;
			packet.TradeItem(copy, 1);
			trade_partner_->send_packet(&packet);
		}
		// remove the original item amount from the inventory
		inventory->remove_item_by_slot(static_cast<signed char>(item_slot), item_amount, true, true);

		break;
	}
	case 0x10: // add mesos to trade
	{
		int mesos = read<int>();
		if (!trade_partner_ || mesos < 1 || mesos > mesos_)
		{
			return;
		}
		add_mesos(-mesos);
		trade_mesos += mesos;
		{
			PacketCreator packet;
			packet.TradeMesos(trade_mesos, 0);
			send_packet(&packet);
		}
		{
			PacketCreator packet;
			packet.TradeMesos(trade_mesos, 1);
			trade_partner_->send_packet(&packet);
		}
		break;
	}
	case 0x11: // confirm the trade
	{
		// check if the trade partner is valid
		if (!trade_partner_ || trade_partner_->get_trade_partner() != this || trade_partner_->get_map() != map_)
		{
			return;
		}
		// send the confirmation packet to the other trader
		PacketCreator packet20;
		packet20.TradeConfirm();
		trade_partner_->send_packet(&packet20);

		// check if the trade partner has confirmed the trade already
		if (!trade_partner_->is_trade_confirmed())
		{
			trade_locked_ = true;
			return;
		}
		// give the items and mesos from this player to the trade partner
		for (size_t i = 0; i < trade_items_.size(); ++i)
		{
			auto item = trade_items_[i];
			trade_partner_->get_inventory(item->get_inventory_id())->add_item_find_slot(item);
		}
		trade_partner_->add_mesos(trade_mesos);

		// give the items and mesos from the trader partner to this player
		auto pitems = trade_partner_->get_trade_items();
		for (auto item : *pitems)
		{
			Inventory *inventory = get_inventory(item->get_inventory_id());
			if (inventory)
			{
				inventory->add_item_find_slot(item);
			}
		}
		add_mesos(this->trade_partner_->get_trade_mesos());

		// send packets to show that the trade has completed
		{
			PacketCreator packet;
			packet.TradeError(0, 6);
			send_packet(&packet);
		}
		{
			PacketCreator packet;
			packet.TradeError(1, 6);
			trade_partner_->send_packet(&packet);
		}
		// reset the trade data
		trade_partner_->reset_trade();
		reset_trade();

		break;
	}
	case 20: // Hired Merchant Maintenance
	{
		if (merchant_)
		{
			return;
		}

		skip_bytes(2);
		std::string pic = read<std::string>();
		int map_object_id = read<int>();

		auto merchant = map_->get_hired_merchant(map_object_id);

		if (merchant && merchant->is_owner(id_))
		{
			{
				PacketCreator packet;
				packet.MerchantStoreMaintenanceResponse(map_object_id);
				send_packet(&packet);
			}
		}

		break;
	}
	case 0x21: // Add item into Merchant
	{
		std::shared_ptr<HiredMerchant> merchant = get_hired_merchant();

		if (!merchant || !merchant->is_owner(id_))
		{
			return;
		}

		signed char inventory_id = read<signed char>();
		short item_slot = read<short>();
		short bundles = read<short>();
		short amount_per_bundle = read<short>();
		int price = read<int>();
		Inventory *inventory = get_inventory(inventory_id);
		if (!inventory)
		{
			return;
		}
		std::shared_ptr<Item> item = inventory->get_item_by_slot(static_cast<signed char>(item_slot));
		if (!item)
		{
			return;
		}
		if ((item->get_amount() < (bundles * amount_per_bundle)) && !item->is_star())
		{
			return;
		}
		if (price < 1)
		{
			return;
		}
		// create a copy of the original item
		std::shared_ptr<Item> copy(new Item(*item));
		if (!item->is_star())
		{
			copy->set_amount(amount_per_bundle);
		}
		inventory->remove_item_by_slot(static_cast<signed char>(item_slot), (item->is_star() ? item->get_amount() : (bundles * amount_per_bundle)), true, true);

		std::shared_ptr<HiredMerchantItem> hiredmerchant_item(new HiredMerchantItem(copy, bundles, amount_per_bundle, item_slot, price));
		merchant->add_item(hiredmerchant_item);
		{
			PacketCreator packet;
			packet.UpdateHiredMerchant(merchant.get());
			send_packet(&packet);
		}
		break;
	}
	case 0x22: // Buy item in Merchant
	{
		signed char slot = read<signed char>();
		short amount = read<short>();

		if (merchant_ && !merchant_->is_owner(id_) && amount > 0 && merchant_->get_item(slot))
		{
			merchant_->buy_item(this, slot, amount);
		}

		break;
	}
	case 0x26: // Remove item from Merchant
	{
		skip_bytes(1);
		short slot = read<short>();

		if (merchant_ && merchant_->is_owner(id_) && merchant_->get_item(slot))
		{
			merchant_->return_item(slot, this);
			merchant_->remove_item(slot);
			{
				PacketCreator packet;
				packet.UpdateHiredMerchant(merchant_.get());
				send_packet(&packet);
			}
		}

		break;
	}
	case 0x27: // End of maintenance (owner goes out of store)
	{
		{
			PacketCreator packet;
			packet.EnableAction();
			send_packet(&packet);
		}
		break;
	}
	case 0x28: // Tidy up (retrieves earned mesos and removes sold items from list)
	{
		// TO-DO code this action
		{
			PacketCreator packet;
			packet.EnableAction();
			send_packet(&packet);
		}
		break;
	}
	case 0x29: // Close Merchant
	{
		if (merchant_ && merchant_->is_owner(id_))
		{
			if (map_->get_hired_merchant(merchant_->get_id()))
			{
				map_->remove_hired_merchant(merchant_->get_id());
				{
					PacketCreator packet;
					packet.DestroyHiredMerchant(id_);
					map_->send_packet(&packet);
				}
			}

			// give mesos to fredrick
			merchant_storage_mesos_ += merchant_->get_merchant_mesos();

			// give items to fredrick
			int counter = 0;
			auto items = merchant_->get_items();

			for (auto it : *items)
			{
				auto merchant_item = it.second;

				if (merchant_item->get_bundles() > 0)
				{
					merchant_storage_items_[counter] = merchant_item->get_item();
					merchant_storage_items_[counter]->set_amount(merchant_item->get_quantity());
					++counter;
				}
			}
			{
				PacketCreator packet;
				packet.EnableAction();
				send_packet(&packet);
			}
			{
				PacketCreator packet;
				packet.CloseMerchantStoreResponse();
				send_packet(&packet);
			}
		}

		merchant_.reset();

		break;
	}
	}
}
