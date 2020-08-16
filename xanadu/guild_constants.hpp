//

#pragma once

namespace GuildSendPacketActions
{
	constexpr signed char kCreate = 1;
	constexpr signed char kInvite = 5;
	constexpr signed char kChangeEmblem = 17;
	constexpr signed char kInfo = 26;
	constexpr signed char kAddPlayer = 39;
	constexpr signed char kPlayerLeave = 44;
	constexpr signed char kPlayerExpelled = 47;
	constexpr signed char kDisband = 50;
	constexpr signed char kMemberOnline = 61;
	constexpr signed char kUpdateRanks = 62;
	constexpr signed char kChangeRank = 64;
	constexpr signed char kShowEmblem = 66;
	constexpr signed char kUpdateNotice = 68;
}

namespace GuildBBSSendPacketActions
{
	constexpr signed char kShowThreads = 6;
	constexpr signed char kShowThread = 7;
}

namespace GuildAllianceSendPacketActions
{
	constexpr signed char kGetInfo = 12;
	constexpr signed char kGetAlliances = 13;
	constexpr signed char kAllianceGuildMemberOnline = 14;
	constexpr signed char kMakeAlliance = 15;
	constexpr signed char kRemoveGuildFromAlliance = 16;
	constexpr signed char kAddGuildToAlliance = 18;
	constexpr signed char kUpdateAllianceJobLevel = 24;
	constexpr signed char kChangeAllianceRankTitle = 26;
	constexpr signed char kGuildAllianceNotice = 28;
	constexpr signed char kDisbandAlliance = 29;
}
