//

#include "player.hpp"

#include "quest.hpp"
#include "quest_data.hpp"
#include "quest_data_provider.hpp"
#include "quest_reward_data.hpp"
#include "packetcreator.hpp"
#include "world.hpp"
#include "server_constants.hpp"

void Player::give_quest(int quest_id)
{
	QuestData *data = QuestDataProvider::get_instance()->get_quest_data(quest_id);
	if (!data)
	{
		return;
	}
	if (quests_in_progress_.find(quest_id) != quests_in_progress_.end())
	{
		return;
	}
	if (completed_quests_.find(quest_id) != completed_quests_.end())
	{
		return;
	}

	std::shared_ptr<Quest> quest(new Quest(quest_id, false));
	quests_in_progress_[quest_id] = quest;

	// mode values:
	// 0 = forfeit, 1 = update, 2 = completed
	signed char mode = 1;
	if (quest->is_completed())
	{
		mode = 2;
	}
	{
		PacketCreator packet;
		packet.UpdateQuestInfo(mode, quest.get());
		send_packet(&packet);
	}
	rewards_data(quest_id, true);
}

void Player::complete_quest(int quest_id, int npc_id)
{
	QuestData *data = QuestDataProvider::get_instance()->get_quest_data(quest_id);
	if (!data)
	{
		return;
	}
	if (quests_in_progress_.find(quest_id) == quests_in_progress_.end())
	{
		return;
	}

	std::shared_ptr<Quest> quest = quests_in_progress_[quest_id];
	// check if the monster requirements to complete the quest are met
	auto killed_monsters = quest->get_killed_mobs2();
	auto required_monsters = data->get_required_mobs();

	for (auto &it : *required_monsters)
	{
		int required_monster_id = it.first;
		int required_amount = it.second;
		auto temp_it = killed_monsters->find(required_monster_id);
		if (temp_it == killed_monsters->end())
		{
			return;
		}
		if (temp_it->second < required_amount)
		{
			return;
		}
	}

	// check if the player has the items that are required to complete the quest
	auto required_items = data->get_required_items();
	for (auto &it : *required_items)
	{
		int required_item_id = it.first;
		int required_amount = it.second;
		if (get_item_amount(required_item_id) < required_amount)
		{
			return;
		}

		remove_item(required_item_id, required_amount);
	}

	completed_quests_[quest_id] = quest;
	quests_in_progress_.erase(quest_id);
	quest->set_completed();

	// mode values:
	// 0 = forfeit, 1 = update, 2 = completed
	signed char mode = 1;
	if (quest->is_completed())
	{
		mode = 2;
	}
	{
		PacketCreator packet;
		packet.UpdateQuestInfo(mode, quest.get());
		send_packet(&packet);
	}
	{
		PacketCreator packet;
		packet.UpdateQuest(quest.get(), npc_id);
		send_packet(&packet);
	}
	rewards_data(quest_id, false);
}

void Player::remove_quest(int quest_id)
{
	auto iterator = quests_in_progress_.find(quest_id);
	if (iterator == quests_in_progress_.end())
	{
		return;
	}

	// mode values:
	// 0 = forfeit, 1 = update, 2 = completed
	{
		PacketCreator packet;
		packet.UpdateQuestInfo(0, iterator->second.get());
		send_packet(&packet);
	}
	quests_in_progress_.erase(quest_id);
}

void Player::initialize_player_quests(int quest_id, bool is_completed, int mob_id, int amount)
{
	std::shared_ptr<Quest> quest(new Quest(quest_id, is_completed));
	if (is_completed)
	{
		completed_quests_[quest_id] = quest;
	}
	else
	{
		quests_in_progress_[quest_id] = quest;
		quest->add_killed_mob(mob_id, amount);
	}
}

void Player::rewards_data(int quest_id, bool start)
{
	QuestData *data = QuestDataProvider::get_instance()->get_quest_data(quest_id);
	if (!data)
	{
		return;
	}

	auto rewards = data->get_rewards_data();

	for (auto &it : *rewards)
	{
		QuestRewardData *reward = it;

		if (reward->start == start)
		{
			if (reward->item)
			{
				give_item(reward->id, reward->count);
			}

			else if (reward->exp)
			{
				int gain = (kExpRate * reward->id);
				set_exp(exp_ + gain);
				{
					PacketCreator packet;
					packet.GainExp(gain, true, true, 0);
					send_packet(&packet);
				}
			}

			else if (reward->fame)
			{
				set_fame(get_fame() + reward->id);
				{
					PacketCreator packet;
					packet.FameGainChat(reward->id);
					send_packet(&packet);
				}
			}

			else if (reward->mesos)
			{
				add_mesos(reward->id);
				{
					PacketCreator packet;
					packet.MesosGainChat(reward->id);
					send_packet(&packet);
				}
			}
		}
	}
}

std::unordered_map<int, std::shared_ptr<Quest>> *Player::get_quests_in_progress()
{
	return &quests_in_progress_;
}

std::unordered_map<int, std::shared_ptr<Quest>> *Player::get_completed_quests()
{
	return &completed_quests_;
}
