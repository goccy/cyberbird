#include <cyberbird/storage.h>
#include <cyberbird/util.h>
#include <gtest/gtest.h>
#include <fcntl.h>

using namespace cyberbird;

class StorageTest : public ::testing::Test{};

TEST_F(StorageTest, createTable) {
    Table::Builder personBuilder("person");
    Table *personTable = personBuilder.addStringColumn("name", 16)->addNumberColumn("age")->build();

    Table::Builder eventBuilder("event");
    Table *eventTable = eventBuilder.addStringColumn("name", 16)->addNumberColumn("desc")->build();

    char path[PATH_MAX] = {0};
    std::string pathTemplate = "/tmp/storage.dbXXXXXX";
    int fd = mkstemp((char *)pathTemplate.c_str());
    if (fcntl(fd, F_GETPATH, path) == -1) {
        EXPECT_TRUE(false);
    }
    Storage storage((const char *)path);
    EXPECT_TRUE(storage.createTable(personTable));
    EXPECT_TRUE(storage.createTable(eventTable));
    EXPECT_EQ(storage.table("person"), personTable);
    EXPECT_EQ(storage.table("event"), eventTable);

    unlink(path);
}

TEST_F(StorageTest, insert) {
    Table::Builder personBuilder("person");
    Table *personTable = personBuilder.addStringColumn("name", 16)->addNumberColumn("age")->build();

    char path[PATH_MAX] = {0};
    std::string pathTemplate = "/tmp/storage.dbXXXXXX";
    int fd = mkstemp((char *)pathTemplate.c_str());
    if (fcntl(fd, F_GETPATH, path) == -1) {
        EXPECT_TRUE(false);
    }
    Storage storage((const char *)path);
    EXPECT_TRUE(storage.createTable(personTable));

    double latitude  = 35.65796;
    double longitude = 139.708928;
    cyberbird::object o;
    o.insert(std::make_pair("name", cyberbird::value("bob")));
    o.insert(std::make_pair("age", cyberbird::value(20)));
    EXPECT_GT(personTable->insert(latitude, longitude, o), 0);

    unlink(path);
}

TEST_F(StorageTest, flush_load) {
    Table::Builder personBuilder("person");
    Table *personTable = personBuilder.addStringColumn("name", 16)->addNumberColumn("age")->build();

    char path[PATH_MAX] = {0};
    std::string pathTemplate = "/tmp/storage.dbXXXXXX";
    int fd = mkstemp((char *)pathTemplate.c_str());
    if (fcntl(fd, F_GETPATH, path) == -1) {
        EXPECT_TRUE(false);
    }
    Storage storage((const char *)path);

    double latitude  = 35.65796;
    double longitude = 139.708928;
    cyberbird::object o;
    o.insert(std::make_pair("name", cyberbird::value("bob")));
    o.insert(std::make_pair("age", cyberbird::value(20)));

    storage.createTable(personTable);
    personTable->insert(latitude, longitude, o);
    storage.flush();

    Storage newStorage((const char *)path);
    Table *loadedPersonTable = newStorage.table("person");

    cyberbird::array people = loadedPersonTable->select(latitude, longitude, 1);
    EXPECT_EQ(people.size(), 1);
    cyberbird::object person = people[0].get<cyberbird::object>();
    EXPECT_EQ(person["name"].get<std::string>(), "bob");
    EXPECT_EQ(person["age"].get<double>(), 20);
    unlink(path);
}
