//

#include "player.hpp"

#include "map.hpp"
#include "session.hpp"
#include "packetcreator.hpp"

void Player::handle_player_movement()
{
	skip_bytes(1);
	skip_bytes(4);

	short start_position_x = read<short>();
	short start_position_y = read<short>();

	position_x_ = start_position_x;
	position_y_ = start_position_y;

	short player_position_x = 0;
	short player_position_y = 0;
	short player_foothold = 0;
	signed char player_stance = 0;

	movement_handler(player_position_x, player_position_y, player_foothold, player_stance);

	if (player_position_x != 0)
	{
		position_x_ = player_position_x;
	}

	if (player_position_y != 0)
	{
		position_y_ = player_position_y;
	}

	if (player_foothold != 0)
	{
		foothold_ = player_foothold;
	}

	if (player_stance != 0)
	{
		stance_ = player_stance;
	}

	// only send the packet if there are atleast 2 players in the map

	auto map_players = map_->get_players();
	if (map_players->size() >= 2)
	{
		// send a packet to all players in the map except the player to show the movement of the player
		// exclude some bytes from being sent as that data is not used for the packet

		constexpr const int excluded_bytes = (9 + 2);
		int size = (recv_length_ - excluded_bytes);
		if (size < 1)
		{
			return;
		}
		{
			PacketCreator packet;
			packet.ShowPlayerMovement(id_, start_position_x, start_position_y, session_->get_receive_buffer() + excluded_bytes, size);
			map_->send_packet(&packet, this);
		}
	}
}
