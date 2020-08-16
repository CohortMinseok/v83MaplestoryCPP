//

#include "player.hpp"

#include "messenger.hpp"
#include "packetcreator.hpp"
#include "world.hpp"
#include "messenger_constants.hpp"

void Player::handle_messenger_action()
{
	signed char action = read<signed char>();
	switch (action)
	{
	case MessengerReceivePaketActions::kOpen:
	{
		if (!messenger_)
		{
			int messenger_id = read<int>();
			if (messenger_id == 0)
			{
				World::get_instance()->create_messenger(this);
			}
			else
			{
				World::get_instance()->join_messenger(this, messenger_id);
			}
		}
		break;
	}
	case MessengerReceivePaketActions::kLeave:
	{
		if (messenger_)
		{
			if (messenger_->get_players()->size() == 1)
			{
				World::get_instance()->remove_messenger(messenger_->get_id());
			}
			else
			{
				messenger_->delete_member(id_);
				{
					PacketCreator packet;
					packet.MessengerRemovePlayer(get_messenger_position());
					messenger_->send_packet(&packet);
				}
			}

			messenger_ = nullptr;
		}

		break;
	}
	case MessengerReceivePaketActions::kInvite:
	{
		if (messenger_)
		{
			std::string invited_name = read<std::string>();
			Player *target = World::get_instance()->GetPlayerByName(invited_name);
			if (target)
			{
				if (!target->get_messenger())
				{
					{
						PacketCreator packet;
						packet.MessengerInvite(this);
						target->send_packet(&packet);
					}
					{
						PacketCreator packet;
						packet.MessengerNote(invited_name, true);
						send_packet(&packet);
					}
				}
				else
				{
					{
						PacketCreator packet;
						packet.MessengerChat(name_, invited_name + " is already using Maple Messenger.");
						send_packet(&packet);
					}
				}
			}
			else
			{
				{
					PacketCreator packet;
					packet.MessengerNote(invited_name, false);
					send_packet(&packet);
				}
			}
		}
		break;
	}
	case MessengerReceivePaketActions::kDecline:
	{
		std::string inviter_name = read<std::string>();
		Player *target = World::get_instance()->GetPlayerByName(inviter_name);
		if (target)
		{
			{
				PacketCreator packet;
				packet.MessengerDeclined(name_);
				target->send_packet(&packet);
			}
		}
		break;
	}
	case MessengerReceivePaketActions::kChat:
	{
		if (messenger_)
		{
			std::string name = read<std::string>();
			std::string text = read<std::string>();
			{
				PacketCreator packet;
				packet.MessengerChat(name, text);
				messenger_->send_packet(&packet, this);
			}
		}
		break;
	}
	}
}
