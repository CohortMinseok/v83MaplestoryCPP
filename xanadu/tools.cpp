//

#include "tools.hpp"

#include <ctime>
#include <random>

namespace tools
{
	int random_int(int min, int max)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<std::mt19937::result_type> dist(min, max); // distribution in range [min, max]

		return dist(mt);
	}

	int get_distance(short pos1_x, short pos1_y, short pos2_x, short pos2_y)
	{
		return static_cast<int>((pos1_x - pos2_x) * (pos1_x - pos2_x) + (pos1_y - pos2_y) * (pos1_y - pos2_y));
	}

	long long time_to_tick()
	{
		time_t time_stamp = time(nullptr); // unix time in seconds
		time_stamp *= 1000; // convert to milliseconds
		time_stamp *= 10000; // convert to 100-nanonseconds
		time_stamp += 116444592000000000; // change timezone from UTC to EDT
		return time_stamp;
	}

	bool is_star(int item_id)
	{
		return (item_id / 10000 == 207 || item_id / 10000 == 233);
	}

	signed char get_inventory_id_from_item_id(int item_id)
	{
		return (item_id / 1000000);
	}

	// skill related functions

	bool is_monster_riding(int skill_id)
	{
		switch (skill_id)
		{
		case 1004: // Monster Rider
		case 5221006: // Battle Ship
			return true;
		default:
			return false;
		}
	}

	bool skill_id_is_summon(int skill_id)
	{
		switch (skill_id)
		{
		case 3111002: // Ranger Puppet
		case 3211002: // Sniper Puppet
		case 3120012: // Bowmaster Elite Puppet
		case 3220012: // Marksman Elite Puppet
		case 13111004: // Wind Archer Puppet
		case 1321007: // Beholden
		case 2121005: // Ifrit
		case 2221005: // Elquines
		case 2321003: // Bahamut
		case 3101007: // Silver Hawk
		case 3201007: // Golden Eagle
		case 3111005: // Eagle Eye
		case 3211005: // Frostprey
		case 4111007: // Dark Flare
		case 4211007: // Dark Flare
			return true;
		default:
			return false;
		}
	}

	bool skill_is_puppet(int skill_id)
	{
		switch (skill_id)
		{
		case 3111002:  // Ranger Puppet
		case 3211002:  // Sniper Puppet
		case 3120012:  // Bowmaster Elite Puppet
		case 3220012:  // Marksman Elite Puppet
		case 13111004: // Wind Archer Puppet
			return true;
		default:
			return false;
		}
	}

	bool skill_id_is_special_skill(int skill_id)
	{
		switch (skill_id)
		{
		case 2321001: // Big Bang
		case 2111002: // Explosion
		case 3121004: // Hurricane
		case 3221001: // Piercing Arrow
		case 5221004: // Rapid Fire
			return true;
		default:
			return false;
		}
	}

	short get_job_id_from_skill_id(int skill_id)
	{
		return (skill_id / 10000);
	}

	bool is_fourth_job_skill(int skill_id)
	{
		if (skill_id / 10000 % 100 && skill_id / 10000 % 10 == 2)
			return true;
		return false;
	}

}
