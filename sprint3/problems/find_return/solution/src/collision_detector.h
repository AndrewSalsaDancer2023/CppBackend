#pragma once
#include <algorithm>
#include <vector>
#include <cassert>
namespace collision_detector {

struct Point2D {
    Point2D(double _x, double _y) : x{_x}, y{_y} {}
    double x, y;
};

struct CollectionResult {

    CollectionResult(double distance, double ratio)
        :sq_distance{distance}, proj_ratio{ratio}
    {}

    bool IsCollected(double collect_radius) const {
        return proj_ratio >= 0 && proj_ratio <= 1 && sq_distance <= collect_radius * collect_radius;
    }

    // квадрат расстояния до точки
    double sq_distance;

    // доля пройденного отрезка
    double proj_ratio;
};

// Движемся из точки a в точку b и пытаемся подобрать точку c.
// Эта функция реализована в уроке.
//CollectionResult TryCollectPoint(geom::Point2D a, geom::Point2D b, geom::Point2D c);
CollectionResult TryCollectPoint(Point2D a, Point2D b, Point2D c) {
    // Проверим, что перемещение ненулевое.
    // Тут приходится использовать строгое равенство, а не приближённое,
    // пскольку при сборе заказов придётся учитывать перемещение даже на небольшое
    // расстояние.
    assert(b.x != a.x || b.y != a.y);
    const double u_x = c.x - a.x;
    const double u_y = c.y - a.y;
    const double v_x = b.x - a.x;
    const double v_y = b.y - a.y;
    const double u_dot_v = u_x * v_x + u_y * v_y;
    const double u_len2 = u_x * u_x + u_y * u_y;
    const double v_len2 = v_x * v_x + v_y * v_y;
    const double proj_ratio = u_dot_v / v_len2;
    const double sq_distance = u_len2 - (u_dot_v * u_dot_v) / v_len2;

    return CollectionResult(sq_distance, proj_ratio);
}

struct Item {
    Item(const Point2D& pos, double wdth) : position{pos}, width{wdth} {}
    Point2D position;
    double width;
};

struct Gatherer {
    Gatherer(const Point2D& start, const Point2D& end, double wdth)
        : start_pos{start}, end_pos{end}, width{wdth}  {}
    Point2D start_pos;
    Point2D end_pos;
    double width;
};

class ItemGathererProvider {
protected:
    ~ItemGathererProvider() = default;

public:
    virtual size_t ItemsCount() const = 0;
    virtual Item GetItem(size_t idx) const = 0;
    virtual size_t GatherersCount() const = 0;
    virtual Gatherer GetGatherer(size_t idx) const = 0;
};

struct GatheringEvent {
    GatheringEvent(size_t itm_id, size_t gath_id, double distance, double tm)
        :item_id{itm_id}, gatherer_id{gath_id}, sq_distance{distance}, time{tm}
    {}
    size_t item_id;
    size_t gatherer_id;
    double sq_distance;
    double time;
};

class ItemGatherer: public ItemGathererProvider {
public:
    void AddItem(const Item& item) {
        items_.push_back(item);
    }
     size_t ItemsCount() const override  {
        return items_.size();
    }
    Item GetItem(size_t idx) const override {
        return  items_[idx];
    }

    void AddGatherer(const Gatherer& gatherer) {
        gatherers_.push_back(gatherer);
    }

    size_t GatherersCount() const override {
        return gatherers_.size();
    }

    Gatherer GetGatherer(size_t idx) const override {
        return gatherers_[idx];
    }
private:
    std::vector<Item> items_;
    std::vector<Gatherer> gatherers_;
};

// Эту функцию вам нужно будет реализовать в соответствующем задании.
// При проверке ваших тестов она не нужна - функция будет линковаться снаружи.
std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider) {

    std::vector<GatheringEvent> res;
    for(size_t i = 0; i < provider.GatherersCount(); ++i)
    {
        const auto& cur_gath = provider.GetGatherer(i);
        for(size_t j = 0; j < provider.ItemsCount(); ++j)
        {
            const auto& item = provider.GetItem(j);
            auto col = TryCollectPoint(cur_gath.start_pos, cur_gath.end_pos, item.position);

            if(col.IsCollected((cur_gath.width + item.width) / 2.0))
            {
                GatheringEvent event(j, i, col.sq_distance, col.proj_ratio);
                res.push_back(event);
            }
        }
    }
    return res;
}

}  // namespace collision_detector

