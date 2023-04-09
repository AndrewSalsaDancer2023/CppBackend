#include "game_session.h"
#include "player_tokens.h"
#include "model.h"
#include "server_exceptions.h"
#include "loot_generator.h"
#include "utils.h"

namespace model
{

Player::Player(unsigned int id, const std::string& name, const std::string& token, const model::Map* map, bool spawn_dog_in_random_point)
   	  : id_(id), name_(name), token_(token)
{
	dog_ = std::make_shared<Dog>(map, spawn_dog_in_random_point);
}

std::shared_ptr<Player> GameSession::AddPlayer(const std::string player_name, const model::Map* map, bool spawn_dog_in_random_point)
{
	auto itFind = std::find_if(players_.begin(), players_.end(), [&player_name](std::shared_ptr<Player>& player){
				return player->GetName() == player_name;
			});

   if(itFind != players_.end())
	return *itFind;
	
   PlayerTokens tk;
   auto token = tk.GetToken();
   auto player = std::make_shared<Player>(player_id, player_name, token, map, spawn_dog_in_random_point);

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

void GameSession::MoveDogs(int deltaTime)
{
	std::for_each(players_.begin(), players_.end(), [deltaTime](std::shared_ptr<Player>& player){
		player->GetDog()->Move(deltaTime);
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

	auto loot_index = utils::GetRandomNumber<size_t>(0, num_loots-1);

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
	return model::LootInfo(loot_index, x, y);
}


void GameSession::GenerateLoot(int deltaTime, const Map* pMap)
{
//	std::cout << "GenerateLoot:" << std::endl;
	auto num_loot_to_generate = lootGen_->Generate(loot_gen::LootGenerator::TimeInterval{deltaTime}, loots_info_.size(), players_.size());
	while(num_loot_to_generate > 0)
	{
//		std::cout << "num_loot_to_generate:" << num_loot_to_generate << std::endl;
		loots_info_.push_back(GenerateLootInfo(pMap));
//		std::cout << "loots size:" << loots_info_.size() << std::endl;
		num_loot_to_generate--;
	}
}
}
