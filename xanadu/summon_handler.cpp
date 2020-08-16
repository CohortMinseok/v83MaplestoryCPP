//

#include "player.hpp"

#include "map.hpp"
#include "mob.hpp"
#include "summon.hpp"
#include "session.hpp"
#include "packetcreator.hpp"

void Player::handle_summon_movement()
{
	int summon_object_id = read<int>();
	Summon *summon = get_summon_by_id(summon_object_id);
	if (!summon)
	{
		return;
	}

	short start_position_x = read<short>();
	short start_position_y = read<short>();

	summon->position_x_ = start_position_x;
	summon->position_y_ = start_position_y;

	short summon_position_x = 0;
	short summon_position_y = 0;
	short summon_foothold = 0;
	signed char summon_stance = 0;

	movement_handler(summon_position_x, summon_position_y, summon_foothold, summon_stance);

	if (summon_position_x != 0 || summon_position_y != 0 || summon_foothold != 0 || summon_stance != 0)
	{
		summon->position_x_ = summon_position_x;
		summon->position_y_ = summon_position_y;
	}

	// only send if there are other players in the map
	if (map_->get_players()->size() > 1)
	{
		// exclude header
		constexpr const int excluded_bytes = (8 + 2);
		int size = (recv_length_ - excluded_bytes);
		if (size < 1)
		{
			return;
		}
		{
			PacketCreator packet;
			packet.MoveSummon(id_, summon_object_id, start_position_x, start_position_y, session_->get_receive_buffer() + excluded_bytes, recv_length_ - excluded_bytes);
			map_->send_packet(&packet, this);
		}
	}
}

void Player::handle_damage_mob_summon()
{
	int summon_object_id = read<int>();
	Summon *summon = get_summon_by_id(summon_object_id);
	if (!summon)
	{
		return;
	}

	skip_bytes(5);

	std::unordered_map<int, int> damages;
	signed char targets = read<signed char>();

	for (signed char i = 0; i < targets; ++i)
	{
		int mob_object_id = read<int>();
		skip_bytes(14);
		int damage = read<int>();

		damages[mob_object_id] = damage;
	}

	// nothing to send if there are no other players in the map
	if (map_->get_players()->size() > 1)
	{
		{
			PacketCreator packet;
			packet.DamageMobSummon(this, summon_object_id, 4, damages);
			map_->send_packet(&packet, this);
		}
	}

	for (auto &it : damages)
	{
		int mob_object_id = it.first;
		int damage = it.second;

		Mob *mob = map_->get_mob(mob_object_id);

		if (mob && !mob->is_dead())
		{
			mob->take_damage(this, damage);

			if (mob->get_hp() == 0)
			{
				map_->kill(mob);
			}
		}
	}
}

void Player::handle_puppet_damage()
{
	int puppet_object_id = read<int>();
	Summon *summon = get_summon_by_id(puppet_object_id);
	if (!summon)
	{
		return;
	}

	signed char unk = read<signed char>();
	int damage = read<int>();
	int mob_id = read<int>();

	summon->hp_ = (summon->hp_ - damage);
	{
		PacketCreator packet;
		packet.DamageSummon(id_, summon->skill_id_, damage, unk, mob_id);
		map_->send_packet(&packet);
	}
	if (summon->hp_ <= 0)
	{
		cancel_buff(summon->skill_id_, false);
		remove_summon();
	}
}
