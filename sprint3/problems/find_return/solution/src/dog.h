#pragma once
#include "model.h"
#include <optional>
#include <iostream>

namespace collision_detector {
	enum class ItemType;
	struct GatheringEvent;
	struct Gatherer;
}

namespace model {
struct Point;
class Map;
class Road;
struct LootInfo;

struct DogPosition
{
    DogPosition(double newx, double newy) : x{newx}, y{newy} {}
    double x{0.0};
    double y{0.0};
};

struct DogSpeed
{
    double vx{0.0};
    double vy{0.0};
};

struct DogPos
{
    size_t current_road_index{0};
    DogPosition curr_position{0.0, 0.0};
    DogSpeed curr_speed{0.0, 0.0};
};


enum class DogDirection { NORTH, SOUTH, WEST, EAST, STOP };
enum class RoadType{Parallel, Adjacent, Crossed};

struct RoadInfo
{
    size_t road_index{};
    RoadType road_type{RoadType::Parallel};
    RoadInfo(size_t index, RoadType rdType):road_index(index), road_type(rdType)  {}
};

std::string ConvertDogDirectionToString(model::DogDirection direction);

class DogNavigator {
public:
    DogNavigator(const std::vector<model::Road>& roads, bool spawn_dog_in_random_point) : roads_(roads)
    {
        adjacent_roads_ = std::vector<std::vector<RoadInfo>>(roads.size());
        FindAdjacentRoads();
        if(spawn_dog_in_random_point)
        {
        	SetStartPositionRandomRoad();
        }
        else
        {
        	SetStartPositionFirstRoad();
        }
    }
public:
    void MoveDog(DogDirection direction, DogSpeed speed, int time);
    model::DogPos GetDogPos() { return  dog_info_;}
    DogPosition GetDogPosition() { return dog_info_.curr_position;}
    DogSpeed GetDogSpeed() { return dog_info_.curr_speed;}
    void SpawnDogInMap(bool spawn_in_random_point);
    void SetDogSpeed(const DogSpeed& speed) { dog_info_.curr_speed = speed;}
private:
    void FindNewPosMovingHorizontal(const model::Road& road, DogPosition& newPos);
    void FindNewPosMovingVertical(const model::Road& road, DogPosition& newPos);
    //std::optional<size_t> FindNearestAdjacentRoad(const Point& edge_point, bool find_horizontal_road);
    void FindAdjacentRoads();
    void SetStartPositionFirstRoad();
    bool RoadsCrossed(const model::Road& road1, const model::Road& road2);
    bool RoadsAdjacent(const model::Road& road1, const model::Road& road2);
    std::optional<size_t> FindNearestAdjacentVerticalRoad(const DogPosition& edge_point);
    std::optional<size_t> FindNearestVerticalCrossRoad(const DogPosition& newPos);
    void FindNewPosPerpendicularHorizontal(const model::Road& road, DogDirection direction, DogPosition& newPos);
    std::optional<size_t> FindNearestHorizontalCrossRoad(const DogPosition& newPos);
    std::optional<size_t> FindNearestAdjacentHorizontalRoad(const DogPosition& edge_point);
    void FindNewPosPerpendicularVertical(const model::Road& road, DogDirection direction, DogPosition& newPos);
	void SetStartPositionRandomRoad();
	void CorrectDogPosition();
private:
    const std::vector<model::Road>& roads_;
    std::vector<std::vector<RoadInfo>> adjacent_roads_;
    model::DogPos dog_info_;
 };

class Dog
{
public:
	Dog(const model::Map *map, bool spawn_dog_in_random_point, unsigned defaultBagCapacity);
	void SetSpeed(model::DogDirection dir, double speed);
	void SetDirection(const DogDirection& dir) { direction_ = dir;}
	std::optional<collision_detector::Gatherer> Move(int deltaTime);
	DogDirection GetDirection() {return direction_;}
	DogPosition GetPosition() {return navigator_->GetDogPosition();}
	DogSpeed GetSpeed() {return navigator_->GetDogSpeed();}
	void SpawnDogInMap(bool spawn_in_random_point) {navigator_->SpawnDogInMap(spawn_in_random_point);}
	const std::vector<model::LootInfo>& GetGatheredLoot() { return gathered_loots_;}
	bool AddLoot(const model::LootInfo& loot);
	void PassLootToOffice();
	int GetScore() const noexcept { return score_;}
private:
	DogDirection direction_;
	DogSpeed speed_{0.0, 0.0};
	const model::Map* map_;
	std::shared_ptr<DogNavigator> navigator_;
	unsigned bag_capacity_{};
	std::vector<model::LootInfo> gathered_loots_;
	int score_{};
};


}


