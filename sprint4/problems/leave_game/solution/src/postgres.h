#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>
#include "model.h"

namespace postgres {
/*
class RetiredRepository {
public:
    virtual void Save(const model::PlayerRecordItem& retired) = 0;
    virtual std::vector<model::PlayerRecordItem> GetRetired() = 0;
protected:
    ~RetiredRepository() = default;
};
*/
class RetiredRepositoryImpl //: public RetiredRepository {
{
public:
    explicit RetiredRepositoryImpl(pqxx::connection& connection)
        : connection_{connection}
    {}

    void SaveRetired(const model::PlayerRecordItem& retired);
    std::vector<model::PlayerRecordItem> GetRetired(int start = 0, int max_items = 0);
private:
    pqxx::connection& connection_;
};

class Database {
public:
    explicit Database(pqxx::connection connection);
//
//    RetiredRepositoryImpl& GetRetired() {
//        return retired_;
//    }

    void CreateTable();
private:
    pqxx::connection connection_;
 //   RetiredRepositoryImpl retired_{connection_};
};

}  // namespace postgres
