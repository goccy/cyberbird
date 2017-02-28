#include <cyberbird/storage.h>
#include <cyberbird/util.h>
#include <gtest/gtest.h>
#include "fixture/fixture.h"

using namespace cyberbird;

class StorageTest : public ::testing::Test, public Fixture{
public:
    char *DB_PATH;

    virtual void SetUp() {
        this->DB_PATH = dbpath();
        if (!this->DB_PATH) {
            EXPECT_TRUE(false);
        }
    }

    virtual void TearDown() {
        unlink(this->DB_PATH);
    }
};

TEST_F(StorageTest, createTable) {
    Table::Builder personBuilder("person");
    Table *personTable = personBuilder.addStringColumn("name", 16)->addNumberColumn("age")->build();

    Table::Builder eventBuilder("event");
    Table *eventTable = eventBuilder.addStringColumn("name", 16)->addNumberColumn("desc")->build();

    Storage storage(DB_PATH);
    EXPECT_TRUE(storage.createTable(personTable));
    EXPECT_TRUE(storage.createTable(eventTable));
    EXPECT_EQ(storage.table("person"), personTable);
    EXPECT_EQ(storage.table("event"), eventTable);
}

TEST_F(StorageTest, insert) {
    Table::Builder personBuilder("person");
    Table *personTable = personBuilder.addStringColumn("name", 16)->addNumberColumn("age")->build();

    Storage storage(DB_PATH);
    EXPECT_TRUE(storage.createTable(personTable));

    cyberbird::object o;
    o.insert(std::make_pair("name", cyberbird::value("bob")));
    o.insert(std::make_pair("age", cyberbird::value(20)));
    EXPECT_GT(personTable->insert(SIBUYA_STATION_LAT, SIBUYA_STATION_LON, o), 0);
}

TEST_F(StorageTest, flush_load) {
    Table::Builder personBuilder("person");
    Table *personTable = personBuilder.addStringColumn("name", 16)->addNumberColumn("age")->build();

    Storage storage(DB_PATH);

    cyberbird::object o;
    o.insert(std::make_pair("name", cyberbird::value("bob")));
    o.insert(std::make_pair("age", cyberbird::value(20)));

    storage.createTable(personTable);
    personTable->insert(SIBUYA_STATION_LAT, SIBUYA_STATION_LON, o);
    storage.flush();

    Storage newStorage(DB_PATH);
    Table *loadedPersonTable = newStorage.table("person");

    cyberbird::array people = loadedPersonTable->select(SIBUYA_STATION_LAT, SIBUYA_STATION_LON, 1);
    EXPECT_EQ(people.size(), 1);
    cyberbird::object person = people[0].get<cyberbird::object>();
    EXPECT_EQ(person["name"].get<std::string>(), "bob");
    EXPECT_EQ(person["age"].get<double>(), 20);
}
