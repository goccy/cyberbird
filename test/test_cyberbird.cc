#include <cyberbird/cyberbird.h>
#include <gtest/gtest.h>
#include <fcntl.h>

class CyberBirdTest : public ::testing::Test{};

TEST_F(CyberBirdTest, main) {
    char path[PATH_MAX] = {0};
    std::string pathTemplate = "/tmp/cyberbird.dbXXXXXX";
    int fd = mkstemp((char *)pathTemplate.c_str());
    if (fcntl(fd, F_GETPATH, path) == -1) {
        EXPECT_TRUE(false);
    }

    cyberbird::CyberBird cyberBird;
    cyberBird.wake(path);

    cyberbird::Storage *storage = cyberBird.storage();

    cyberbird::Table::Builder builder("person");
    cyberbird::Table *table = builder.addStringColumn("name", 16)->addNumberColumn("age")->build();
    storage->createTable(table);

    {
        cyberbird::object o;
        o.insert(std::make_pair("name", cyberbird::value("bob")));
        o.insert(std::make_pair("age", cyberbird::value(20)));
        storage->table("person")->insert(35.65796, 139.708928, o);
    }

    cyberbird::array people = storage->table("person")->select(35.65796, 139.708928, 1);
    EXPECT_EQ(people.size(), 1);
    cyberbird::object person = people[0].get<cyberbird::object>();
    EXPECT_EQ(person["name"].get<std::string>(), "bob");
    EXPECT_EQ(person["age"].get<double>(), 20);
    cyberBird.sleep();
    unlink(path);
}
