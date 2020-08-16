
// NLC taxi

void main(Player @player)
{
	player.set_state(299);

	if (player.get_map().get_id() == 600000000)
	{
		player.set_map(682000000);
	}

	else if (player.get_map().get_id() == 682000000)
	{
		player.set_map(600000000);
	}
}
