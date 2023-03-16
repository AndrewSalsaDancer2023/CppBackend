#include "game_session.h"
#include "player_tokens.h"
#include <algorithm>
#include "model.h"
#include "server_exceptions.h"

namespace model
{
/*
void GameSession::AddDog(const model::Dog& dog) 
{ 
	dogs_.push_back(dog);
}
*/
std::shared_ptr<Player> GameSession::AddPlayer(const std::string player_name)
{

 auto itFind = std::find_if(players_.begin(), players_.end(), [&player_name](std::shared_ptr<Player>& player){
 	return player->GetName() == player_name;
 });

   if(itFind != players_.end())
	return *itFind;
	
   PlayerTokens tk;
   auto token = tk.GetToken();
   auto player = std::make_shared<Player>(player_id, player_name, token);

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

//std::map<unsigned int, std::string> GameSession::GetAllPlayersInfo(const std::string& auth_token)
const std::vector<std::shared_ptr<Player>> GameSession::GetAllPlayers(const std::string& auth_token)
{
	//std::vector<std::shared_ptr<Player>> result;

	//if(!HasPlayerWithAuthToken(auth_token))
	//	return {};

/*	std::for_each(players_.begin(), players_.end(), [&result](std::shared_ptr<Player>& player){
		 	result[player->GetId()] = player->GetName();
		 });
*/
	return players_;
}
/*
void Player::CreateDog()
{

}*/
}
