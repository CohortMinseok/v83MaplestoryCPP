
// regular cab in victoria

void main(Player @player)
{
	int [] maps = {104000000, 102000000, 101000000, 103000000, 100000000};
	int [] prices = {800, 1000, 1000, 1200, 1000};

	if (player.get_state() == 0)
	{
		player.send_simple("Please choose where you would like to go: \r\n#b#L0##m" + maps[0] + "#(" + prices[0] + " mesos)#l \r\n#L1##m" + maps[1] + "#(" + prices[1] + " mesos)#l \r\n#L2##m" + maps[2] + "#(" + prices[2] + " mesos)#l \r\n#L3##m" + maps[3] + "#(" + prices[3] + " mesos)#l \r\n#L4##m" + maps[4] + "#(" + prices[4] + " mesos)#l");
	}

	else if (player.get_state() == 1)
	{
		player.set_npc_variable("city", player.get_selected());
		player.send_yes_no("You don't have anything else to do here, huh? Do you really want to go to #b#m" + maps[player.get_selected()] + "##k? it will cost you #b" + prices[player.get_selected()] + " mesos#k.");
	}

	else if (player.get_state() == 2)
	{
		if (player.get_selected() == YES)
		{
			player.set_selected(player.get_npc_variable("city"));
			if (player.get_mesos() >= prices[player.get_selected()])
			{
				player.add_mesos(-prices[player.get_selected()]);
				player.set_map(maps[player.get_selected()]);
			}
			else
			{
				player.send_ok("You need #b" + prices[player.get_selected()] + " mesos#k to go to #m" + maps[player.get_selected()] + "#.");
			}
		}
		else
		{
			player.send_ok("There's a lot to see in this town, too. Come back and find me when you need to go to a different town.");
		}
	}
	else
	{
		player.set_state(1000);
	}
}
