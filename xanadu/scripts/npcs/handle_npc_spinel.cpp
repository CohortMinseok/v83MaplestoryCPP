//NPC ID: 9000020
#include "../player.hpp"
#include "../job_constants.hpp"

void Player::handle_npc_spinel()
{
	int town_map_ids[] = { 100000000, 101050000, 200000000, 211000000, 220000000, 221000000, 230000000, 222000000, 260000000, 310000000, 251000000, 250000000, 140000000, 540000000, 270000000, 682000000, 240000000 };
	if (get_state() == 0)
	{
		send_simple("Where would you like to go? \r\n#L0#Henesys#l \r\n#L1#Elluel#l \r\n#L2#Orbis#l \r\n#L3#El Nath#l \r\n#L4#Ludibrium#l \r\n#L5#Omega Sector#l \r\n#L6#Aquarium#l \r\n#L7#Korean Folk Town#l \r\n#L8#Ariant#l \r\n#L9#Edelstein#l \r\n#L10#Herb Town#l \r\n#L11#Mu Lung#l \r\n#L12#Rien#l \r\n#L13#Singapore#l \r\n#L14#Time Road - Three Doors#l \r\n#L15#Haunted House#l \r\n#L16#Leafre#l");
	}
	if (get_state() == 1)
	{
		int target_map_id = town_map_ids[get_selected()];
		set_map(target_map_id);
	}
}
