#include "json_loader.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/json.hpp>

namespace json = boost::json;

namespace json_loader {
   std::string MakeMapListResponce(const std::vector<std::pair<std::string, std::string>>& mapInfo)
   {
   	if(!mapInfo.size())
	   	return "";
	   	
	json::array map_ar;

	for(auto it = mapInfo.begin(); it != mapInfo.end(); ++it)
	{
	   json::object map_obj;
	   
           map_obj[ "id" ] = it->first;  
           map_obj[ "name" ] = it->second;  
        
      	   map_ar.emplace_back(map_obj);
        }
        
        return json::serialize(map_ar);
   }

model::Road ParseRoad(const json::object& road_object)
{
//	std::cout << "roads parsing:" << std::endl;
	if(road_object.contains("x1"))
	{
//	std::cout << "horizontalk road:" << std::endl;
	 model::Road road(model::Road::HORIZONTAL, model::Point(road_object.at("x0").as_int64(), 
	 							  road_object.at("y0").as_int64()), 
	 							  road_object.at("x1").as_int64());
	return road;
       }
 //	std::cout << "vertical road:" << std::endl;
	 model::Road road(model::Road::VERTICAL, model::Point(road_object.at("x0").as_int64(), 
	 							  road_object.at("y0").as_int64()), 
	 							  road_object.at("y1").as_int64());
	return road;
	
}

std::vector<model::Road> ParseRoads(const json::object& map_object)
{
	std::vector<model::Road> roads;
	
	 auto roadsArray = map_object.at("roads").as_array();
 
  //	 std::cout << "roads size:" << roadsArray.size() << std::endl;

	for(auto index = 0; index < roadsArray.size(); ++index)
	{
		auto curRoad = roadsArray.at(index).as_object();
		roads.emplace_back(ParseRoad(curRoad));
	}
	
	return roads;
}


model::Building ParseBuilding(const json::object& building_object)
{
//	std::cout << "building parsing:" << std::endl;
        model::Building building(model::Rectangle(model::Point(building_object.at("x").as_int64(),
        							  building_object.at("y").as_int64()), 
        					   model::Size(building_object.at("w").as_int64(),
 	           					        building_object.at("h").as_int64())));   
	return building;
}


std::vector<model::Building> ParseBuildings(const json::object& map_object)
{
	std::vector<model::Building> buildings;
	
	 auto buildingsArray = map_object.at("buildings").as_array();
 
 // 	 std::cout << "buildings size:" << buildingsArray.size() << std::endl;

	for(auto index = 0; index < buildingsArray.size(); ++index)
	{
		auto curBuilding = buildingsArray.at(index).as_object();
		buildings.emplace_back(ParseBuilding(curBuilding));
	}
	
	return buildings;
}

model::Office ParseOffice(const json::object& office_object)
{
//	std::cout << "office parsing:" << std::endl;
        model::Office office(model::Office::Id(office_object.at("id").as_string().data()),
        		     model::Point(office_object.at("x").as_int64(),
        				  office_object.at("y").as_int64()),
        		     model::Offset(office_object.at("offsetX").as_int64(),
		      		     	    office_object.at("offsetY").as_int64()));
	return office;
}


std::vector<model::Office> ParseOffices(const json::object& office_object)
{
	std::vector<model::Office> offices;
	
	 auto officesArray = office_object.at("offices").as_array();
 
  //	 std::cout << "offices size:" << officesArray.size() << std::endl;

	for(auto index = 0; index < officesArray.size(); ++index)
	{
		auto curOffice = officesArray.at(index).as_object();
		offices.emplace_back(ParseOffice(curOffice));
	}
	
	return offices;
}


model::Map ParseMapObject(const json::object& map_object)
{
	auto idVal = map_object.at("id").as_string();
	auto nameVal = map_object.at("name").as_string();
	
	model::Map map(model::Map::Id(idVal.data()), nameVal.data());
	auto roads = ParseRoads(map_object);

	for(const auto& road: roads)
	{
		map.AddRoad(road);
	}
	
	auto buildings = ParseBuildings(map_object);
	for(const auto& building: buildings)
	{
		map.AddBuilding(building);
	}
	
	auto offices = ParseOffices(map_object);
	for(const auto& office: offices)
	{
		map.AddOffice(office);
	}
	
	return map;
}

void ParseMaps(const std::filesystem::path& json_path, model::Game& game) 
{
/*    std::filesystem::path patch_directory{"../data/"};
    patch_directory += json_path;
    if(!exists(patch_directory))
    {
	std::cout << "path :" << patch_directory <<" does not exist" << std::endl;
    }
*/
    std::ifstream input(json_path);//patch_directory);
    std::stringstream sstr;

    while(input >> sstr.rdbuf());
    auto value = json::parse(sstr.str());
 
    auto maps = value.at("maps");
    
    auto arr = maps.as_array();

    for(auto index = 0; index < arr.size(); ++index)
    {
	auto curMap = arr.at(index).as_object();
	game.AddMap(ParseMapObject(curMap));
    }
}

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    model::Game game;
try {    

      ParseMaps(json_path, game);    
 } catch (const std::exception& ex) {
        std::cerr << "Exception:" << ex.what() << std::endl;
    }   

    return game;
}

}  // namespace json_loader
