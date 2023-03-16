#include "dog.h"
#include <map>
#include "server_exceptions.h"

namespace model
{
	void Dog::SetSpeed(model::DogDirection dir, double default_speed)
	{
		double speed = default_speed;
		std::map<model::DogDirection, DogVelocity> velMap{{DogDirection::EAST, {speed, 0}},
														   {DogDirection::WEST, {-speed, 0}},
														   {DogDirection::SOUTH, {0, speed}},
														   {DogDirection::NORTH, {0, -speed}},
														   {DogDirection::STOP, {0, 0}}};

		auto findVel = velMap.find(dir);
		if(findVel == velMap.end())
			throw DogSpeedException();

		velocity_ = findVel->second;
	}
}
