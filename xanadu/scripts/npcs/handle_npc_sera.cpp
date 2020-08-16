//NPC ID: 2100
#include "../player.hpp"

void Player::handle_npc_sera()
{
	if (get_state() == 0)
	{
		send_simple("Hi, welcome to MapleStory!\r\n#L0#Next.#l");
	}
	else if (get_state() == 1)
	{
		send_simple("I will reset your AP so that you can choose which to add.\r\nAlso, you will be able to reset your AP during your 1st job advancement.\r\nSo, please feel free to add STR so that you will be able to level faster! Have fun!\r\n#L0#Next.#l");
	}
	else if (get_state() == 2)
	{
		short temp = (str_ - 4 + dex_ - 4 + int_ - 4 + luk_ - 4);
		set_ap(ap_ + temp);
		set_str(4);
		set_dex(4);
		set_luk(4);
		set_int(4);
		set_map(100000000);
	}
	else
	{
		set_state(1000);
	}
}