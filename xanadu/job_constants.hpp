//

#pragma once

namespace job_ids
{
	constexpr short kBeginner = 0;
	constexpr short kSwordsman = 100;
	constexpr short kFighter = 110;
	constexpr short kCrusader = 111;
	constexpr short kHero = 112;
	constexpr short kPage = 120;
	constexpr short kWhiteKnight = 121;
	constexpr short kPaladin = 122;
	constexpr short kSpearman = 130;
	constexpr short kDragonKnight = 131;
	constexpr short kDarkKnight = 132;
	constexpr short kMagician = 200;
	constexpr short kFpWizard = 210;
	constexpr short kFpMage = 211;
	constexpr short kFpArchMage = 212;
	constexpr short kIlWizard = 220;
	constexpr short kIlMage = 221;
	constexpr short kIlArchMage = 222;
	constexpr short kCleric = 230;
	constexpr short kPriest = 231;
	constexpr short kBishop = 232;
	constexpr short kArcher = 300;
	constexpr short kHunter = 310;
	constexpr short kRanger = 311;
	constexpr short kBowmaster = 312;
	constexpr short kCrossbowman = 320;
	constexpr short kSniper = 321;
	constexpr short kMarksman = 322;
	constexpr short kRogue = 400;
	constexpr short kAssassin = 410;
	constexpr short kHermit = 411;
	constexpr short kNightLord = 412;
	constexpr short kBandit = 420;
	constexpr short kChiefBandit = 421;
	constexpr short kShadower = 422;
	constexpr short kPirate = 500;
	constexpr short kBrawler = 510;
	constexpr short kMarauder = 511;
	constexpr short kBuccaneer = 512;
	constexpr short kGunslinger = 520;
	constexpr short kOutlaw = 521;
	constexpr short kCorsair = 522;
	constexpr short kGm = 900;
	constexpr short kNoblesse = 1000;
	constexpr short kDawnWarrior1 = 1100;
	constexpr short kDawnWarrior2 = 1110;
	constexpr short kDawnWarrior3 = 1111;
	constexpr short kDawnWarrior4 = 1112;
	constexpr short kBlazeWizard1 = 1200;
	constexpr short kBlazeWizard2 = 1210;
	constexpr short kBlazeWizard3 = 1211;
	constexpr short kBlazeWizard4 = 1212;
	constexpr short kWindArcher1 = 1300;
	constexpr short kWindArcher2 = 1310;
	constexpr short kWindArcher3 = 1311;
	constexpr short kWindArcher4 = 1312;
	constexpr short kNightWalker1 = 1400;
	constexpr short kNightWalker2 = 1410;
	constexpr short kNightWalker3 = 1411;
	constexpr short kNightWalker4 = 1412;
	constexpr short kThunderBreaker1 = 1500;
	constexpr short kThunderBreaker2 = 1510;
	constexpr short kThunderBreaker3 = 1511;
	constexpr short kThunderBreaker4 = 1512;
	constexpr short kLegend = 2000;
	constexpr short kAran1 = 2100;
	constexpr short kAran2 = 2110;
	constexpr short kAran3 = 2111;
	constexpr short kAran4 = 2112;
}

inline bool is_beginner_job(short job_id)
{
	switch (job_id)
	{
	case job_ids::kBeginner:
	case job_ids::kNoblesse:
	case job_ids::kLegend:
		return true;
	default:
		return false;
	}
}
