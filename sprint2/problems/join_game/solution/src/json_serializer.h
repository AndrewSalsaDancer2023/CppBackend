#pragma once
#include <map>
#include <string>

#include "model.h"

namespace json_serializer {
std::string MakeBadRequestResponce();
std::string MakeMapNotFoundResponce();
std::string MakeAuthResponce(const std::string& auth_key, unsigned playerId);
std::string MakeMappedResponce(const std::map<std::string, std::string>& key_values);
std::string GetMapListResponce(const model::Game& game);
std::string GetMapContentResponce(const model::Game& game, const std::string& map_id);
std::string GetPlayerInfoResponce(const std::map<unsigned int, std::string>& players_info);
}  // namespace json_serializer
