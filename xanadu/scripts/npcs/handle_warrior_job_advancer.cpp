//

#include "../player.hpp"
#include "../job_constants.hpp"

void Player::handle_warrior_job_advancer()
{
	//Initialize variables
	int level_diff = 0;
	bool sp_correct = false;
	switch (job_)
	{

	case job_ids::kBeginner:
	{

		if (level_ >= 10 && get_str() >= 35)
		{
			if (get_state() == 0)
			{
				send_simple("Do you want to become a Warrior? \r\n#L0#Yes.#l \r\n#L1#No.#l");
			}
			else if (get_state() == 1)
			{
				switch (get_selected())
				{
				case 0:
					level_diff = get_level() - 10;
					set_job(job_ids::kSwordsman);
					if (level_diff > 0)
					{
						add_sp(level_diff * 3);
					}
					give_item(1402001, 1);
					send_ok("You have chosen wisely. Now go, go with pride.");
					break;

				case 1:
					send_ok("Come to me again when you made up your mind.");
					break;
				}
			}
		}

		else {
			send_ok("Train a bit more and I can show you the way of the #rWarrior#k.");
		}

		break;
	}

	case job_ids::kSwordsman:
	{
		level_diff = get_level() - 30;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 30 && sp_correct)
		{
			if (get_state() == 0)
			{
				send_simple("Choose the path you wish to follow. \r\n#L0#Fighter#l \r\n#L1#Page#l \r\n#L2#Spearman#l");
			}

			else if (get_state() == 1)
			{
				switch (get_selected())
				{
				case 0:
					set_job(job_ids::kFighter);

					break;

				case 1:
					set_job(job_ids::kPage);

					break;

				case 2:
					set_job(job_ids::kSpearman);

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

	case job_ids::kFighter:
	case job_ids::kPage:
	case job_ids::kSpearman:
	{
		level_diff = get_level() - 70;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 70 && sp_correct)
		{
			send_ok("You are really a strong one.");
			switch (job_)
			{
			case job_ids::kFighter:
				set_job(job_ids::kCrusader);
				break;

			case job_ids::kPage:
				set_job(job_ids::kWhiteKnight);
				break;

			case job_ids::kSpearman:
				set_job(job_ids::kDragonKnight);
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

	case job_ids::kCrusader:
	case job_ids::kWhiteKnight:
	case job_ids::kDragonKnight:
	{

		level_diff = get_level() - 120;
		if (get_sp() <= (level_diff * 3))
			sp_correct = true;
		if (level_ >= 120 && sp_correct)
		{
			send_ok("I knew this day would come.");
			switch (job_)
			{
			case job_ids::kCrusader:
				set_job(job_ids::kHero);
				break;

			case job_ids::kWhiteKnight:
				set_job(job_ids::kPaladin);
				break;

			case job_ids::kDragonKnight:
				set_job(job_ids::kDarkKnight);
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

	case job_ids::kHero:
	case job_ids::kPaladin:
	case job_ids::kDarkKnight:
	{
		send_ok("You have chosen wisely");
		break;
	}

	default:
		set_state(1000);
		break;
	}
}