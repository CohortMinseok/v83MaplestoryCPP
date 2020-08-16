//

#include "effect.hpp"

#include <functional>

#include "timer.hpp"
#include "skill_data_provider.hpp"
#include "party.hpp"
#include "skill_data.hpp"
#include "skill_level_data.hpp"
#include "player.hpp"
#include "packetcreator.hpp"
#include "item.hpp"
#include "inventory.hpp"
#include "item_data.hpp"
#include "item_data_provider.hpp"
#include "map.hpp"
#include "world.hpp"
#include "timer.hpp"
#include "summon.hpp"
#include "buffstat_constants.hpp"
#include "constants.hpp"

std::unordered_map<int, Effect *> Effect::item_effects_;
std::unordered_map<int, std::unordered_map<signed char, Effect *>> Effect::skill_effects_;

Effect *Effect::get_item_effect(int id)
{
	if (item_effects_.find(id) != item_effects_.end())
	{
		return item_effects_[id];
	}

	ItemData *item = ItemDataProvider::get_instance()->get_item_data(id);

	if (!item)
	{
		return nullptr;
	}

	Effect *eb = new Effect(id, item);
	item_effects_[id] = eb;
	return eb;
}

Effect *Effect::get_skill_effect(int id, signed char level)
{
	if (skill_effects_.find(id) != skill_effects_.end() && skill_effects_[id].find(level) != skill_effects_[id].end())
	{
		return skill_effects_[id][level];
	}

	SkillData *skill = SkillDataProvider::get_instance()->get_skill_data(id);

	if (!skill)
	{
		return nullptr;
	}

	SkillLevelData *seffect = skill->get_skill_level(level);

	if (!seffect)
	{
		return nullptr;
	}

	Effect *eb = new Effect(id, seffect);
	skill_effects_[id][level] = eb;
	return eb;
}

Effect::Effect(int id, ItemData *data) :
	level_(0),
	id_(-id),
	skill_data_(nullptr),
	item_data_(data)
{
	auto values = values_.get_values();

	if (data->watk != 0)
	{
		values->push_back(Value(buffstat_constants::kWatk, data->watk));
	}
	if (data->wdef != 0)
	{
		values->push_back(Value(buffstat_constants::kWdef, data->wdef));
	}
	if (data->matk != 0)
	{
		values->push_back(Value(buffstat_constants::kMatk, data->matk));
	}
	if (data->mdef != 0)
	{
		values->push_back(Value(buffstat_constants::kMdef, data->mdef));
	}
	if (data->acc != 0)
	{
		values->push_back(Value(buffstat_constants::kAcc, data->acc));
	}
	if (data->avo != 0)
	{
		values->push_back(Value(buffstat_constants::kAvoid, data->avo));
	}
	if (data->speed != 0)
	{
		values->push_back(Value(buffstat_constants::kSpeed, data->speed));
	}
	if (data->jump != 0)
	{
		values->push_back(Value(buffstat_constants::kJump, data->jump));
	}
}

Effect::Effect(int id, SkillLevelData *data) :
	level_(data->id),
	id_(id),
	skill_data_(data),
	item_data_(nullptr)
{
	auto values = values_.get_values();
	if (data->weapon_atk != 0)
	{
		values->push_back(Value(buffstat_constants::kWatk, data->weapon_atk));
	}
	if (data->weapon_def != 0)
	{
		values->push_back(Value(buffstat_constants::kWdef, data->weapon_def));
	}
	if (data->magic_atk != 0)
	{
		values->push_back(Value(buffstat_constants::kMatk, data->magic_atk));
	}
	if (data->magic_def != 0)
	{
		values->push_back(Value(buffstat_constants::kMdef, data->magic_def));
	}
	if (data->accuracy != 0)
	{
		values->push_back(Value(buffstat_constants::kAcc, data->accuracy));
	}
	if (data->avoid != 0)
	{
		values->push_back(Value(buffstat_constants::kAvoid, data->avoid));
	}
	if (data->speed != 0)
	{
		values->push_back(Value(buffstat_constants::kSpeed, data->speed));
	}
	if (data->jump != 0)
	{
		values->push_back(Value(buffstat_constants::kJump, data->jump));
	}

	switch (id)
	{
	case 2001002: // Mage Magic Guard
		values->push_back(Value(buffstat_constants::kMagicGuard, skill_data_->x));
		break;
	case 2301003: // Cleric Invincible
		values->push_back(Value(buffstat_constants::kInvincible, skill_data_->x));
		break;
	case 2121004: // FPArchMage Infinity
	case 2221004: // ILArchMage Infinity
	case 2321004: // Bishop Infinity
		values->push_back(Value(buffstat_constants::kInfinityBuff, skill_data_->x));
		values->push_back(Value(buffstat_constants::kStance, skill_data_->prop));
		break;
	case 2311003: // Priest Holy Symbol
	case 9101002: // SuperGM Holy Symbol
		values->push_back(Value(buffstat_constants::kHolySymbol, skill_data_->x));
		break;
	case 4201011: // ChiefBandit Meso Guard
		values->push_back(Value(buffstat_constants::kMesoGuard, skill_data_->x));
		break;
	case 4211003: // ChiefBandit Pickpocket
		values->push_back(Value(buffstat_constants::kPickPocket, skill_data_->x));
		break;
	case 4111002: // Hermit Shadow Partner
	case 4211008: // Chief Bandit Shadow Partner
		values->push_back(Value(buffstat_constants::kShadowPartner, skill_data_->x));
		break;
	case 4001003: // Thief Darksight
		values->push_back(Value(buffstat_constants::kDarksight, skill_data_->x));
		break;
	case 1101004: // Fighter Weapon Booster
	case 1201004: // Page Weapon Booster
	case 1301004: // Spearman Weapon Booster
	case 2111005: // FP Mage Spell Booster
	case 2211005: // IL Mage Spell Booster
	case 2311006: // Priest Magic Booster
	case 3101002: // Hunter Bow Booster
	case 3201002: // Crossbowman Crossbow Booster
	case 4101003: // Assassin Claw Booster
	case 4201002: // Bandit Dagger Booster
	case 5101006: // Brawler Knuckle Booster
	case 5201003: // Gunslinger Gun Booster
		values->push_back(Value(buffstat_constants::kBooster, skill_data_->x));
		break;
	case 1121000:
	case 1221000:
	case 1321000:
	case 2121000:
	case 2221000:
	case 2321000:
	case 3121000:
	case 3221000:
	case 4121000:
	case 4221000:
	case 4341000:
	case 5121000: // Pirate Maple Warrior
	case 5221000: // Pirate Maple Warrior
		values->push_back(Value(buffstat_constants::kMapleWarrior, skill_data_->x));
		break;
	case 1211004:  // Flame Charge
	case 1211006:  // Blizzard Charge
	case 1211008:  // Lightning Charge
	case 1221004:  // Holy Charge
		values->push_back(Value(buffstat_constants::kWkCharge, skill_data_->x));
		break;
	case 1301007: // Hyper Body
	case 9101008: // SuperGM Hyper Body
		values->push_back(Value(buffstat_constants::kHyperbodyHp, 0));
		values->push_back(Value(buffstat_constants::kHyperbodyMp, 0));
		break;
	case 9101004: // SuperGM Hide
		skill_data_->time = 2100000000;
		values->push_back(Value(buffstat_constants::kDarksight, skill_data_->x));
		break;
	case 3101004: // Soul Arrow
	case 3201004:
		values->push_back(Value(buffstat_constants::kSoulArrow, skill_data_->x));
		break;
	case 3121002:  // Bow Master Sharp Eyes
	case 3221002:  // Marksman Sharp Eyes
		values->push_back(Value(buffstat_constants::kSharpEyes, (skill_data_->x << 8 | skill_data_->y)));
		break;
	case 3111000: // Concentrate
	case 3211000:
		values->push_back(Value(buffstat_constants::kConcentrate, skill_data_->x));
		break;
	case 1111002: // Combo
		values->push_back(Value(buffstat_constants::kCombo, 1));
		break;
	case 1121002: // Power Stance
	case 1221002: // Power Stance
	case 1321002: // Power Stance
	case 5321010: // Pirate's Spirit
		values->push_back(Value(buffstat_constants::kStance, skill_data_->prop));
		break;
	//case 5110001: // Energy Charge
	//	values->push_back(Value(buffstat_constants_position_2::kEnergyCharge, 0, 2));
	//	break;
	case 1004: // Monster Rider
	case 5221006: // Battle Ship
		values->push_back(Value(buffstat_constants_position_2::kMonsterRiding, id_, 2));
		break;
	case 5111005: // Transformation
	case 5121003: // Super Transformation
	case 15111002: // Transformation
		values->push_back(Value(buffstat_constants::kMorph, 1000));
		break;
	case 5001005: // dash
	case 15001003: // dash
		values->push_back(Value(buffstat_constants_position_2::kDashSpeed, skill_data_->x, 2));
		values->push_back(Value(buffstat_constants_position_2::kDashJump, skill_data_->y, 2));
		break;
	case 5121009: // speed infusion
	case 5221010: // speed infusion
	case 15111005: // speed infusion
		values->push_back(Value(buffstat_constants_position_2::kSpeedInfusion, skill_data_->x, 2));
		break;
	}
}

int Effect::get_id()
{
	return id_;
}

Values *Effect::get_values()
{
	return &values_;
}

void handle_stop_cooldown(int player_id, int skill_id, const std::error_code &ec)
{
	if (ec)
	{
		return;
	}

	Player *player = World::get_instance()->GetPlayerById(player_id);
	if (!player)
	{
		return;
	}

	// timer
	timer::Id timer_id(timer::kTimerTypesCoolDownTimer, skill_id);

	auto timers = player->get_timers();
	if (timers->find(timer_id) != timers->end())
	{
		timers->erase(timer_id);
	}
	{
		PacketCreator packet;
		packet.SendCooldown(skill_id, 0);
		player->send_packet(&packet);
	}
}

void Effect::use_attack_skill(Player *player)
{
	if (skill_data_->hp_cost > 0)
	{
		player->add_hp(-skill_data_->hp_cost);
	}
	if (skill_data_->mp_cost > 0)
	{
		player->add_mp(-skill_data_->mp_cost);
	}
	if (skill_data_->cooldown_time > 0)
	{
		{
			PacketCreator packet;
			packet.SendCooldown(id_, skill_data_->cooldown_time);
			player->send_packet(&packet);
		}
		// timer
		timer::Id timer_id(timer::kTimerTypesCoolDownTimer, id_);

		std::shared_ptr<Timer> timer(new Timer(skill_data_->cooldown_time));
		timer->get_object()->async_wait(std::bind(&handle_stop_cooldown, player->get_id(), id_, std::placeholders::_1));
		auto timers = player->get_timers();
		timers->operator[](timer_id) = timer;
	}
}

void handle_stop_buff(int player_id, int buff_id, const std::error_code &ec)
{
	if (ec)
	{
		return;
	}

	Player *player = World::get_instance()->GetPlayerById(player_id);
	if (!player)
	{
		return;
	}
	player->cancel_buff(buff_id, true);
}

void Effect::use_skill(Player *player)
{
	{
		PacketCreator packet;
		packet.ShowBuffEffect(player->get_id(), 1, id_, level_);
		player->get_map()->send_packet(&packet, player);
	}
	if (id_ == 1121010)
	{
		int combo_skill_id = 1111002;
		switch (player->get_job())
		{
		case 1110:
		case 1111:
		case 1112:
			combo_skill_id = 11111001;
			break;
		}

		player->set_crusader_combo_value(player->get_crusader_combo_value() - 10);

		Values m;
		auto vals = m.get_values();
		vals->push_back(Value(buffstat_constants::kCombo, player->get_crusader_combo_value()));
		{
			PacketCreator packet;
			packet.ShowPlayerBuff(&m, combo_skill_id, 1000 * 100);
			player->send_packet(&packet);
		}
		{
			PacketCreator packet;
			packet.ShowMapBuff(player->get_id(), &m);
			player->get_map()->send_packet(&packet);
		}
	}

	if (skill_data_->hp_cost > 0)
	{
		player->add_hp(-skill_data_->hp_cost);
	}

	if (skill_data_->mp_cost > 0)
	{
		player->add_mp(-skill_data_->mp_cost);
	}

	if (skill_data_->required_item_id > 0)
	{
		player->remove_item(skill_data_->required_item_id, skill_data_->required_item_count);
	}

	if (skill_data_->ltx != 0 && skill_data_->rbx != 0 && player->get_party())
	{
		auto players = player->get_party()->get_players();
		for (auto it : *players)
		{
			Player *target_player = it.second;

			if (target_player != player && target_player->get_map() == player->get_map())
			{
				if (target_player->get_position_x() > player->get_position_x() + skill_data_->ltx &&
					target_player->get_position_y() > player->get_position_y() + skill_data_->lty &&
					target_player->get_position_x() < player->get_position_x() + skill_data_->rbx &&
					target_player->get_position_y() < player->get_position_y() + skill_data_->rby)
				{
					if (target_player->get_hp() > 0)
					{
						{
							PacketCreator packet;
							packet.ShowBuffEffect(target_player->get_id(), 2, id_, level_);
							target_player->get_map()->send_packet(&packet, target_player);
						}
						{
							PacketCreator packet;
							packet.ShowBuffEffect(0, 2, id_, level_);
							target_player->send_packet(&packet);
						}
						if (id_ == 1301007 || id_ == 9101008) // Hyper Body - SuperGM Hyper Body
						{
							int max_hp_value = (target_player->get_max_hp() / 100 * skill_data_->x);
							int max_mp_value = (target_player->get_max_mp() / 100 * skill_data_->x);
							target_player->set_hyperbody_max_hp(max_hp_value);
							target_player->set_hyperbody_max_mp(max_mp_value);
						}

						bool is_buff = (skill_data_->time > 0 && values_.get_values()->size() > 0);
						if (is_buff)
						{
							target_player->add_buff(this);

							{
								PacketCreator packet;
								packet.ShowPlayerBuff(&values_, id_, skill_data_->time);
								target_player->send_packet(&packet);
							}
							// timer
							timer::Id timer_id(timer::kTimerTypesBuffTimer, id_);
							std::shared_ptr<Timer> timer(new Timer(skill_data_->time / 1000));
							timer->get_object()->async_wait(std::bind(&handle_stop_buff, target_player->get_id(), id_, std::placeholders::_1));
							auto timers = target_player->get_timers();
							timers->operator[](timer_id) = timer;
						}
					}
				}
			}
		}
	}

	if (tools::is_monster_riding(id_))
	{
		skill_data_->time = 2100000000;
		int mount_id = 0;

		if (id_ == 1004) // 1004 is the monster rider skill id
		{
			if (player->get_inventory(kInventoryConstantsTypesEquipped)->get_item_by_slot(kItemConstantsEquippedSlotsTamingMobSaddle)
				&& player->get_inventory(kInventoryConstantsTypesEquipped)->get_item_by_slot(kItemConstantsEquippedSlotsTamingMob))
			{
				mount_id = player->get_inventory(kInventoryConstantsTypesEquipped)->get_item_by_slot(kItemConstantsEquippedSlotsTamingMob)->get_item_id();
			}

			if (mount_id == 0)
			{
				return;
			}
		}

		if (id_ == 5221006)
		{
			mount_id = 1932000;
		}

		player->set_mount_item_id(mount_id);
		player->set_mount_skill_id(id_);
	}

	if (skill_data_->cooldown_time > 0)
	{
		{
			PacketCreator packet;
			packet.SendCooldown(id_, skill_data_->cooldown_time);
			player->send_packet(&packet);
		}
		// timer
		timer::Id timer_id(timer::kTimerTypesCoolDownTimer, id_);
		std::shared_ptr<Timer> timer(new Timer(skill_data_->cooldown_time));
		timer->get_object()->async_wait(std::bind(&handle_stop_cooldown, player->get_id(), id_, std::placeholders::_1));
		auto timers = player->get_timers();
		timers->operator[](timer_id) = timer;
	}

	if (id_ == 1301007 || id_ == 9101008) // Hyper Body - GM Hyper Body
	{
		int max_hp_value = (player->get_max_hp() / 100 * skill_data_->x);
		int max_mp_value = (player->get_max_mp() / 100 * skill_data_->x);
		player->set_hyperbody_max_hp(max_hp_value);
		player->set_hyperbody_max_mp(max_mp_value);
	}

	bool is_buff = (skill_data_->time > 0 && values_.get_values()->size() > 0);

	if (is_buff)
	{
		player->add_buff(this);
		{
			PacketCreator packet;
			if (tools::is_monster_riding(id_))
			{
				packet.ShowPlayerBuff(&values_, player->get_mount_item_id(), id_);
			}
			else if (id_ == 5001005 || id_ == 15001003) // dash
			{
				packet.GiveDashOrInfusionBuff(&values_, id_, skill_data_->time / 1000, true);
			}
			else if (id_ == 5121009 || id_ == 5221010 || id_ == 15111005) // speed infusion
			{
				packet.GiveDashOrInfusionBuff(&values_, id_, skill_data_->time / 1000, false);
			}
			else
			{
				packet.ShowPlayerBuff(&values_, id_, skill_data_->time);
			}
			player->send_packet(&packet);
		}
		// timer
		timer::Id timer_id(timer::kTimerTypesBuffTimer, id_);
		std::shared_ptr<Timer> timer(new Timer(skill_data_->time / 1000));
		timer->get_object()->async_wait(std::bind(&handle_stop_buff, player->get_id(), id_, std::placeholders::_1));
		auto timers = player->get_timers();
		timers->operator[](timer_id) = timer;

		// display buff for the map
		Values m;
		auto vals = m.get_values();

		if (values_.get_value_by_stat(buffstat_constants::kDarksight) != 0)
		{
			vals->push_back(Value(buffstat_constants::kDarksight, 1));
		}
		if (values_.get_value_by_stat(buffstat_constants::kShadowPartner) != 0)
		{
			vals->push_back(Value(buffstat_constants::kShadowPartner, 1));
		}
		if (values_.get_value_by_stat(buffstat_constants_position_2::kMonsterRiding) != 0)
		{
			vals->push_back(Value(buffstat_constants_position_2::kMonsterRiding, 0, 2));
		}
		if (values_.get_value_by_stat(buffstat_constants::kMorph) != 0)
		{
			int test = 0;
			int test_id = 1000;
			if (test_id % 10 == 0)
			{
				 test = test_id + static_cast<int>(player->get_gender());
			}
			else
			{
				test = test_id + 100 * static_cast<int>(player->get_gender());
			}

			vals->push_back(Value(buffstat_constants::kMorph, test));
		}
		if (values_.get_value_by_stat(buffstat_constants_position_2::kSpeedInfusion) != 0)
		{
			vals->push_back(Value(buffstat_constants_position_2::kSpeedInfusion, -1, 2));
		}
		if (values_.get_value_by_stat(buffstat_constants_position_2::kDashJump) != 0)
		{
			vals->push_back(Value(buffstat_constants_position_2::kDashJump, 1, 2));
		}
		if (values_.get_value_by_stat(buffstat_constants_position_2::kDashSpeed) != 0)
		{
			vals->push_back(Value(buffstat_constants_position_2::kDashSpeed, 1, 2));
		}
		if (vals->size() > 0)
		{
			PacketCreator packet;
			if (tools::is_monster_riding(id_))
			{
				packet.ShowMonsterRiding(player->get_id(), player->get_mount_item_id(), id_);
				player->get_map()->send_packet(&packet, player);
			}
			else if (id_ == 5001005 || id_ == 15001003) // dash
			{
				packet.ShowForeignDashOrInfusionBuff(player->get_id(), &m, id_, skill_data_->time / 1000, true);
				player->get_map()->send_packet(&packet);
			}
			else if (id_ == 5121009 || id_ == 5221010 || id_ == 15111005) // speed infusion
			{
				packet.ShowForeignDashOrInfusionBuff(player->get_id(), &m, id_, skill_data_->time, false);
				player->get_map()->send_packet(&packet);
			}
			else
			{
				packet.ShowMapBuff(player->get_id(), &m);
				player->get_map()->send_packet(&packet);
			}
		}
	}
}

void Effect::use_item(Player *player)
{
	if (item_data_->hhp > 0 && player->get_hp() > 0)
	{
		player->add_hp(item_data_->hhp);
	}

	if (item_data_->hmp > 0)
	{
		player->add_mp(item_data_->hmp);
	}

	if (item_data_->hpr > 0)
	{
		player->add_hp(item_data_->hpr * player->get_max_hp() / 100);
	}

	if (item_data_->mpr > 0)
	{
		player->add_mp(item_data_->mpr * player->get_max_mp() / 100);
	}

	if (item_data_->time > 0 && values_.get_values()->size() > 0)
	{
		player->add_buff(this);
		{
			PacketCreator packet;
			packet.ShowPlayerBuff(&values_, id_, item_data_->time);
			player->send_packet(&packet);
		}
		// timer
		std::shared_ptr<Timer> timer(new Timer(item_data_->time / 1000));
		timer::Id timer_id(timer::kTimerTypesBuffTimer, id_);
		auto timers = player->get_timers();
		timers->operator[](timer_id) = timer;
		timer->get_object()->async_wait(std::bind(&handle_stop_buff, player->get_id(), id_, std::placeholders::_1));
	}
}
