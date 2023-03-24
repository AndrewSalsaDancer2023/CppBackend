#include "dog.h"
#include <map>
#include <vector>
#include <exception>
#include "server_exceptions.h"
#include <cmath>
#include <iostream>
constexpr float dE = 0.0001f;
constexpr float dS = 0.4f;

namespace model
{

	Dog::Dog(const model::Map *map) : map_(map)
	{
		direction_ = DogDirection::NORTH;
		navigator_ = std::make_shared<DogNavigator>(map_->GetRoads());
	}

	void Dog::SetSpeed(model::DogDirection dir, float default_speed)
	{
		double speed = default_speed;
		std::map<model::DogDirection, DogSpeed> velMap{{DogDirection::EAST, {speed, 0}},
														   {DogDirection::WEST, {-speed, 0}},
														   {DogDirection::SOUTH, {0, speed}},
														   {DogDirection::NORTH, {0, -speed}},
														   {DogDirection::STOP, {0, 0}}};

		auto findVel = velMap.find(dir);
		if(findVel == velMap.end())
			throw DogSpeedException();

		direction_ = dir;
		speed_ = findVel->second;
	}

	void Dog::Move(int deltaTime)
	{
		std::cout << "Move dog:" << std::endl;
		navigator_->MoveDog(direction_, speed_, deltaTime);
	}

	bool DogNavigator::RoadsCrossed(const Road& road1, const Road& road2)
	{
	    if((road1.IsHorizontal() && road2.IsVertical()) || (road1.IsVertical() && road2.IsHorizontal()))
	    {
	        const auto& first_start = road1.GetStart();

	        const auto& second_start = road2.GetStart();
	        const auto& second_end = road2.GetEnd();

	        if(road1.IsHorizontal())
	        {
	            if(((second_start.y < first_start.y) && (second_end.y < first_start.y)) ||
	              ((second_start.y > first_start.y) && (second_end.y > first_start.y)))
	                    return false;
	        }
	        else
	            if(road1.IsVertical())
	            {
	                if(((second_start.x < first_start.x) && (second_end.x < first_start.x)) ||
	                  ((second_start.x > first_start.x) && (second_end.x > first_start.x)))
	                    return false;

	            }
	        return true;
	    }
	    return false;
	}

	bool DogNavigator::RoadsAdjacent(const Road& road1, const Road& road2)
	{
	    if((road1.IsHorizontal() && road2.IsHorizontal()) || (road1.IsVertical() && road2.IsVertical()))
	    {
	        auto first_start = road1.GetStart();
	        auto first_end = road1.GetEnd();

	        auto second_start = road2.GetStart();
	        auto second_end = road2.GetEnd();

	        if((first_start == second_start) || (first_start == second_end) ||
	            (first_end == second_start) || (first_end == second_end))
	            return true;
	    }
	    return false;
	}

	void DogNavigator::FindAdjacentRoads()
	{
	    for(size_t i = 0; i < roads_.size(); ++i)
	    {
	        for(size_t j = i+1; j < roads_.size(); ++j)
	        {
	            RoadType road_type{RoadType::Parallel};

	            if(RoadsAdjacent(roads_[i], roads_[j]))
	                road_type = RoadType::Adjacent;
	            else
	                if(RoadsCrossed(roads_[i], roads_[j]))
	                    road_type = RoadType::Crossed;

	            if(road_type != RoadType::Parallel)
	            {
	                adjacent_roads_[i].push_back(RoadInfo(j,road_type));
	                adjacent_roads_[j].push_back(RoadInfo(i,road_type));
	            }
	        }
	    }
	}

	void DogNavigator::FindStartPositionOnMap()
	{
		dog_info_.current_road_index = 0;
		auto start = roads_[dog_info_.current_road_index].GetStart();
		dog_info_.curr_position = DogPosition(start.x, start.y);
	}

	std::optional<size_t> DogNavigator::FindNearestAdjacentRoad(const Point& edge_point, bool is_horizontal_road)
	{
	    std::optional<size_t> res;

	    const auto& adj_roads = adjacent_roads_[dog_info_.current_road_index];
	    for(const auto& road_info : adj_roads)
	    {
	        if(road_info.road_type != RoadType::Adjacent)
	            continue;
	        const auto& adj_road = roads_[dog_info_.current_road_index];
	        if(is_horizontal_road)
	        {
	            if(((adj_road.GetStart() == edge_point) && (adj_road.GetEnd().y == edge_point.y)) ||
	               ((adj_road.GetEnd() == edge_point) && (adj_road.GetStart().y == edge_point.y)))
	            {
	                    res = road_info.road_index;
	                    return res;
	            }
	        }
	        else
	        {
	            if(((adj_road.GetStart() == edge_point) && (adj_road.GetEnd().x == edge_point.x)) ||
	               ((adj_road.GetEnd() == edge_point) && (adj_road.GetStart().x == edge_point.x)))
	            {
	                    res = road_info.road_index;
	                    return res;
	            }
	        }
	    }

	    return res;
	}

	std::optional<size_t> DogNavigator::FindNearestAdjacentVerticalRoad(const DogPosition& edge_point)
	{
	    std::optional<size_t> res;

	    const auto& adj_roads = adjacent_roads_[dog_info_.current_road_index];
	    for(const auto& road_info : adj_roads)
	    {
	        if(road_info.road_type != RoadType::Adjacent)
	            continue;

	        const auto& adj_road = roads_[dog_info_.current_road_index];
	        if(!adj_road.IsVertical())
	            continue;

	        float dist1 = std::abs((float)adj_road.GetStart().x - edge_point.x);
	        float dist2 = std::abs((float)adj_road.GetEnd().x - edge_point.x);

	        if((dist1 <= 2*dS) || (dist2 <= 2*dS))
	        {
	             res = road_info.road_index;
	             return res;
	        }
	    }

	    return res;
	}

	std::optional<size_t> DogNavigator::FindNearestAdjacentHorizontalRoad(const DogPosition& edge_point)
	{
	    std::optional<size_t> res;

	    const auto& adj_roads = adjacent_roads_[dog_info_.current_road_index];
	    for(const auto& road_info : adj_roads)
	    {
	        if(road_info.road_type != RoadType::Adjacent)
	            continue;

	        const auto& adj_road = roads_[dog_info_.current_road_index];
	        if(!adj_road.IsHorizontal())
	            continue;

	        float dist1 = std::abs((float)adj_road.GetStart().y - edge_point.y);
	        float dist2 = std::abs((float)adj_road.GetEnd().y - edge_point.y);

	        if((dist1 <= 2*dS) || (dist2 <= 2*dS))
	        {
	             res = road_info.road_index;
	             return res;
	        }
	    }

	    return res;
	}

	std::optional<size_t> DogNavigator::FindNearestHorizontalCrossRoad(const DogPosition& newPos)
	{
	    std::optional<size_t> res;

	    const auto& adj_roads = adjacent_roads_[dog_info_.current_road_index];
	    for(const auto& road_info : adj_roads)
	    {
	        if(road_info.road_type != RoadType::Crossed)
	            continue;

	        const auto& adj_road = roads_[road_info.road_index];

	        if((newPos.x < (double)adj_road.GetStart().x) && (newPos.x < (double)adj_road.GetEnd().x))
	            continue;

	        if((newPos.x < (double)adj_road.GetStart().x) && (newPos.x < (double)adj_road.GetEnd().x))
	            continue;

	        float dist1 = std::abs((double)adj_road.GetStart().y - newPos.y);
	        float dist2 = std::abs((double)adj_road.GetEnd().y - newPos.y);

	        if((dist1 <= 2*dS) || (dist2 <= 2*dS))
	        {
	             res = road_info.road_index;
	             return res;
	        }
	    }

	    return res;
	}

	std::optional<size_t> DogNavigator::FindNearestVerticalCrossRoad(const DogPosition& newPos)
	{
	    std::optional<size_t> res;

	    const auto& adj_roads = adjacent_roads_[dog_info_.current_road_index];
	    for(const auto& road_info : adj_roads)
	    {
	        if(road_info.road_type != RoadType::Crossed)
	            continue;

	        const auto& adj_road = roads_[road_info.road_index];//dog_info_.current_road_index];

	        if((newPos.y < (float)adj_road.GetStart().y) && (newPos.y < (float)adj_road.GetEnd().y))
	            continue;

	        if((newPos.y > (float)adj_road.GetStart().y) && (newPos.y > (float)adj_road.GetEnd().y))
	            continue;

	        float dist1 = std::abs((float)adj_road.GetStart().x - newPos.x);
	        float dist2 = std::abs((float)adj_road.GetEnd().x - newPos.x);

	        if((dist1 <= 2*dS) || (dist2 <= 2*dS))
	        {
	             res = road_info.road_index;
	             return res;
	        }
	    }

	    return res;
	}

	void DogNavigator::FindNewPosCrossMovingHorizontal(const Road& road, DogDirection direction, DogPosition& newPos)
	{
	 /*   auto start = road.GetStart();
	    auto end = road.GetEnd();

	    if(start.y > end.y)
	        std::swap(start, end);
	*/
	    std::optional<size_t> adjRoad = FindNearestAdjacentVerticalRoad(newPos);
	     bool findRoad = false;
	    if(adjRoad)
	    {
	        const auto& road_cand = roads_[*adjRoad];
	        if(direction == DogDirection::NORTH)
	        {
	            if((road_cand.GetStart().y <= road.GetStart().y) && (road_cand.GetEnd().y <= road.GetStart().y))
	                findRoad = true;
	        }
	        else
	        {
	            if((road_cand.GetStart().y >= road.GetStart().y) && (road_cand.GetEnd().y >= road.GetStart().y))
	                findRoad = true;
	        }
	/*        if(findRoad)
	        {
	            dog_info_.curr_position = newPos;
	            dog_info_.current_road_index = *adjRoad;
	            return;
	        }*/
	    }
	    adjRoad = FindNearestVerticalCrossRoad(newPos);
	    if(findRoad || adjRoad)
	    {
	        dog_info_.curr_position = newPos;
	        dog_info_.current_road_index = *adjRoad;
	        return;
	    }

	    if(std::abs(newPos.y - road.GetStart().y) > dS)
	    {
	        if(direction == DogDirection::NORTH)
	            newPos.y = road.GetStart().y - dS;
	        else
	            newPos.y = road.GetStart().y + dS;
	    }

	    dog_info_.curr_position = newPos;
	}

	void DogNavigator::FindNewPosCrossMovingVertical(const Road& road, DogDirection direction, DogPosition& newPos)
	{
	    std::optional<size_t> adjRoad = FindNearestAdjacentHorizontalRoad(newPos);
	     bool findRoad = false;
	    if(adjRoad)
	    {
	        const auto& road_cand = roads_[*adjRoad];
	        if(direction == DogDirection::WEST)
	        {
	            if((road_cand.GetStart().x <= road.GetStart().x) && (road_cand.GetEnd().x <= road.GetStart().x))
	                findRoad = true;
	        }
	        else
	        {
	            if((road_cand.GetStart().x >= road.GetStart().x) && (road_cand.GetEnd().x >= road.GetStart().x))
	                findRoad = true;
	        }
	    }
	    adjRoad = FindNearestHorizontalCrossRoad(newPos);
	    if(findRoad || adjRoad)
	    {
	        dog_info_.curr_position = newPos;
	        dog_info_.current_road_index = *adjRoad;
	        return;
	    }

	    if(std::abs(newPos.x - road.GetStart().x) > dS)
	    {
	        if(direction == DogDirection::WEST)
	            newPos.x = road.GetStart().x - dS;
	        else
	            newPos.x = road.GetStart().x + dS;
	    }

	    dog_info_.curr_position = newPos;
	}

	void DogNavigator::FindNewPosMovingVertical(const Road& road, DogDirection direction, DogPosition& newPos, DogSpeed speed)
	{
	    auto start = road.GetStart();
	    auto end = road.GetEnd();

	    if(start.y > end.y)
	        std::swap(start, end);

	    if(direction == DogDirection::NORTH)
	    {
	        if(newPos.y < (float)start.y)
	        {
	            std::optional<size_t> adjRoad = FindNearestAdjacentRoad(road.GetStart(), false);
	            if(adjRoad)
	            {
	                dog_info_.current_road_index = *adjRoad;
	            }
	            else
	                if(newPos.y < ((float)start.y - dS))
	                {
	                    newPos.y = ((float)start.y - dS);
	                    speed = {0.0f, 0.0f};
	                }
	        }
	        dog_info_.curr_position = newPos;
	    }
	    else
	    {
	        if(newPos.y > (float)end.y)
	        {
	            std::optional<size_t> adjRoad = FindNearestAdjacentRoad(road.GetEnd(), false);
	            if(adjRoad)
	            {
	                dog_info_.current_road_index = *adjRoad;
	            }
	            else
	                if(newPos.y > ((float)end.y + dS))
	                {
	                    newPos.y = ((float)end.y + dS);
	                    speed = {0.0f, 0.0f};
	                }
	        }
	        dog_info_.curr_position = newPos;
	    }
	}

	void DogNavigator::FindNewPosMovingHorizontal(const Road& road, DogDirection direction, DogPosition& newPos, DogSpeed speed)
	{
	    auto start = road.GetStart();
	    auto end = road.GetEnd();

	    if(start.x > end.x)
	        std::swap(start, end);

	    if(direction == DogDirection::WEST)
	    {
	        if(newPos.x < (float)start.x)
	        {
	            std::optional<size_t> adjRoad = FindNearestAdjacentRoad(road.GetStart(), true);
	            if(adjRoad)
	            {
	                dog_info_.current_road_index = *adjRoad;
	            }
	            else
	                if(newPos.x < ((float)start.x - dS))
	                {
	                    newPos.x = ((float)start.x - dS);
	                    speed = {0.0f, 0.0f};
	                }
	        }
	        dog_info_.curr_position = newPos;
	    }
	    else
	    {
	        if(newPos.x > (float)end.x)
	        {
	            std::optional<size_t> adjRoad = FindNearestAdjacentRoad(road.GetEnd(), true);
	            if(adjRoad)
	            {
	                dog_info_.current_road_index = *adjRoad;
	            }
	            else
	                if(newPos.x > ((float)end.x + dS))
	                {
	                    newPos.x = ((float)end.x + dS);
	                    speed = {0.0f, 0.0f};
	                }
	        }
	        dog_info_.curr_position = newPos;
	    }
	}

	void DogNavigator::MoveDog(DogDirection direction, DogSpeed speed, int time)
	{
	    const auto& road = roads_[dog_info_.current_road_index];

	    float dt = (float)time/1000;
	    DogPosition newPos{0.0, 0.0};

	    newPos.x = dog_info_.curr_position.x + dt * speed.vx;
	    newPos.y = dog_info_.curr_position.y + dt * speed.vy;

	    if(road.IsHorizontal())
	    {
	        if((direction == DogDirection::WEST) || (direction == DogDirection::EAST))
	            FindNewPosMovingHorizontal(road, direction, newPos, speed);        else
	            if((direction == DogDirection::NORTH) || (direction == DogDirection::SOUTH))
	            {
	                FindNewPosCrossMovingHorizontal(road, direction, newPos);
	            }
	    }
	    else
	    {
	        if((direction == DogDirection::NORTH) || (direction == DogDirection::SOUTH))
	            FindNewPosMovingVertical(road, direction, newPos, speed);
	        else
	            if((direction == DogDirection::WEST) || (direction == DogDirection::EAST))
	            {
	                FindNewPosCrossMovingVertical(road, direction, newPos);
	            }
	    }
	}
}
