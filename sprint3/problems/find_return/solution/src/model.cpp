#include "model.h"
#include "server_exceptions.h"

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

void Map::AddLoot(Loot loot)
{
	loots_.emplace_back(std::move(loot));
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

size_t Game::GetNumPlayersInAllSessions()
{
	size_t players_coutner = 0;
		std::for_each(sessions_.begin(), sessions_.end(),[&players_coutner](std::shared_ptr<GameSession>& session){
			players_coutner += session->GetNumPlayers();
		});

	return players_coutner;
}


Game::PlayerAuthInfo Game::AddPlayer(const std::string& map_id, const std::string& player_name) {
    const Map* mapToAdd = FindMap(Map::Id(map_id));

   if(player_name.empty()) 
	    throw EmptyNameException();

   if(!mapToAdd)
	    throw MapNotFoundException();

    std::shared_ptr<GameSession> session = FindSession(map_id);
    if(!session)
    {
    	auto [loot_period, loot_probability] = GetLootParameters();
    	session = std::make_shared<GameSession>(map_id, loot_period, loot_probability);
    	sessions_.push_back(session);
    }
    auto player = session->AddPlayer(player_name, const_cast<Map*>(mapToAdd), spawn_in_random_points_, default_bag_capacity_);
    return {player->GetToken(), player->GetId()};
}

std::shared_ptr<GameSession> Game::GetSessionForToken(const std::string& auth_token)
{
	auto itFind = std::find_if(sessions_.begin(), sessions_.end(),[&auth_token](std::shared_ptr<GameSession>& session){
		return session->HasPlayerWithAuthToken(auth_token) == true;
	});

	if(itFind == sessions_.end())
		return std::shared_ptr<GameSession>();

	return (*itFind);
}

const std::vector<std::shared_ptr<Player>> Game::FindAllPlayersForAuthInfo(const std::string& auth_token)
{
/*	auto itFind = std::find_if(sessions_.begin(), sessions_.end(),[&auth_token](std::shared_ptr<GameSession>& session){
			return session->HasPlayerWithAuthToken(auth_token) == true;
		});

	if(itFind == sessions_.end())
		return {};
*/
	auto session = GetSessionForToken(auth_token);
	if(!session)
		return {};

	return session->GetAllPlayers();
}

const vector<LootInfo> Game::GetLootsForAuthInfo(const std::string& auth_token)
{
/*	auto itFind = std::find_if(sessions_.begin(), sessions_.end(),[&auth_token](std::shared_ptr<GameSession>& session){
			return session->HasPlayerWithAuthToken(auth_token) == true;
		});

	if(itFind == sessions_.end())
		return {};
*/
	auto session = GetSessionForToken(auth_token);
		if(!session)
			return {};

	return session->GetLootsInfo();
}

std::shared_ptr<Player> Game::GetPlayerWithAuthToken(const std::string& auth_token)
{
	auto itFind = std::find_if(sessions_.begin(), sessions_.end(),[&auth_token](std::shared_ptr<GameSession>& session){
			return session->HasPlayerWithAuthToken(auth_token) == true;
		});

	if(itFind == sessions_.end())
		throw PlayerAbsentException();

	return (*itFind)->GetPlayerWithAuthToken(auth_token);
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

std::shared_ptr<GameSession> Game::GetSessionWithAuthInfo(const std::string& auth_token)
{
	auto itFind = std::find_if(sessions_.begin(), sessions_.end(),[&auth_token](std::shared_ptr<GameSession>& session){
					return session->HasPlayerWithAuthToken(auth_token) == true;
				});

	if(itFind == sessions_.end())
		throw InvalidSessionException();

	return *itFind;
}

void Game::MoveDogs(int deltaTime)
{
	std::for_each(sessions_.begin(), sessions_.end(),[deltaTime](std::shared_ptr<GameSession>& session){
		session->MoveDogs(deltaTime);
	});
}

void Game::GenerateLoot(int deltaTime)
{
	std::for_each(sessions_.begin(), sessions_.end(),[this, deltaTime](std::shared_ptr<GameSession>& session){
		const auto& mapName =  session->GetMap();
		const Map* pMap = this->FindMap(model::Map::Id(mapName));
		if(pMap)
			session->GenerateLoot(deltaTime, pMap);
	});
}

void Game::SetLootParameters(double period, double probability)
{
	loot_period_ = period;
	loot_probability_ = probability;
}

}  // namespace model
