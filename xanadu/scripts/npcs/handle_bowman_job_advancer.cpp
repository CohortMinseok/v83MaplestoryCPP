//

#include "../player.hpp"
#include "../job_constants.hpp"

void Player::handle_bowman_job_advancer()
{
	//Initialize variables
	int level_diff = 0;
	bool sp_correct = false;
	switch (job_)
	{

	case job_ids::kBeginner:
	{
		
		if (level_ >= 10 && get_dex() >= 25)
		{
			if (get_state() == 0)
			{
				send_simple("So you decided to become a #rBowman#k? \r\n#L0#Yes.#l \r\n#L1#No.#l");
			}
			else if (get_state() == 1)
			{
				switch (get_selected())
				{
				case 0:
					level_diff = get_level() - 10;
					set_job(job_ids::kArcher);
					if (level_diff > 0)
					{
						add_sp(level_diff * 3);
					}
					give_item(1452002, 1);
					give_item(2060004, 5000);
					send_ok("You have chosen wisely. Now go, go with pride.");
					break;

				case 1:
					send_ok("Come to me again when you made up your mind.");
					break;
				}
			}
		}

		else {
			send_ok("Train a bit more and I can show you the way of the #rBowman#k.");
		}

		break;
	}

	case job_ids::kArcher:
	{
		level_diff = get_level() - 30;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 30 && sp_correct)
		{
			if (get_state() == 0)
			{
				send_simple("Choose the path you wish to follow. \r\n#L0#Hunter#l \r\n#L1#Crossbowman#l");
			}

			else if (get_state() == 1)
			{
				switch (get_selected())
				{
				case 0:
					set_job(job_ids::kHunter);
					break;

				case 1:
					set_job(job_ids::kCrossbowman);
					break;
				}
			}
		}
		else
		{
			if (level_ >= 30 && !sp_correct)
				send_ok("Your time has yet to come.");
			else
				send_ok("You have chosen wisely");
		}

		break;
	}

	case job_ids::kHunter:
	case job_ids::kCrossbowman:
	{
		level_diff = get_level() - 70;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 70 && sp_correct)
		{
			send_ok("You are really a strong one.");
			switch (job_)
			{
			case job_ids::kHunter:
				set_job(job_ids::kRanger);
				break;

			case job_ids::kCrossbowman:
				set_job(job_ids::kSniper);
				break;
			}
		}
		else
		{
			if (level_ >= 70 && !sp_correct)
				send_ok("Your time has yet to come.");
			else
				send_ok("You have chosen wisely");
		}

		break;
	}

	case job_ids::kRanger:
	case job_ids::kSniper:
	{
		level_diff = get_level() - 120;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 120 && sp_correct)
		{
			send_ok("I knew this day would come.");
			switch (job_)
			{
			case job_ids::kRanger:
				set_job(job_ids::kBowmaster);
				break;

			case job_ids::kSniper:
				set_job(job_ids::kMarksman);
				break;
			}
		}
		else
		{
			if (level_ >= 120 && !sp_correct)
				send_ok("Your time has yet to come.");
			else
				send_ok("You have chosen wisely");
		}

		break;
	}

	case job_ids::kBowmaster:
	case job_ids::kMarksman:
	{
		send_ok("You have chosen wisely");
		break;
	}

	default:
		set_state(1000);
		break;
	}
}