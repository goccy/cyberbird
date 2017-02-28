#include <cyberbird/cyberbird.h>
#include <gtest/gtest.h>
#include "fixture/fixture.h"

class CyberBirdTest : public ::testing::Test, public Fixture {
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

TEST_F(CyberBirdTest, main) {

    cyberbird::CyberBird cyberBird;
    cyberBird.wake(DB_PATH);

    cyberbird::Storage *storage = cyberBird.storage();

    cyberbird::Table::Builder builder("person");
    cyberbird::Table *table = builder.addStringColumn("name", 16)->addNumberColumn("age")->build();
    storage->createTable(table);

    {
        cyberbird::object o;
        o.insert(std::make_pair("name", cyberbird::value("bob")));
        o.insert(std::make_pair("age", cyberbird::value(20)));
        storage->table("person")->insert(SIBUYA_STATION_LAT, SIBUYA_STATION_LON, o);
    }

    cyberbird::array people = storage->table("person")->select(SIBUYA_STATION_LAT, SIBUYA_STATION_LON, 1);
    EXPECT_EQ(people.size(), 1);
    cyberbird::object person = people[0].get<cyberbird::object>();
    EXPECT_EQ(person["name"].get<std::string>(), "bob");
    EXPECT_EQ(person["age"].get<double>(), 20);
    cyberBird.sleep();
}
