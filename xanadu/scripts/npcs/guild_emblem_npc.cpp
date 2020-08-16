//

#include "../player.hpp"

void Player::handle_lea_guild_emblem_npc()
{
	if (get_state() == 0)
	{ 
		send_yes_no("A guild emblem costs 5,000,000 mesos. Do you want to create one?");
	}

	else if (get_state() == 1)
	{
		if (get_selected() == 1)
		{
			if (get_has_guild() && guild_rank_ == 1)
			{
				if (mesos_ >= 5000000)
				{
					add_mesos(-5000000);
					change_guild_emblem();
					set_state(1000);
				}
				else
				{
					send_ok("You don't have have enough mesos to buy a guild emblem.");
				}
			}
			else
			{
				send_ok("You are not in a guild or you are not the guild leader.");
			}
		}
	}
}
