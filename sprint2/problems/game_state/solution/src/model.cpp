#include "model.h"
#include "server_exceptions.h"
#include "algorithm"
#include <stdexcept>

namespace model {
using namespace std::literals;

void Map::AddOffice(Office office) {
    if (warehouse_id_to_index_.contains(office.GetId())) {
        throw std::invalid_argument("Duplicate warehouse");
    }

    const size_t index = offices_.size();
    Office& o = offices_.emplace_back(std::move(office));
    try {
        warehouse_id_to_index_.emplace(o.GetId(), index);
    } catch (...) {
        // Удаляем офис из вектора, если не удалось вставить в unordered_map
        offices_.pop_back();
        throw;
    }
}

void Game::AddMap(Map map) {
    const size_t index = maps_.size();
    if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
        throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
    } else {
        try {
            maps_.emplace_back(std::move(map));
        } catch (...) {
            map_id_to_index_.erase(it);
            throw;
        }
    }
}

std::shared_ptr<GameSession> Game::FindSession(const std::string& map_name)
{
	auto itFind = std::find_if(sessions_.begin(), sessions_.end(),[&map_name](std::shared_ptr<GameSession>& session){
		return session->GetMap() == map_name;
	});
	
	if(itFind != sessions_.end())
		return *itFind;
	
	std::shared_ptr<GameSession> res;
	return res;
}

Game::PlayerAuthInfo Game::AddPlayer(const std::string& map_id, const std::string& player_name) {
    const Map* mapToAdd = FindMap(Map::Id(map_id));

//    if(!mapToAdd)
//	    throw MapNotFoundException();

   if(player_name.empty()) 
	    throw EmptyNameException();

   if(!mapToAdd)
	    throw MapNotFoundException();

    std::shared_ptr<GameSession> session = FindSession(map_id);
    if(!session)
    	session = std::make_shared<GameSession>(map_id);
    
    auto player = session->AddPlayer(player_name);
    sessions_.push_back(session);
    
    return {player->GetToken(), player->GetId()};
}

const std::vector<std::shared_ptr<Player>> Game::FindAllPlayersForAuthInfo(const std::string& auth_token)
{
	auto itFind = std::find_if(sessions_.begin(), sessions_.end(),[&auth_token](std::shared_ptr<GameSession>& session){
			return session->HasPlayerWithAuthToken(auth_token) == true;
		});

	if(itFind == sessions_.end())
		return {};

	return (*itFind)->GetAllPlayers(auth_token);
}

bool Game::HasSessionWithAuthInfo(const std::string& auth_token)
{
	auto itFind = std::find_if(sessions_.begin(), sessions_.end(),[&auth_token](std::shared_ptr<GameSession>& session){
				return session->HasPlayerWithAuthToken(auth_token) == true;
			});

	if(itFind == sessions_.end())
		return false;

	return true;
}

}  // namespace model
