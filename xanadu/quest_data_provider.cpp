//

#include "quest_data_provider.hpp"

#include "quest_data.hpp"
#include "world.hpp"
#include "wznode.hpp"
#include "wzmain.hpp"

// singleton

QuestDataProvider *QuestDataProvider::singleton_ = nullptr;

QuestDataProvider *QuestDataProvider::get_instance()
{
	if (!singleton_)
	{
		singleton_ = new QuestDataProvider();
	}

	return singleton_;
}

void QuestDataProvider::load_data()
{
	// Quest.wz

	WZMain *wz_reader = World::get_instance()->wz_reader_;
	WZNode node1 = wz_reader->base_["Quest"]["QuestInfo"];

	for (auto it1 : node1)
	{
		std::string valuename1 = it1.name();
		int quest_id = std::stoi(valuename1);
		QuestData *quest = new QuestData(quest_id);
		quests_[quest_id] = quest;
	}

	// Act.img / Rewards

	WZNode node2 = wz_reader->base_["Quest"]["Act"];

	for (auto it2 : node2)
	{
		std::string valuename1 = it2.name();
		int quest_id = std::stoi(valuename1);
		QuestData *quest = quests_[quest_id];

		if (!quest)
		{
			quest = new QuestData(quest_id);
			quests_[quest_id] = quest;
		}

		WZNode node31 = wz_reader->base_["Quest"]["Act"][valuename1.c_str()];

		for (auto it4 : node31)
		{
			std::string valuename2 = it4.name();

			WZNode node32 = wz_reader->base_["Quest"]["Act"][valuename1.c_str()][valuename2.c_str()];

			for (auto it5 : node32)
			{
				std::string valuename3 = it5.name();

				if (valuename3 == "item")
				{
					WZNode node33 = wz_reader->base_["Quest"]["Act"][valuename1.c_str()][valuename2.c_str()]["item"];

					for (auto it6 : node33)
					{
						std::string valuename4 = it6.name();

						QuestRewardData *rew = new QuestRewardData();

						rew->start = false;
						rew->item = false;
						rew->mesos = false;
						rew->exp = false;
						rew->fame = false;
						rew->id = 0;
						rew->count = 0;

						WZNode node34 = wz_reader->base_["Quest"]["Act"][valuename1.c_str()][valuename2.c_str()]["item"][valuename4.c_str()];

						for (auto it7 : node34)
						{

							std::string valuename5 = it7.name();

							if (valuename5 == "id")
							{
								rew->id = it7.get_int_value();
							}

							if (valuename5 == "count")
							{
								rew->count = it7.get_int_value();
							}

							if (rew->id != 0 && rew->count != 0)
							{
								rew->item = true;
							}
						}
						
						quest->add_reward(rew);
					}
				}

				else if (valuename3 == "exp")
				{
					QuestRewardData *rew = new QuestRewardData();

					rew->start = false;
					rew->item = false;
					rew->mesos = false;
					rew->exp = false;
					rew->fame = false;
					rew->id = 0;
					rew->count = 0;

					rew->exp = (it5.get_int_value() != 0);
					rew->id = it5.get_int_value();

					quest->add_reward(rew);
				}

				else if (valuename3 == "money")
				{
					QuestRewardData *rew = new QuestRewardData();

					rew->start = false;
					rew->item = false;
					rew->mesos = false;
					rew->exp = false;
					rew->fame = false;
					rew->id = 0;
					rew->count = 0;

					rew->mesos = (it5.get_int_value() != 0);
					rew->id = it5.get_int_value();

					quest->add_reward(rew);
				}

				else if (valuename3 == "pop")
				{
					QuestRewardData *rew = new QuestRewardData();

					rew->start = false;
					rew->item = false;
					rew->mesos = false;
					rew->exp = false;
					rew->fame = false;
					rew->id = 0;
					rew->count = 0;

					rew->fame = (it5.get_int_value() != 0);
					rew->id = it5.get_int_value();

					quest->add_reward(rew);
				}
			}
		}
	}
	// TO-DO implement this above this
	/*if (valuename3 == "start")
			{
				bool start = it3.get_int_value() ? 0 : 1;
				rew->start = start;
			}*/

	// Check.img / Requests

	WZNode node3 = wz_reader->base_["Quest"]["Check"];

	for (auto it3 : node3)
	{
		std::string valuename1 = it3.name();
		int quest_id = std::stoi(valuename1);
		QuestData *quest = quests_[quest_id];

		if (!quest)
		{
			quest = new QuestData(quest_id);
			quests_[quest_id] = quest;
		}

		WZNode node31 = wz_reader->base_["Quest"]["Check"][valuename1.c_str()];

		for (auto it4 : node31)
		{
			std::string valuename2 = it4.name();

			WZNode node32 = wz_reader->base_["Quest"]["Check"][valuename1.c_str()][valuename2.c_str()];

			for (auto it5 : node32)
			{
				std::string valuename3 = it5.name();

				if (valuename3 == "item")
				{
					WZNode node33 = wz_reader->base_["Quest"]["Check"][valuename1.c_str()][valuename2.c_str()]["item"];

					for (auto it6 : node33)
					{
						std::string valuename4 = it6.name();

						WZNode node34 = wz_reader->base_["Quest"]["Check"][valuename1.c_str()][valuename2.c_str()]["item"][valuename4.c_str()];

						int object_id = 0;
						int amount = 0;

						for (auto it7 : node34)
						{
							std::string valuename5 = it7.name();

							if (valuename5 == "id")
							{
								object_id = it7.get_int_value();
							}

							if (valuename5 == "count")
							{
								amount = it7.get_int_value();
							}
						}

						if (object_id != 0 && amount != 0)
						{
							quest->add_item_requirement(object_id, amount);
						}
					}
				}

				else if (valuename3 == "mob")
				{
					WZNode node33 = wz_reader->base_["Quest"]["Check"][valuename1.c_str()][valuename2.c_str()]["mob"];

					for (auto it6 : node33)
					{
						std::string valuename4 = it6.name();

						WZNode node34 = wz_reader->base_["Quest"]["Check"][valuename1.c_str()][valuename2.c_str()]["mob"][valuename4.c_str()];

						int object_id = 0;
						int amount = 0;

						for (auto it7 : node34)
						{
							std::string valuename5 = it7.name();

							if (valuename5 == "id")
							{
								object_id = it7.get_int_value();
							}

							if (valuename5 == "count")
							{
								amount = it7.get_int_value();
							}
						}

						if (object_id != 0 && amount != 0)
						{
							quest->add_monster_requirement(object_id, amount);
						}
					}
				}
			}
		}
	}
}

QuestData *QuestDataProvider::get_quest_data(int quest_id)
{
	if (quests_.find(quest_id) == quests_.end())
	{
		return nullptr;
	}

	return quests_[quest_id];
}

std::unordered_map<int, QuestData *> *QuestDataProvider::get_data()
{
	return &quests_;
}
