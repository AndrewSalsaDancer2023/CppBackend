#pragma once

#include <string>

#include "model.h"

namespace json_serializer {
std::string MakeBadRequestResponce();
std::string MakeMapNotFoundResponce();
std::string GetMapListResponce(const model::Game& game);
std::string GetMapContentResponce(const model::Game& game, const std::string& map_id);

}  // namespace json_serializer
