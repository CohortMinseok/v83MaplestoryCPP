//

#include "player.hpp"

#include "map.hpp"
#include "mob.hpp"
#include "buffvalues.hpp"
#include "packetcreator.hpp"
#include "attackinfo.hpp"
#include "world.hpp"
#include "buffstat_constants.hpp"
#include "constants.hpp"

void Player::handle_use_attack(signed char attack_type)
{
	PlayerAttackInfo attack;

	attack.item_id_ = 0;
	attack.player_id_ = get_id();

	skip_bytes(1); // portal count
	attack.info_byte_ = read<signed char>();

	attack.skill_id_ = read<int>();

	if (attack.skill_id_ > 0)
	{
		if (skills_.find(attack.skill_id_) != skills_.end())
		{
			attack.skill_level_ = skills_[attack.skill_id_].level_;
		}
		else
		{
			attack.skill_level_ = 1;
		}
	}
	else
	{
		attack.skill_level_ = 0;
	}

	switch (attack.skill_id_)
	{
	case 2121001: // Big Bang
	case 2221001: // Big Bang
	case 2321001: // Big Bang
	case 5201002: // Grenade
	case 5101004: // Corkscrew Blow
	case 21120006: // Combo Tempest
		attack.charge_ = read<int>();
		break;
	}

	skip_bytes(8);
	signed char display = read<signed char>();
	attack.direction_ = read<signed char>();
	attack.stance_ = read<signed char>();

	// meso explosion
	if (attack.skill_id_ == 4211006)
	{
		// TODO
	}

	skip_bytes(1);
	attack.weapon_speed_ = read<signed char>();
	skip_bytes(4);

	short star_slot = 0;

	if (attack_type == attack_type_constants::kRanged)
	{
		star_slot = read<short>();
		skip_bytes(2); // could be cash star slot?
		skip_bytes(1);

		switch (attack.skill_id_)
		{
		case 3121004: // Hurricane
		case 3221001: // Pierce
		case 5221004: // Rapid Fire
			skip_bytes(4);
			break;
		}
	}

	int mob_object_id;

	signed char targets = (attack.info_byte_ >> 4 & 0xF);
	signed char hits = (attack.info_byte_ & 0xF);

	for (signed char i = 0; i < targets; ++i)
	{
		mob_object_id = read<int>();
		skip_bytes(14);

		int total_damage = 0;
		std::vector<int> damages;

		for (signed char j = 0; j < hits; ++j)
		{
			int damage = read<int>();

			if (attack.skill_id_ == 3221007) // Snipe
			{
				damage += 0x80000000; // Critical
			}

			damages.push_back(damage);
			total_damage += damage;
		}

		// warning system

		if (total_damage > 50000 && !get_is_gm() && level_ < 120)
		{
			std::string text = "[WARNING] player: " + name_ + " made damage : " + std::to_string(total_damage) + " with skill : " + std::to_string(attack.skill_id_);

			auto players = World::get_instance()->get_players();

			for (auto &it : *players)
			{
				Player *player = it.second;
				if (player->get_is_gm())
				{
					{
						PacketCreator packet;
						packet.ShowMessage(text, 6);
						player->send_packet(&packet);
					}
				}
			}
		}

		Mob *mob = map_->get_mob(mob_object_id);

		if (mob && !mob->is_dead())
		{
			// mage mp eater

			int mp_eater = 0;

			switch (job_)
			{
			case 210:
			case 211:
			case 212:
				mp_eater = 2100000;
				break;
			case 220:
			case 221:
			case 222:
				mp_eater = 2200000;
				break;
			case 230:
			case 231:
			case 232:
				mp_eater = 2300000;
				break;
			}

			if (mp_eater != 0 && !mob->is_boss() && mob->get_mp())
			{
				if (skills_.find(mp_eater) != skills_.end())
				{
					auto & skill = skills_[mp_eater];
					int level = skill.level_;

					if (level)
					{
						int emp = mob->get_max_mp() * (20 + level) / 100;

						if (emp > mob->get_mp())
						{
							emp = mob->get_mp();
						}

						mob->set_mp(mob->get_mp() - emp);
						add_mp(emp);
					}
				}
			}

			mob->take_damage(this, total_damage);
		}

		attack.damages_[mob_object_id] = damages;

		// rapid fire check
		if (attack.skill_id_ != 5221004)
		{
			skip_bytes(4);
		}
	}

	// ranged only

	if (star_slot > 0)
	{
		Inventory *inventory = get_inventory(kInventoryConstantsTypesUse);

		if (!inventory)
		{
			return;
		}

		std::shared_ptr<Item> item = inventory->get_item_by_slot(static_cast<signed char>(star_slot));

		if (item)
		{
			attack.item_id_ = item->get_item_id();

			if (!is_buff_stat_active(buffstat_constants::kSoulArrow) && // Soul Arrow
				!is_buff_id_active(4111009)) // Shadow Stars
			{
				inventory->remove_item_by_slot(static_cast<signed char>(star_slot), hits);
			}
		}
	}

	switch (attack.skill_id_)
	{
	case 21120007: // Combo Barrier
	case 21100005: // Combo Drain
	case 21120006: // Combo Tempest
	{
		aran_combo_value_ = 0;
		{
			PacketCreator packet;
			packet.ShowAranCombo(aran_combo_value_);
			send_packet(&packet);
		}
		break;
	}
	}

	// handle energy charge

	if (skills_.find(5110001) != skills_.end())
	{
		for (signed char i = 0; i < hits; ++i)
		{
			if (energy_bar_ < 10000)
			{
				energy_bar_ += 102;
			}
			if (energy_bar_ > 10000)
			{
				energy_bar_ = 10000;
			}
			{
				PacketCreator packet;
				packet.GiveEnergyCharge(0, energy_bar_, 10000);
				send_packet(&packet);
			}
		}
	}

	// handle combo

	if (is_buff_stat_active(buffstat_constants::kCombo) && targets > 0)
	{
		signed char max_orb_count = 6;
		int combo_skill_id = 1111002;
		int advanced_combo_skill_id = 1120003;
		int combo_time = 1000 * 100;

		switch (job_)
		{
		case 1110:
		case 1111:
		case 1112:
			combo_skill_id = 11111001;
			advanced_combo_skill_id = 11110005;
			break;
		}

		if (skills_.find(advanced_combo_skill_id) != skills_.end())
		{
			max_orb_count = 11;
		}

		switch (attack.skill_id_)
		{
		case 1111003:
			crusader_combo_value_ -= 2;
			break;
		case 1111005:
			crusader_combo_value_ -= 1;
			break;
		}

		if (attack.skill_id_ == 1111003 || attack.skill_id_ == 1111005)
		{
			Values m;
			auto vals = m.get_values();
			vals->push_back(Value(buffstat_constants::kCombo, crusader_combo_value_));
			{
				PacketCreator packet;
				packet.ShowPlayerBuff(&m, combo_skill_id, combo_time);
				send_packet(&packet);
			}
			{
				PacketCreator packet;
				packet.ShowMapBuff(id_, &m);
				map_->send_packet(&packet, this);
			}
		}

		if (crusader_combo_value_ < max_orb_count)
		{
			crusader_combo_value_ += 1;

			Values m;
			auto vals = m.get_values();
			vals->push_back(Value(buffstat_constants::kCombo, crusader_combo_value_));
			{
				PacketCreator packet;
				packet.ShowPlayerBuff(&m, combo_skill_id, combo_time);
				send_packet(&packet);
			}
			{
				PacketCreator packet;
				packet.ShowMapBuff(id_, &m);
				map_->send_packet(&packet, this);
			}
		}
	}

	if (attack.skill_id_ > 0)
	{
		apply_attack_skill_costs(attack.skill_id_, attack.skill_level_);
	}

	// nothing to send if there are no other players in the map

	if (map_->get_players()->size() > 1)
	{
		{
			PacketCreator packet;
			packet.PlayerAttack(attack_type, attack);
			map_->send_packet(&packet, this);
		}
	}

	mob_object_id = 0;

	for (auto &it : attack.damages_)
	{
		mob_object_id = it.first;

		Mob *mob = map_->get_mob(mob_object_id);

		if (mob && !mob->is_dead() && mob->get_hp() == 0)
		{
			map_->kill(mob);
		}
	}
}
