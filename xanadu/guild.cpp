//

#include "guild.hpp"

#include "guild_member.hpp"
#include "player.hpp"
#include "world.hpp"

#include "Poco\Data\RecordSet.h"

// constructor

Guild::Guild(int id, std::string name) :
	logo_color_(0),
	logo_background_color_(0),
	logo_(0),
	logo_background_(0),
	capacity_(100),
	points_(0),
	alliance_id_(0),
	id_(id),
	name_(name),
	notice_("Welcome to the Guild!"),
	rank1_("Master"),
	rank2_("Jr. Master"),
	rank3_("Member"),
	rank4_("Member"),
	rank5_("Member")
{
}

// destructor

Guild::~Guild()
{
	for (auto &it : players_)
	{
		Player *player = it.second;
		player->set_guild(0);
		player->set_guild_rank(5);
	}
}

signed char Guild::get_logo_color()
{
	return logo_color_;
}

signed char Guild::get_logo_background_color()
{
	return logo_background_color_;
}

short Guild::get_logo()
{
	return logo_;
}

short Guild::get_logo_background()
{
	return logo_background_;
}

int Guild::get_capacity()
{
	return capacity_;
}

int Guild::get_points()
{
	return points_;
}

int Guild::get_alliance_id()
{
	return alliance_id_;
}

int Guild::get_id()
{
	return id_;
}

std::string Guild::get_name()
{
	return name_;
}

std::string Guild::GetNotice()
{
	return notice_;
}

std::string Guild::GetRank1()
{
	return rank1_;
}

std::string Guild::GetRank2()
{
	return rank2_;
}

std::string Guild::GetRank3()
{
	return rank3_;
}

std::string Guild::GetRank4()
{
	return rank4_;
}

std::string Guild::GetRank5()
{
	return rank5_;
}

std::unordered_map<int, std::unique_ptr<GuildMember>> *Guild::get_members()
{
	return &members_;
}

void Guild::SetLogoColor(signed char color_, bool init)
{
	logo_color_ = color_;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET logo_color = " << static_cast<int>(logo_color_)
			<< " WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::SetLogoBackgroundColor(signed char background_color, bool init)
{
	logo_background_color_ = background_color;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET logo_bg_color = " << static_cast<int>(logo_background_color_)
			<< " WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::SetLogo(short logo, bool init)
{
	logo_ = logo;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET logo = " << static_cast<int>(logo_)
			<< " WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::SetLogoBackground(short background, bool init)
{
	logo_background_ = background;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET logo_bg = " << static_cast<int>(logo_background_)
			<< " WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::set_name(const std::string &name, bool init)
{
	name_ = name;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET guild_name = '" << name_
			<< "' WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::SetNotice(const std::string &notice, bool init)
{
	notice_ = notice;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET notice = '" << notice_
			<< "' WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::SetRank1(std::string rank, bool init)
{
	rank1_ = rank;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET rank1 = '" << rank1_
			<< "' WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::SetRank2(std::string rank, bool init)
{
	rank2_ = rank;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET rank2 = '" << rank2_
			<< "' WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::SetRank3(std::string rank, bool init)
{
	rank3_ = rank;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET rank3 = '" << rank3_
			<< "' WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::SetRank4(std::string rank, bool init)
{
	rank4_ = rank;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET rank4 = '" << rank4_
			<< "' WHERE guild_id = " << id_;
		statement.execute();
	}
}

void Guild::SetRank5(std::string rank, bool init)
{
	rank5_ = rank;

	if (!init)
	{
		// mysql query
		Poco::Data::Session &mysql_session = World::get_instance()->get_mysql_session();
		Poco::Data::Statement statement(mysql_session);
		statement << "UPDATE guilds SET rank5 = '" << rank5_
			<< "' WHERE guild_id = " << id_;
		statement.execute();
	}
}

GuildMember *Guild::GetMember(int player_id)
{
	auto iterator = members_.find(player_id);
	if (iterator != members_.end())
	{
		return iterator->second.get();
	}

	return nullptr;
}

void Guild::AddMember(Player *player)
{
	int player_id = player->get_id();
	players_[player_id] = player;
	members_[player_id].reset(new GuildMember(player_id, player->get_job(), player->get_level(), player->get_guild_rank(), 1, player->get_name()));
}

void Guild::AddMember(GuildMember *member)
{
	members_[member->get_id()].reset(member);
}

void Guild::RemoveMember(int player_id)
{
	auto iterator = players_.find(player_id);
	if (iterator != players_.end())
	{
		players_.erase(iterator);
	}

	GuildMember *member = GetMember(player_id);
	if (member)
	{
		member->set_online_status(0);
	}
}

void Guild::DeleteMember(int player_id)
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

void Guild::send_packet(PacketCreator *packet)
{
	for (auto it : players_)
	{
		Player *target_player = it.second;
		target_player->send_packet(packet);
	}
}

void Guild::send_packet(PacketCreator *packet, Player *excluded_player)
{
	for (auto it : players_)
	{
		Player *target_player = it.second;
		if (target_player != excluded_player)
		{
			target_player->send_packet(packet);
		}
	}
}
