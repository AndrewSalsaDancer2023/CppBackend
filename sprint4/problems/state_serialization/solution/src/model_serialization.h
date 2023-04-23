#pragma once
#include <boost/serialization/vector.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <sstream>
#include <fstream>
#include <iostream>
#include "model.h"
#include "dog.h"
#include "game_session.h"
#include "geom.h"
#include <chrono>

namespace geom {

template <typename Archive>
void serialize(Archive& ar, Point2D& point, [[maybe_unused]] const unsigned version) {
//	std::cout << "Point2D serialized" << std::endl;
    ar& point.x;
    ar& point.y;
}

template <typename Archive>
void serialize(Archive& ar, Vec2D& vec, [[maybe_unused]] const unsigned version) {
	//std::cout << "Vec2D serialized" << std::endl;
    ar& vec.x;
    ar& vec.y;
}

}  // namespace geom

namespace model {
/*
template <typename Archive>
void serialize(Archive& ar, FoundObject& obj, [[maybe_unused]] const unsigned version) {
    ar&(*obj.id);
    ar&(obj.type);
}
*/
template <typename Archive>
void serialize(Archive& ar, DogPosition& pos, [[maybe_unused]] const unsigned version) {
	//std::cout << "DogPosition serialized" << std::endl;
    ar& pos.x;
    ar& pos.y;
}

template <typename Archive>
void serialize(Archive& ar, DogSpeed& speed, [[maybe_unused]] const unsigned version) {
	//std::cout << "DogSpeed serialized" << std::endl;
    ar& speed.vx;
    ar& speed.vy;
}

template <typename Archive>
void serialize(Archive& ar, DogPos& pos, [[maybe_unused]] const unsigned version) {
//	std::cout << "DogPos serialized" << std::endl;
//    ar& speed.vx;
//    ar& speed.vy;

	ar&  pos.current_road_index;
    ar& pos.curr_position;
    ar& pos.curr_speed;


}

/*
template <typename Archive>
void serialize(Archive& ar, DogState& state, [[maybe_unused]] const unsigned version) {
    ar& state.direction_;
    ar& state.speed_;
    ar& state.bag_capacity_;
    ar& state.score_;
    ar& state.gathered_loots_;
}
*/
template <typename Archive>
void serialize(Archive& ar, LootInfo& loot, [[maybe_unused]] const unsigned version) {
    ar& loot.id;
    ar& loot.type;
    ar& loot.x;
    ar& loot.y;
}
/*
template <typename Archive>
void serialize(Archive& ar, PlayerState& state, [[maybe_unused]] const unsigned version) {
    ar& state.name_;
    ar& state.token_;
    ar& state.id_;
}*/
/*
template <typename Archive>
void serialize(Archive& ar, GameSessionState& state, [[maybe_unused]] const unsigned version) {
    ar& state.map_id_;
    ar& state.player_id_;

    ar& state.player_state_;
    ar& state.loots_info_state;
    for(auto& plState : state.player_state_)
    	ar& plState;

    for(auto& loot : state.loots_info_state)
     	ar& loot;


}
*/
}  // namespace model

namespace /*serialization*/ {

using InputArchive = boost::archive::text_iarchive;
using OutputArchive = boost::archive::text_oarchive;
using TimeInterval = std::chrono::milliseconds;
//std::ofstream ofs("filename.arch");
TimeInterval time_without_saving_{};

void DeserializeSessions(model::Game& game)
{
	model::GameSessionsStates states;
	if(std::filesystem::exists(game.GetSavePath()))
	{
		std::ifstream file{game.GetSavePath()};
		InputArchive ia{file};
		ia >> states;
	}
	game.RestoreSessions(states);
}


void SerializeSessions(const model::Game& game)
{
	 std::stringstream ss;
	 OutputArchive oa{ss};
	 oa << *game.GetGameSessionsStates();
	 //oa << session.GetState();
	 std::ofstream ofs(game.GetSavePath());//"filename.arch");
	 ofs << ss.str();
}

void SerializeSessions(const model::Game& game, int deltaTime, int savePeriod)
{
	time_without_saving_ += TimeInterval{deltaTime};
	if(time_without_saving_ < TimeInterval{savePeriod})
		return ;

	SerializeSessions(game);

	time_without_saving_ = {};
}

void SerializeGameSession(const model::GameSession& session)
{
	 std::stringstream ss;
	 OutputArchive oa{ss};

	 oa << session.GetState();
	 std::ofstream ofs("filename.arch");
	 ofs << ss.str();
}

model::GameSessionState DeserializeGameSession()
{
	std::stringstream ss;
	InputArchive ia{ss};

	model::GameSessionState state;

	ia >> state;
	return state;
}

}  // namespace serialization

/*
namespace serialization {

class DogRepr {
public:
    DogRepr() = default;

    explicit DogRepr(const model::Dog& dog)
        : id_(dog.GetId())
        , name_(dog.GetName())
        , pos_(dog.GetPosition())
        , bag_capacity_(dog.GetBagCapacity())
        , speed_(dog.GetSpeed())
        , direction_(dog.GetDirection())
        , score_(dog.GetScore())
        , bag_content_(dog.GetBagContent()) {
    }

    [[nodiscard]] model::Dog Restore() const {
        model::Dog dog{id_, name_, pos_, bag_capacity_};
        dog.SetSpeed(speed_);
        dog.SetDirection(direction_);
        dog.AddScore(score_);
        for (const auto& item : bag_content_) {
            if (!dog.PutToBag(item)) {
                throw std::runtime_error("Failed to put bag content");
            }
        }
        return dog;
    }

    template <typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
        ar&* id_;
        ar& name_;
        ar& pos_;
        ar& bag_capacity_;
        ar& speed_;
        ar& direction_;
        ar& score_;
        ar& bag_content_;
    }

private:
    model::Dog::Id id_ = model::Dog::Id{0u};
    std::string name_;
    geom::Point2D pos_;
    size_t bag_capacity_ = 0;
    geom::Vec2D speed_;
    model::Direction direction_ = model::Direction::NORTH;
    model::Score score_ = 0;
    model::Dog::BagContent bag_content_
};

}  // namespace serialization
*/