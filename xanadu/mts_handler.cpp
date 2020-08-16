//

#include "player.hpp"

#include "map.hpp"

void Player::handle_enter_mts()
{
	// warp the player to the free market map
	set_map(910000000);
}
