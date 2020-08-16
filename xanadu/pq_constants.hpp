//

#pragma once

// MCPQ = Monster Carnival Party Quest

namespace mcpq_constants
{
	// team constants

	constexpr const unsigned char kTeamRed = 0;
	constexpr const unsigned char kTeamBlue = 1;
	constexpr const unsigned char kNoTeamButStarted = 2;
	constexpr const unsigned char kNoTeamButNotStarted = 0xFF;

	// message constants, used for carnival_pq_message function

	constexpr const signed char kMessageNotEnoughCP = 1; // You don't have enough CP to continue.
	constexpr const signed char kMessageYouCanNotSummonMob = 2; // You can no longer summon the Monster.
	constexpr const signed char kMessageYouCanNotSummonBeing = 3; // You can no longer summon the being.
	constexpr const signed char kMessageBeingAlreadySummoned = 4; // This being is already summoned.
	constexpr const signed char kMessageRequestFailedUnkError = 5; // This request has failed due to an unknown error.
																   
																   // info for carnival_pq_summon function

																   // possible values for tab:
																   // TAB_SPAWNS/SUMMONS/MOBS = 0;
																   // TAB_DEBUFF/SKILL = 1;
																   // TAB_GUARDIAN = 2;

																   // possible values for summon number:
																   // 0 = Brown Teddy
																   // 1 = Bloctopus
																   // 2 = Ratz
}
