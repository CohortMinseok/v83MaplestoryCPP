//

#include "party.hpp"

#include "channel.hpp"
#include "map.hpp"
#include "party_member.hpp"
#include "player.hpp"
#include "packetcreator.hpp"

// constructor

Party::Party(int party_id, Player *leader) :
id_(party_id),
leader_(leader->get_id())
{
	add_member(leader);
	{
		PacketCreator packet;
		packet.PartyCreated(party_id);
		leader->send_packet(&packet);
	}
}

// destructor

Party::~Party()
{
	for (auto &it : players_)
	{
		Player *player = it.second;
		player->set_party(nullptr);
	}
}

int Party::get_id()
{
	return id_;
}

int Party::get_leader()
{
	return leader_;
}

std::unordered_map<int, Player *> *Party::get_players()
{
	return &players_;
}

std::unordered_map<int, std::unique_ptr<PartyMember>> *Party::get_members()
{
	return &members_;
}

PartyMember *Party::get_member(int player_id)
{
	auto iterator = members_.find(player_id);
	if (iterator != members_.end())
	{
		return iterator->second.get();
	}

	return nullptr;
}

void Party::set_leader(int player_id)
{
	leader_ = player_id;
	{
		PacketCreator packet;
		packet.ChangeLeader(player_id, false);
		send_packet(&packet);
	}
}

void Party::add_member(Player *player)
{
	player->set_party(this);
	players_[player->get_id()] = player;
	members_[player->get_id()].reset(new PartyMember(player->get_id(), player->get_job(), player->get_level(), player->get_channel_id(), player->get_map()->get_id(), player->get_name()));
}

void Party::delete_member(int player_id)
{
	auto iterator_players = players_.find(player_id);
	if (iterator_players != players_.end())
	{
		players_.erase(iterator_players);
	}
	auto iterator_members = members_.find(player_id);
	if (iterator_members != members_.end())
	{
		members_.erase(iterator_members);
	}
}

void Party::remove_member(int player_id)
{
	auto iterator_players = players_.find(player_id);
	if (iterator_players != players_.end())
	{
		players_.erase(iterator_players);
	}
	auto iterator_members = members_.find(player_id);
	if (iterator_members != members_.end())
	{
		PartyMember *member = iterator_members->second.get();
		member->set_channel_id(-2);
		member->set_map_id(999999999);
	}
}

void Party::send_packet(PacketCreator *packet)
{
	for (auto it : players_)
	{
		Player *player = it.second;
		player->send_packet(packet);
	}
}

void Party::set_variable(std::string name, int val)
{
	variables_[name] = val;
}

int Party::get_variable(std::string name)
{
	auto iterator = variables_.find(name);
	if (iterator != variables_.end())
	{
		return variables_[name];
	}
	return -1;
}

void Party::delete_variable(std::string &name)
{
	auto iterator = variables_.find(name);
	if (iterator != variables_.end())
	{
		variables_.erase(iterator);
	}
}

void Party::clear_variables()
{
	variables_.clear();
}
