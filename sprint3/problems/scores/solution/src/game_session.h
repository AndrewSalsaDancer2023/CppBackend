#pragma once
#include "dog.h"

using namespace std;

class Dog;

namespace loot_gen {
class LootGenerator;
}

namespace model
{
class Map;
struct LootInfo;

class Player
{
public:
	Player(unsigned int id, const std::string& name, const std::string& token,
		 const model::Map* map, bool spawn_dog_in_random_point, unsigned defaultBagCapacity);
  	const std::string& GetToken() { return token_;}
  	const std::string& GetName() { return name_;}
  	unsigned int GetId() {return id_;}
  	std::shared_ptr<Dog> GetDog() { return dog_;}

private:
	std::string name_;
	std::string token_;
	unsigned int id_{0};
	std::shared_ptr<Dog> dog_;
};


class GameSession
{
public:
	GameSession(const std::string& map_id, double loot_period, double loot_probability)
	: map_id_(map_id)
	{InitLootGenerator(loot_period, loot_probability);}
	std::shared_ptr<Player> AddPlayer(const std::string player_name, model::Map* map,
									  bool spawn_dog_in_random_point, unsigned defaultBagCapacity);
	const std::string& GetMap() {return map_id_;}
	bool HasPlayerWithAuthToken(const std::string& auth_token);
	const std::vector<std::shared_ptr<Player>> GetAllPlayers();
	std::shared_ptr<Player> GetPlayerWithAuthToken(const std::string& auth_token);
	void MoveDogs(int deltaTime);
	size_t GetNumPlayers() { return players_.size();}
	const vector<model::LootInfo>& GetLootsInfo() { return loots_info_;};
	void GenerateLoot(int deltaTime, const Map* pMap);

private:
	void InitLootGenerator(double loot_period, double loot_probability);
	vector<std::shared_ptr<Player>> players_;
	vector<LootInfo> loots_info_;
	std::string map_id_;
	unsigned int player_id = 0;
	model::Map* map_{};
	std::shared_ptr<loot_gen::LootGenerator> lootGen_;
};

}
