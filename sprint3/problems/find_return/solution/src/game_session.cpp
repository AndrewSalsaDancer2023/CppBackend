#include "game_session.h"
#include "player_tokens.h"
#include "model.h"
#include "server_exceptions.h"
#include "loot_generator.h"
#include "utils.h"
#include "collision_detector.h"

constexpr double baseWidth = 0.5;
constexpr double lootWidth = 0.0;

namespace model
{

Player::Player(unsigned int id, const std::string& name, const std::string& token,
			   const model::Map* map, bool spawn_dog_in_random_point, unsigned defaultBagCapacity)
   	  : id_(id), name_(name), token_(token)
{
	dog_ = std::make_shared<Dog>(map, spawn_dog_in_random_point, defaultBagCapacity);
}

std::shared_ptr<Player> GameSession::AddPlayer(const std::string player_name, model::Map* map,
											   bool spawn_dog_in_random_point, unsigned defaultBagCapacity)
{
	map_ = map;
	auto itFind = std::find_if(players_.begin(), players_.end(), [&player_name](std::shared_ptr<Player>& player){
				return player->GetName() == player_name;
			});

   if(itFind != players_.end())
	return *itFind;
	
   PlayerTokens tk;
   auto token = tk.GetToken();
   auto player = std::make_shared<Player>(player_id, player_name, token, map, spawn_dog_in_random_point, defaultBagCapacity);

   players_.push_back(player);
   player_id++;
   
   return players_.back();
}

bool GameSession::HasPlayerWithAuthToken(const std::string& auth_token)
{
	auto itFind = std::find_if(players_.begin(), players_.end(), [&auth_token](std::shared_ptr<Player>& player){
	 	return player->GetToken() == auth_token;
	 });

	   if(itFind != players_.end())
		return true;

	return false;
}

std::shared_ptr<Player> GameSession::GetPlayerWithAuthToken(const std::string& auth_token)
{
	auto itFind = std::find_if(players_.begin(), players_.end(), [&auth_token](std::shared_ptr<Player>& player){
		 	return player->GetToken() == auth_token;
		 });

	if(itFind != players_.end() )
		return *itFind;

	throw PlayerAbsentException();
}

const std::vector<std::shared_ptr<Player>> GameSession::GetAllPlayers()
{
	return players_;
}

void AddLootToDog(std::shared_ptr<model::Dog> dog, std::vector<LootInfo>& loots, const std::vector<collision_detector::Item>& items)
{
	for(const auto& item : items)
	{
		if(item.item_type == collision_detector::ItemType::Office)
		{
			std::cout << "GameSession::Pass loot to office: " << std::endl;
			dog->PassLootToOffice();
		}
		else
		{
			auto itFind = std::find_if(loots.begin(), loots.end(),
									[id = item.id](const auto& elem ){
														return elem.id == id;
													});

			if(itFind == loots.end())
				continue;

			if(dog->AddLoot(*itFind))
				loots.erase(itFind);
		}
	}
}

std::vector<collision_detector::Item> GetGatheredItems(const collision_detector::Gatherer& gatherer, std::vector<LootInfo>& loots, model::Map* map)
{
	collision_detector::ItemGatherer item_gath;

	item_gath.AddGatherer(gatherer);

	for(const auto& cur_loot : loots)
	{
		collision_detector::Item item(cur_loot.id, {cur_loot.x, cur_loot.y}, lootWidth);
		item_gath.AddItem(item);
	}

	if(map)
	{
		const auto& offices = map->GetOffices();
		for(const auto& office : offices)
		{
		//		std::cout << "Dog::GatherLoot gather office loot" << std::endl;
			collision_detector::Item item(std::numeric_limits<unsigned>::max(),
										  {(double)office.GetPosition().x, (double)office.GetPosition().y},
										  baseWidth, collision_detector::ItemType::Office);
			//item_gath.AddItem(item);
			item_gath.AddItem(item);
		}
	}
	const auto& ev = collision_detector::FindGatherEvents(item_gath);

	std::vector<collision_detector::Item> result;

	for(size_t i =0; i < ev.size(); ++i)
	{
/*		gathered_items_id_.push_back(ev[i].item_id);
		std::cout << std::endl;
        std::cout << "gather loot id: " << ev[i].item_id << std::endl;
        std::cout << std::endl;*/
		std::cout << "gather loot id: " << ev[i].item_id << std::endl;
		result.push_back(item_gath.GetItem(ev[i].item_id));
		//std::cout << "GetGatheredItems push_back item: " << result.back().id << std::endl;
	}

	return result;
}

void GameSession::MoveDogs(int deltaTime)
{
	std::for_each(players_.begin(), players_.end(), [this, deltaTime](std::shared_ptr<Player>& player){
		std::optional<collision_detector::Gatherer> gatherer = player->GetDog()->Move(deltaTime);
		if(!gatherer)
			return;

		auto items = GetGatheredItems(*gatherer, loots_info_, map_);
		AddLootToDog(player->GetDog(), loots_info_, items);
	});
}

void GameSession::InitLootGenerator(double loot_period, double loot_probability)
{
	unsigned long duration = loot_period * 1000;
	lootGen_ = std::make_shared<loot_gen::LootGenerator>(loot_gen::LootGenerator::TimeInterval{duration}, loot_probability);
}

model::LootInfo GenerateLootInfo(const Map* pMap)
{
	if(!pMap)
		throw std::logic_error("Invalid map");

	size_t num_loots = pMap->GetNumLoots();
	if(!num_loots)
		throw logic_error("No loot specified for the map!");

	static unsigned loot_id = 0;
	auto loot_type = utils::GetRandomNumber<size_t>(0, num_loots-1);

	size_t num_roads = pMap->GetNumRoads();
	if(!num_roads)
			throw logic_error("No roads specified for the map!");

	auto road_index = utils::GetRandomNumber<size_t>(0, num_roads-1);

	const auto& roads = pMap->GetRoads();


	auto start = roads[road_index].GetStart();
	auto end = roads[road_index].GetEnd();
	int x, y;
	if(roads[road_index].IsHorizontal())
	{
		if(start.x > end.x)
			std::swap(start, end);

		x = utils::GetRandomNumber<int>(start.x, end.x);
		y = start.y;
	}
	else
	{
		if(start.y > end.y)
			std::swap(start, end);

		x = start.x;
		y = utils::GetRandomNumber<int>(start.y, end.y);
	}
//	std::cout << "loots generated:" << "index:" << loot_index << " x:" << x <<" y:" << y << std::endl;
	model::LootInfo loot_info(loot_id, loot_type, x, y);
	loot_id++;
	return loot_info;
}


void GameSession::GenerateLoot(int deltaTime, const Map* pMap)
{
//	std::cout << "GenerateLoot players:" << players_.size() << "Loots size: " << loots_info_.size() << std::endl;

	auto num_loot_to_generate = lootGen_->Generate(loot_gen::LootGenerator::TimeInterval{deltaTime}, loots_info_.size(), players_.size());

//	std::cout << "GenerateLoot num_loot_to_generate:" << num_loot_to_generate << std::endl;
	while(num_loot_to_generate > 0)
	{
//		std::cout << "num_loot_to_generate:" << num_loot_to_generate << std::endl;
		loots_info_.push_back(GenerateLootInfo(pMap));
//		std::cout << "loots size:" << loots_info_.size() << std::endl;
		num_loot_to_generate--;
	}
}
}
