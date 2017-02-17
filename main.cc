#include <cyberbird/cyberbird.h>


int main(int argc, char **argv)
{
    cyberbird::CyberBird cyberBird;
    cyberBird.wake("map.db");

    cyberbird::Storage *storage = cyberBird.storage();

    cyberbird::object o;
    o.insert(std::make_pair("name", cyberbird::value("event quest")));
    o.insert(std::make_pair("desc", cyberbird::value("difficulty level: max")));
    storage->table("events")->insert(35.65796, 139.708928, 14, cyberbird::value(o));

    cyberbird::array events       = storage->table("events")->select(35.65796, 139.708928);
    cyberbird::array::iterator it = events.begin();
    for (; it != events.end(); it++) {
        cyberbird::value  v = *it;
        cyberbird::object o = v.get<cyberbird::object>();
        cyberbird::value value = o["name"];
        fprintf(stderr, "get value = { name: [%s], desc: [%s] }", o["name"].get<std::string>().c_str(), o["desc"].get<std::string>().c_str());
        it++;
    }
    cyberBird.sleep();
    return 0;
}
