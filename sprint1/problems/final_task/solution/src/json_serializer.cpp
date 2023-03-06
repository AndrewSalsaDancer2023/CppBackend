#include "json_serializer.h"
#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <boost/json.hpp>

namespace json = boost::json;
using namespace std::literals;

namespace json_serializer {

   std::string MakeErrorResponce(const std::string& codeMessage, const std::string& errorMessage)
   {
        json::object resp_object;
        resp_object["code"] = codeMessage;
        resp_object["message"] = errorMessage;
      	return json::serialize(resp_object);
   }

   std::string MakeMapNotFoundResponce()
   {
	return	MakeErrorResponce("mapNotFound", "Map not found");
   }
   
   std::string MakeBadRequestResponce()
   {
	return	MakeErrorResponce("badRequest", "Bad request");   
   }

    void SerializeOffices(const model::Map& map ,json::object& root)
    {
	json::array offices_ar;
	for(const auto& office : map.GetOffices())
	{
		json::object office_obj;

		office_obj["id"] = *office.GetId();
		office_obj["x"] = office.GetPosition().x;
		office_obj["y"] = office.GetPosition().y;
		office_obj["offsetX"] = office.GetOffset().dx;
		office_obj["offsetY"] = office.GetOffset().dy;
		
		offices_ar.emplace_back(office_obj);
	}
	root["offices"] = offices_ar;
    }

    void SerializeBuildings(const model::Map& map ,json::object& root)
    {
	json::array buildings_ar;
	for(const auto& building : map.GetBuildings())
	{
		json::object building_obj;

		const auto& bounds = building.GetBounds();

		building_obj["x"] = bounds.position.x;
		building_obj["y"] = bounds.position.y;
		building_obj["w"] = bounds.size.width;
		building_obj["h"] = bounds.size.height;
		
		buildings_ar.emplace_back(building_obj);
	}
	root["buildings"] = buildings_ar;
    }

    void SerializeRoads(const model::Map& map ,json::object& root)
    {
	json::array roads_ar;
	for(const auto& road : map.GetRoads())
	{
            json::object road_obj;
		
            model::Point start = road.GetStart();
            model::Point end = road.GetEnd();
		
            road_obj["x0"] = start.x;
            road_obj["y0"] = start.y;
		
            if(road.IsHorizontal())
                road_obj["x1"] = end.x;
            else
                road_obj["y1"] = end.y;

            roads_ar.emplace_back(road_obj);
	}
	root["roads"] = roads_ar;
    }

    std::string GetMapListResponce(const model::Game& game)
    {
        json::array map_ar;
        for( const auto& map: game.GetMaps())
        {
            json::object map_obj;
	
            map_obj[ "id" ] = *map.GetId();
            map_obj[ "name" ] = map.GetName();
            map_ar.emplace_back(map_obj);
        }
  
        return json::serialize(map_ar);
    }

    std::string GetMapContentResponce(const model::Game& game, const std::string& map_id)
    {
	const model::Map* mapFound = game.FindMap(model::Map::Id(map_id));

	if(!mapFound)
            return ("");

	 json::object root;

	root[ "id" ] = *mapFound->GetId() ;  
	root["name"] = mapFound->GetName();

 	SerializeRoads(*mapFound, root);
 	SerializeBuildings(*mapFound, root);
 	SerializeOffices(*mapFound, root);
 	
 	return json::serialize(root);
    }

}  // namespace json_serializer
