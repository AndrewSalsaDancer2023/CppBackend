#pragma once
#include <utility>

namespace model {

using DogPosition = std::pair<double, double>;
using DogVelocity = std::pair<double, double>;
enum class DogDirection { NORTH, SOUTH, WEST, EAST };

class Dog
{
public:
	void SetPosition(const DogPosition& pos) {position_ = pos;}
	void SetVelocity(const DogVelocity& velocity) {velocity_ = velocity;}
	void SetDirection(const DogDirection& dir) { direction_ = dir;}
	DogDirection GetDirection() {return direction_;}
	DogPosition GetPosition() {return position_;}
	DogVelocity GetVelocity() {return velocity_;}
private:
	DogDirection direction_ = DogDirection::NORTH;
	DogPosition position_{0.0, 0.0};
	DogVelocity velocity_{0.0, 0.0};
};

}

