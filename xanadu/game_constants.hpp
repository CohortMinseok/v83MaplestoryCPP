//

#pragma once

#include <string>

constexpr std::size_t kPacketHandshakeServerLength = 13; // the size of the handshake packet
constexpr short kGameVersion = 83; // mayor game version
const std::string kGameMinorVersion = "1"; // minor game version, if 0, then the string is empty

namespace GameLocales
{
	const signed char kGlobalMapleStory = 8;
}

constexpr unsigned char kGameLocale = GameLocales::kGlobalMapleStory;

// standard install path would be: "C:\\Nexon\\MapleStory\\"
const std::string kGameFilePath = "C:/Users/Z41N/Documents/Private Servers/v83/Maplestory/";