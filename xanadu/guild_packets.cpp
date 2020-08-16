//

#include "packetcreator.hpp"

#include "guild.hpp"
#include "guild_member.hpp"
#include "guild_constants.hpp"
#include "player.hpp"
#include "send_packet_opcodes.hpp"

void PacketCreator::GuildMemberData(GuildMember *member)
{
	write_string(member->get_name(), 13);
	write<int>(member->get_job());
	write<int>(member->get_level());
	write<int>(member->get_guild_rank());
	write<int>(member->get_online_status());
	write<int>(member->get_signature());
	write<int>(member->get_alliance_rank());
}

void PacketCreator::CreateGuild()
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kCreate);
}

void PacketCreator::change_guild_emblem()
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kChangeEmblem);
}

void PacketCreator::GuildInfo(Guild *guild)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kInfo);
	bool valid_guild = (guild ? true : false);
	write<bool>(valid_guild);
	if (!valid_guild)
	{
		return;
	}
	GetGuildInfo(guild);
}

void PacketCreator::GetGuildInfo(Guild *guild)
{
	write<int>(guild->get_id());
	write<std::string>(guild->get_name());
	write<std::string>(guild->GetRank1());
	write<std::string>(guild->GetRank2());
	write<std::string>(guild->GetRank3());
	write<std::string>(guild->GetRank4());
	write<std::string>(guild->GetRank5());
	{
		// guild member data
		auto members = guild->get_members();

		write<signed char>(static_cast<unsigned char>(members->size()));

		for (auto &it : *members)
		{
			write<int>(it.second->get_id());
		}
		for (auto &it : *members)
		{
			GuildMemberData(it.second.get());
		}
	}
	write<int>(guild->get_capacity());
	write<short>(guild->get_logo_background());
	write<signed char>(guild->get_logo_background_color());
	write<short>(guild->get_logo());
	write<signed char>(guild->get_logo_color());
	write<std::string>(guild->GetNotice());
	write<int>(guild->get_points());
	write<int>(guild->get_alliance_id());
}

void PacketCreator::AddGuildPlayer(int guild_id, GuildMember *member)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kAddPlayer);
	write<int>(guild_id);
	write<int>(member->get_id());
	GuildMemberData(member);
}

void PacketCreator::GuildPlayerLeave(int guild_id, int player_id, std::string charname, bool expelled)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(expelled ? GuildSendPacketActions::kPlayerExpelled : GuildSendPacketActions::kPlayerLeave);
	write<int>(guild_id);
	write<int>(player_id);
	write<std::string>(charname);
}

void PacketCreator::InviteGuild(Player *inviter)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kInvite);
	write<int>(inviter->get_guild()->get_id());
	write<std::string>(inviter->get_name());
}

void PacketCreator::ChangeRank(int guild_id, int player_id, int rank)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kChangeRank);
	write<int>(guild_id);
	write<int>(player_id);
	write<signed char>(rank);
}

void PacketCreator::UpdateGuildRanks(Guild *guild)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kUpdateRanks);
	write<int>(guild->get_id());
	write<std::string>(guild->GetRank1());
	write<std::string>(guild->GetRank2());
	write<std::string>(guild->GetRank3());
	write<std::string>(guild->GetRank4());
	write<std::string>(guild->GetRank5());
}

void PacketCreator::UpdateGuildNotice(Guild *guild)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kUpdateNotice);
	write<int>(guild->get_id());
	write<std::string>(guild->GetNotice());
}

void PacketCreator::DisbandGuild(int guild_id)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kDisband);
	write<int>(guild_id);
}

void PacketCreator::ShowGuildEmblem(Guild *guild)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kShowEmblem);
	write<int>(guild->get_id());
	write<short>(guild->get_logo_background());
	write<signed char>(guild->get_logo_background_color());
	write<short>(guild->get_logo());
	write<signed char>(guild->get_logo_color());
}

void PacketCreator::GuildMemberOnline(int guild_id, int player_id, bool online)
{
	write<short>(send_headers::kGUILD_OPERATION);
	write<signed char>(GuildSendPacketActions::kMemberOnline);
	write<int>(guild_id);
	write<int>(player_id);
	write<bool>(online);
}

// start of guild bbs

void PacketCreator::guild_bbs_add_thread()
{
	write<int>(0); // local_thread_id
	write<int>(0); // poster cid
	write<std::string>(""); // name
	write<long long>(0); // timestamp
	write<int>(0); // icon
	write<int>(0); // reply count
}

void PacketCreator::guild_bbs_thread_list(int start)
{
	write<short>(send_headers::kGUILD_BBS_OPERATION);
	write<signed char>(GuildBBSSendPacketActions::kShowThreads);
	// if there is no result
	bool has_entries = false;
	if (!has_entries)
	{
		write<bool>(false); // has no notice
		write<int>(0); // thread count
		write<int>(0); // some calculation related to threadcount
		return;
	}
	int thread_count = 0; // get it from sql or from cache
	int local_thread_id = 0;
	bool has_notice = (local_thread_id == 0);
	write<bool>(has_notice);

	if (has_notice)
	{
		guild_bbs_add_thread();
		thread_count--; // one thread didn't count (because it's a notice)
	}

	/*
	// seek to the thread before where we start
	if (!rs.absolute(start + 1))
	{

		rs.first(); // we're trying to start at a place past possible

		start = 0;
	}
	*/

	write<int>(thread_count);
	int testx = 0; // Math.min(10, threadCount - start)
	write<int>(testx);

	for (int i = 0; i < testx; i++/*++i*/)
	{
		guild_bbs_add_thread();
		//rs.next();
	}
}

void PacketCreator::guild_bbs_show_thread(int local_thread_id)
{
	write<short>(send_headers::kGUILD_BBS_OPERATION);
	write<signed char>(GuildBBSSendPacketActions::kShowThread);
	write<int>(local_thread_id);
	write<int>(0); // poster cid
	write<long long>(0); // timestamp
	write<std::string>(""); // name
	write<std::string>(""); // start postt
	write<int>(0); // icon

	int reply_count = 0;
	write<int>(reply_count); // reply count

	int i;
	for (i = 0; i < reply_count/* && repliesRS.next()*/; i++/*++i*/)
	{
		write<int>(0); // reply id
		write<int>(0); // poster cid
		write<long long>(0); // timestamp // mplew.writeLong(getKoreanTimestamp(repliesRS.getLong("timestamp")));
		write<std::string>(""); // content
	}
}

// start of guild alliance

void PacketCreator::GetAllianceInfo(/*MapleAlliance alliance*/)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kGetInfo);
	write<signed char>(1);
	/*mplew.writeInt(alliance.getId());
	mplew.writeMapleAsciiString(alliance.getName());
	for (int i = 1; i <= 5; i++)
	{
		mplew.writeMapleAsciiString(alliance.getRankTitle(i));
	}
	mplew.write(alliance.getGuilds().size());*/
	write<int>(2); // maybe capacity
	/*for (Integer guild : alliance.getGuilds())
	{
		mplew.writeInt(guild);
	}
	mplew.writeMapleAsciiString(alliance.getNotice());*/
}

void PacketCreator::MakeNewAlliance(/*MapleAlliance alliance, */Player *player)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kMakeAlliance);
	/*mplew.writeInt(alliance.getId());
	mplew.writeMapleAsciiString(alliance.getName());
	for (int i = 1; i <= 5; i++)
	{
		mplew.writeMapleAsciiString(alliance.getRankTitle(i));
	}
	mplew.write(alliance.getGuilds().size());
	for (Integer guild : alliance.getGuilds())
	{
		mplew.writeInt(guild);
	}*/
	write<int>(2); // maybe capacity
	write<short>(0);
	/*for (Integer guildd : alliance.getGuilds())
	{
		getGuildInfo(mplew, Server.getInstance().getGuild(guildd, c.getPlayer().getMGC()));
	}*/
}

void PacketCreator::GetGuildAlliances(/*MapleAlliance alliance, */Player *player)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kGetAlliances);
	/*write<int>(alliance.getGuilds().size());
	for (Integer guild : alliance.getGuilds())
	{
		getGuildInfo(mplew, Server.getInstance().getGuild(guild, null));
	}*/
}

void PacketCreator::AddGuildToAlliance(/*MapleAlliance alliance, */int new_guild, Player *player)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kAddGuildToAlliance);
	/*mplew.writeInt(alliance.getId());
	mplew.writeMapleAsciiString(alliance.getName());
	for (int i = 1; i <= 5; i++)
	{
		mplew.writeMapleAsciiString(alliance.getRankTitle(i));
	}
	mplew.write(alliance.getGuilds().size());
	for (Integer guild : alliance.getGuilds())
	{
		mplew.writeInt(guild);
	}*/
	write<int>(2); // maybe capacity
	//mplew.writeMapleAsciiString(alliance.getNotice());
	write<int>(new_guild);
	//getGuildInfo(mplew, Server.getInstance().getGuild(newGuild, null));
}

void PacketCreator::GuildAllianceMemberOnline(Player *player, bool online)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kAllianceGuildMemberOnline);
	write<int>(player->get_guild()->get_alliance_id());
	write<int>(player->get_guild()->get_id());
	write<int>(player->get_id());
	write<bool>(online);
}

void PacketCreator::GuildAllianceNotice(int id, std::string notice)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kGuildAllianceNotice);
	write<int>(id);
	write<std::string>(notice);
}

void PacketCreator::ChangeAllianceRankTitle(int alliance, std::vector<std::string> ranks)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kChangeAllianceRankTitle);
	write<int>(alliance);
	for (auto it : ranks)
	{
		write<std::string>(it);
	}
}

void PacketCreator::UpdateAllianceJobLevel(Player *player)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kUpdateAllianceJobLevel);
	write<int>(player->get_guild()->get_alliance_id());
	write<int>(player->get_guild()->get_id());
	write<int>(player->get_id());
	write<int>(player->get_level());
	write<int>(player->get_job());
}

void PacketCreator::RemoveGuildFromAlliance(/*MapleAlliance alliance, */int expelled_guild, Player *player)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kRemoveGuildFromAlliance);
	/*write<int>(alliance.getId());
	mplew.writeMapleAsciiString(alliance.getName());
	for (int i = 1; i <= 5; i++)
	{
		mplew.writeMapleAsciiString(alliance.getRankTitle(i));
	}
	mplew.write(alliance.getGuilds().size());
	for (Integer guild : alliance.getGuilds())
	{
		mplew.writeInt(guild);
	}*/
	write<int>(2); // maybe capacity
	//mplew.writeMapleAsciiString(alliance.getNotice());
	write<int>(expelled_guild);
	//GetGuildInfo(mplew, Server.getInstance().getGuild(expelledGuild, null));
	write<signed char>(0x01);
}

void PacketCreator::DisbandAlliance(int alliance)
{
	write<short>(send_headers::kGUILD_ALLIANCE_OPERATION);
	write<signed char>(GuildAllianceSendPacketActions::kDisbandAlliance);
	write<int>(alliance);
}
