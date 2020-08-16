//

#include "player.hpp"

void Player::movement_handler(short &position_x, short &position_y, short &foothold, signed char &stance)
{
	signed char actions_amount = read<signed char>();

	for (signed char i = 0; i < actions_amount; ++i)
	{
		signed char action = read<signed char>();

		switch (action)
		{
		case 0:
		case 5:
		case 17:
			position_x = read<short>();
			position_y = read<short>();
			skip_bytes(2);
			skip_bytes(2);
			skip_bytes(2);
			stance = read<signed char>();
			foothold = read<short>();
			break;
		case 15:
			position_x = read<short>();
			position_y = read<short>();
			skip_bytes(2);
			skip_bytes(2);
			skip_bytes(2);
			skip_bytes(2);
			stance = read<signed char>();
			foothold = read<short>();
			break;
		case 1:
		case 2:
		case 6:
		case 12:
		case 13:
		case 16:
		case 18:
		case 19:
		case 20:
		case 22:
			position_x = read<short>();
			position_y = read<short>();
			stance = read<signed char>();
			foothold = read<short>();
			break;
		case 3:
		case 4:
		case 7:
		case 8:
		case 9:
		case 11:
			position_x = read<short>();
			position_y = read<short>();
			skip_bytes(2);
			foothold = read<short>();
			stance = read<signed char>();
			break;
		case 10:
			skip_bytes(1);
			break;
		case 14:
			skip_bytes(2);
			skip_bytes(2);
			skip_bytes(2);
			stance = read<signed char>();
			foothold = read<short>();
			break;
		default:
			break;
		}
	}
}
