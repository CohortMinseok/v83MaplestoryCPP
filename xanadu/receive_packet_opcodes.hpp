//

#pragma once

namespace receive_headers_login
{
	constexpr short kLoginRequest = 0x01;
	constexpr short kWORLD_BACK = 0x04;
	constexpr short kCHANNEL_SELECT = 0x05;
	constexpr short kWORLD_SELECT = 0x06;
	constexpr short kHANDLE_LOGIN = 0x09;
	constexpr short kSHOW_WORLD = 0x0B;
	constexpr short kBACK_TO_WORLD = 0x0C;
	constexpr short kVIEW_ALL_CHARACTERS = 0x0D;
	constexpr short kPLAYER_LOGGEDIN = 0x14;
	constexpr short kNAME_CHECK = 0x15;
	constexpr short kCREATE_CHARACTER = 0x16;
	constexpr short kDELETE_CHARACTER = 0x17;
	constexpr short kLOGIN_BACK = 0x1C;
	constexpr short kSELECT_CHARACTER_WITH_PIC = 0x1E;
	constexpr short kSELECT_CHARACTER_WITH_PIC_VAC = 0x20;
}

namespace receive_headers
{
	constexpr short kREQUEST_MAP_CHANGE = 0x26;
	constexpr short kCHANGE_CHANNEL = 0x27;
	constexpr short kENTER_CASHSHOP = 0x28;
	constexpr short kMOVE_PLAYER = 0x29;
	constexpr short kCANCEL_CHAIR = 0x2A;
	constexpr short kUSE_CHAIR = 0x2B;
	constexpr short kREQUEST_CLOSE_RANGE_ATTACK = 0x2C;
	constexpr short kRANGED_ATTACK = 0x2D;
	constexpr short kMAGIC_ATTACK = 0x2E;
	constexpr short kENERGY_ATTACK = 0x2F;
	constexpr short kREQUEST_TAKE_DAMAGE = 0x30;
	constexpr short kGENERAL_CHAT = 0x31;
	constexpr short kCLOSE_CHALKBOARD = 0x32;
	constexpr short kFACE_EXPRESSION = 0x33;
	constexpr short kUSE_ITEMEFFECT = 0x34;
	constexpr short kHANDLE_NPC = 0x3A;
	constexpr short kNPC_CHAT = 0x3C;
	constexpr short kNPC_SHOP = 0x3D;
	constexpr short kSTORAGE = 0x3E;
	constexpr short kHIRED_MERCHANT_REQUEST = 0x3F;
	constexpr short kFREDRICK_OPERATION = 0x40;
	constexpr short kITEM_SORT = 0x45;
	constexpr short kMOVE_ITEM = 0x47;
	constexpr short kUSE_ITEM = 0x48;
	constexpr short kCANCEL_ITEM_BUFF = 0x49;
	constexpr short kUSE_SUMMON_BAG = 0x4B;
	constexpr short kUSE_PET_FOOD = 0x4C;
	constexpr short kUSE_CASH_ITEM = 0x4F;
	constexpr short kUSE_SKILL_BOOK = 0x52;
	constexpr short kUSE_RETURN_SCROLL = 0x55;
	constexpr short kUSE_SCROLL = 0x56;
	constexpr short kDISTRIBUTE_AP = 0x57;
	constexpr short kDISTRIBUTE_AUTO_AP = 0x58;
	constexpr short kREQUEST_HEAL_OVER_TIME = 0x59;
	constexpr short kDISTRIBUTE_SP = 0x5A;
	constexpr short kUSE_SKILL = 0x5B;
	constexpr short kREQUEST_CANCEL_BUFF = 0x5C;
	constexpr short kSKILL_EFFECT = 0x5D;
	constexpr short kMESO_DROP = 0x5E;
	constexpr short kGIVE_FAME = 0x5F;
	constexpr short kREQUEST_CHARACTER_INFO = 0x61;
	constexpr short kSPAWN_PET = 0x62;
	constexpr short kCHANGE_MAP_SPECIAL = 0x64;
	constexpr short kQUEST_ACTION = 0x6B;
	constexpr short kMULTI_CHAT = 0x77;
	constexpr short kWHISPER = 0x78;
	constexpr short kMESSENGER = 0x7A;
	constexpr short kPLAYER_INTERACTION = 0x7B;
	constexpr short kPARTY_OPERATION = 0x7C;
	constexpr short kPARTY_REQUEST_ANSWER = 0x7D;
	constexpr short kGUILD_OPERATION = 0x7E;
	constexpr short kGUILD_DENIE = 0x7F;
	constexpr short kBUDDY_LIST = 0x82;
	constexpr short kARAN_COMBO = 0xA3;
	constexpr short kMOVE_PET = 0xA7;
	constexpr short kPET_CHAT = 0xA8;
	constexpr short kPET_COMMAND = 0xA9;
	constexpr short kPET_LOOT = 0xAA;
	constexpr short kSUMMON_MOVE = 0xAF;
	constexpr short kDAMAGE_MOB_SUMMON = 0xB0;
	constexpr short kSUMMON_DAMAGE = 0xB1;
	constexpr short kUSE_DOOR = 0x85;
	constexpr short kCHANGE_KEYMAP = 0x87;
	constexpr short kGUILD_ALLIANCE_OPERATION = 0x98;
	constexpr short kGUILD_BBS_OPERATION = 0x9B;
	constexpr short kENTER_MTS = 0x9C;
	constexpr short kMOVE_LIFE = 0xBC;
	constexpr short kREQUEST_ITEM_PICKUP = 0xCA;
	constexpr short kHIT_REACTOR = 0xCD;
	constexpr short kCHECK_CASH = 0xE4;
	constexpr short kCASHSHOP_OPERATION = 0xE5;
	constexpr short kUSE_HAMMER = 0x104;
}
