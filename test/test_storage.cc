#include <cyberbird/storage.h>
#include <cyberbird/util.h>
#include <gtest/gtest.h>

using namespace cyberbird;

class StorageTest : public ::testing::Test{};

TEST_F(StorageTest, createTable) {
    Table::Builder builder("person");
    Table *table = builder.addStringColumn("name", 16)->addNumberColumn("age")->build();
    Storage storage("storage.db");
    storage.createTable(table);
}
