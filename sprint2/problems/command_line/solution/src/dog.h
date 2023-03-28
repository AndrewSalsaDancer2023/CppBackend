#pragma once
#include <utility>
#include <memory>
#include "model.h"
#include <optional>
//#include <iostream>
namespace model {
struct Point;
class Map;
class Road;
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
    size_t current_road_index;
    DogPosition curr_position{0, 0};
    DogSpeed curr_speed{0, 0};
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
    DogNavigator(const std::vector<model::Road>& roads) : roads_(roads)
    {
  //  	std::cout << "DogNavigator roads:" << roads.size() << std::endl;
        adjacent_roads_ = std::vector<std::vector<RoadInfo>>(roads.size());
        FindAdjacentRoads();

        SetStartPositionFirstRoad();
    }
public:
    void MoveDog(DogDirection direction, DogSpeed speed, int time);
    DogPos GetDogPos() { return  dog_info_;}
    DogPosition GetDogPosition() { return dog_info_.curr_position;}
    DogSpeed GetDogSpeed() { return dog_info_.curr_speed;}
    void SpawnDogInMap(bool spawn_in_random_point);
    void SetDogSpeed(const DogSpeed& speed) { dog_info_.curr_speed = speed;}
private:
    bool RoadsCrossed(const Road& road1, const Road& road2);
    void FindAdjacentRoads();
    bool RoadsAdjacent(const Road& road1, const Road& road2);
    void SetStartPositionFirstRoad();
    void SetStartPositionRandomRoad();
    std::optional<size_t> FindNearestHorizontalCrossRoad(const DogPosition& newPos);
    std::optional<size_t> FindNearestVerticalCrossRoad(const DogPosition& newPos);
    std::optional<size_t> FindNearestAdjacentRoad(const Point& edge_point, bool is_horizontal_road);
    void FindNewPosMovingHorizontal(const Road& road, DogDirection direction, DogPosition& newPos, DogSpeed speed);
    void FindNewPosMovingVertical(const Road& road, DogDirection direction, DogPosition& newPos, DogSpeed speed);
    void FindNewPosCrossMovingHorizontal(const Road& road, DogDirection direction, DogPosition& newPos);
    void FindNewPosCrossMovingVertical(const Road& road, DogDirection direction, DogPosition& newPos);
    std::optional<size_t> FindNearestAdjacentVerticalRoad(const DogPosition& edge_point);
    std::optional<size_t> FindNearestAdjacentHorizontalRoad(const DogPosition& edge_point);
private:
 const std::vector<model::Road>& roads_;
 std::vector<std::vector<RoadInfo>> adjacent_roads_;
 DogPos dog_info_;
 };

class Dog
{
public:
	Dog(const model::Map *map);
//	void SetPosition(const DogPosition& pos) {position_ = pos;}
	void SetSpeed(model::DogDirection dir, double speed);
	void SetDirection(const DogDirection& dir) { direction_ = dir;}
	void Move(int deltaTime);
//	void PlaceToMap();
	DogDirection GetDirection() {return direction_;}
	DogPosition GetPosition() {return navigator_->GetDogPosition();}
	DogSpeed GetSpeed() {return navigator_->GetDogSpeed();}//{return speed_;}
	void SpawnDogInMap(bool spawn_in_random_point) {navigator_->SpawnDogInMap(spawn_in_random_point);}
private:
	DogDirection direction_;
	//DogPosition position_{0.0, 0.0};
	DogSpeed speed_{0.0, 0.0};
	const model::Map* map_;
	std::shared_ptr<DogNavigator> navigator_;
};


}


