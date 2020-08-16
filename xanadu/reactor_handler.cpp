//

#include "player.hpp"

void Player::handle_hit_reactor()
{
	int reactorid = read<int>();
	int pos = read<int>();
	short stance = read<short>();

	/*Reactor *reactor = player->getMap()->getReactor(reactorid);

	if (!reactor)
	{
		return;
	}

	get_map()->getReactors()->hitReactor(reactor, player, stance, pos);*/
}
