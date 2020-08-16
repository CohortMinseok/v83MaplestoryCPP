//

#include "../player.hpp"

void Player::handle_heracle_guild_creator_npc()
{
	if (get_state() == 0)
	{
		send_simple("\r\n\t#b#L0#Create a Guild (2,000,000 mesos)#l \r\n\t#L1#Disband your Guild #l");
	}

	else if (get_state() == 1)
	{
		if (get_selected() == 0)
		{
			if (get_has_guild())
			{
				send_ok("You are already in a guild.");
			}
			else
			{
				if (mesos_ < 2000000)
				{
					send_ok("You don't have enough mesos to create a guild.");
				}
				else
				{
					add_mesos(-2000000);
					create_guild();
				}
			}
		}
		else
		{
			if (get_has_guild() && guild_rank_ == 1)
			{
				disband_guild();
				set_state(1000);
			}
			else
			{
				send_ok("You are not in a guild or you are not the guild leader.");
			}
		}
	}
}
