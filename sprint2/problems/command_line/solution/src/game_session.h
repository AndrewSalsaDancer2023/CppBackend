#pragma once
#include <vector>
#include "dog.h"
#include <string>
#include <memory>
#include <map>

using namespace std;

class Dog;

namespace model
{
class Map;

class Player
{
public:
  Player(unsigned int id, const std::string& name, const std::string& token, const model::Map* map);
  const std::string& GetToken() { return token_;}
  const std::string& GetName() { return name_;}
  unsigned int GetId() {return id_;}
//  void CreateDog();
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
	GameSession(const std::string& map_id) : map_(map_id) {}
//	void AddDog(const Dog& dog);
	std::shared_ptr<Player> AddPlayer(const std::string player_name, const model::Map* map);
	const std::string& GetMap() {return map_;}
	bool HasPlayerWithAuthToken(const std::string& auth_token);
	const std::vector<std::shared_ptr<Player>> GetAllPlayers(const std::string& auth_token);
	std::shared_ptr<Player> GetPlayerWithAuthToken(const std::string& auth_token);
	void MoveDogs(int deltaTime);

private:

	vector<std::shared_ptr<Player>> players_;
	std::string map_;
	unsigned int player_id = 0;
};

}
