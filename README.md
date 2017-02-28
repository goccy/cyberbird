# cyberbird
fast/compact geo location database for mobile
# status
DEVELOPMENT

# Getting Started
## BUILD
```bash
$ git submodule init
$ git submodule update
$ mkdir build
$ cd bulid
$ cmake ../
```
## Testing
cyberbird use GoogleTest framework.
```
$ cd build
$ cmake ../
$ ./test/Test
```
## Example Code for Native (C++)
```c++
#include <cyberbird/cyberbird.h>

cyberbird::CyberBird cyberBird;
cyberBird.wake("cyberbird.db");

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

cyberbird::array people  = cyberBird.fly("person", 35.65796, 139.708928, 1);
cyberbird::object person = people[0].get<cyberbird::object>();
person["name"].get<std::string>(); // "bob"
person["age"].get<double>(); //20
cyberBird.sleep();
```

# Work on iOS

## INSTALL
cyberbird supports cocoapods.
write the follwing code your Podfile
```ruby
pod "CyberBird", :git => "git@github.com:goccy/cyberbird.git"
```
execute `pod install`
```
$ pod install
```
## Example

```
$ cd ios/Example
$ pod install
$ open CyberBirdExample.xcworkspace
```



