//

#include "../player.hpp"
#include "../job_constants.hpp"

void Player::handle_mage_job_advancer()
{
	//Initialize variables
	int level_diff = 0;
	bool sp_correct = false;
	switch (job_)
	{

	case job_ids::kBeginner:
	{

		if (level_ >= 8 && get_int() >= 20)
		{
			if (get_state() == 0)
			{
				send_simple("Do you want to become a Magician? \r\n#L0#Yes.#l \r\n#L1#No.#l");
			}
			else if (get_state() == 1)
			{
				switch (get_selected())
				{
				case 0:
					level_diff = get_level() - 8;
					set_job(job_ids::kMagician);
					if (level_diff > 0)
					{
						add_sp(level_diff * 3);
					}
					give_item(1372005, 1);
					send_ok("You have chosen wisely. Now go, go with pride.");
					break;

				case 1:
					send_ok("Come to me again when you made up your mind.");
					break;
				}
			}
		}
		else {
			send_ok("Train a bit more and I can show you the way of the #rMagician#k.");
		}
		break;
	}

	case job_ids::kMagician:
	{
		level_diff = get_level() - 30;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 30 && sp_correct)
		{
			if (get_state() == 0)
			{
				send_simple("Choose the path you wish to follow. \r\n#L0#Fire & Poison Wizard#l \r\n#L1#Ice & Lightning Wizard#l \r\n#L2#Cleric#l");
			}

			else if (get_state() == 1)
			{
				switch (get_selected())
				{
				case 0:
					set_job(job_ids::kFpWizard);

					break;

				case 1:
					set_job(job_ids::kIlWizard);

					break;

				case 2:
					set_job(job_ids::kCleric);

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

	case job_ids::kFpWizard:
	case job_ids::kIlWizard:
	case job_ids::kCleric:
	{

		level_diff = get_level() - 70;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 70 && sp_correct)
		{
			send_ok("You are really a strong one.");
			switch (job_)
			{
			case job_ids::kFpWizard:
				set_job(job_ids::kFpMage);
				break;

			case job_ids::kIlWizard:
				set_job(job_ids::kIlMage);
				break;

			case job_ids::kCleric:
				set_job(job_ids::kPriest);
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

	case job_ids::kFpMage:
	case job_ids::kIlMage:
	case job_ids::kPriest:
	{

		level_diff = get_level() - 120;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 120 && sp_correct)
		{
			send_ok("I knew this day would come.");
			switch (job_)
			{
			case job_ids::kFpMage:
				set_job(job_ids::kFpArchMage);
				break;

			case job_ids::kIlMage:
				set_job(job_ids::kIlArchMage);
				break;

			case job_ids::kPriest:
				set_job(job_ids::kBishop);
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

	case job_ids::kFpArchMage:
	case job_ids::kIlArchMage:
	case job_ids::kBishop:
	{
		send_ok("You have chosen wisely");
		break;
	}

	default:
		set_state(1000);
		break;
	}
}