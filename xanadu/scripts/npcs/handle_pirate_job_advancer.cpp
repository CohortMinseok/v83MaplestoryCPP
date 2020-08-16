//

#include "../player.hpp"
#include "../job_constants.hpp"

void Player::handle_pirate_job_advancer()
{
	//Initialize variables
	int level_diff = 0;
	bool sp_correct = false;
	switch (job_)
	{

	case job_ids::kBeginner:
	{

		if (level_ >= 10 && get_dex() >= 20)
		{
			if (get_state() == 0)
			{
				send_simple("Do you want to become a Pirate? \r\n#L0#Yes.#l \r\n#L1#No.#l");
			}
			else if (get_state() == 1)
			{
				switch (get_selected())
				{
				case 0:
					level_diff = get_level() - 10;
					set_job(job_ids::kPirate);
					if (level_diff > 0)
					{
						add_sp(level_diff * 3);
					}
					send_ok("You have chosen wisely. Now go, go with pride.");
					break;

				case 1:
					send_ok("Come to me again when you made up your mind.");
					break;
				}
			}
		}

		else {
			send_ok("Train a bit more and I can show you the way of the #rPirate#k.");
		}

		break;
	}

	case job_ids::kPirate:
	{
		level_diff = get_level() - 30;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 30 && sp_correct)
		{
			if (get_state() == 0)
			{
				send_simple("Choose the path you wish to follow. \r\n#L0#Brawler#l \r\n#L1#Gunslinger#l");
			}

			else if (get_state() == 1)
			{
				switch (get_selected())
				{
				case 0:
					set_job(job_ids::kBrawler);

					break;

				case 1:
					set_job(job_ids::kGunslinger);

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

	case job_ids::kBrawler:
	case job_ids::kGunslinger:
	{

		level_diff = get_level() - 70;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 70 && sp_correct)
		{
			send_ok("You are really a strong one.");
			switch (job_)
			{
			case job_ids::kBrawler:
				set_job(job_ids::kMarauder);
				break;

			case job_ids::kGunslinger:
				set_job(job_ids::kOutlaw);
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

	case job_ids::kMarauder:
	case job_ids::kOutlaw:
	{

		level_diff = get_level() - 120;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 120 && sp_correct)
		{
			send_ok("I knew this day would come.");
			switch (job_)
			{
			case job_ids::kMarauder:
				set_job(job_ids::kBuccaneer);
				break;

			case job_ids::kOutlaw:
				set_job(job_ids::kCorsair);
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

	case job_ids::kBuccaneer:
	case job_ids::kCorsair:
	{
		send_ok("You have chosen wisely");
		break;
	}

	default:
		set_state(1000);
		break;
	}
}