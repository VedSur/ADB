Amazing Database
=======

# User Guide

Let's take the of a Person struct with int age and string name.
## Include ABD.cpp
```cpp
#include "ADB.cpp"
```
## Make the datatype
Make sure to include the serialize and deserialize functions.
```cpp
struct Person {
    int age;
    std::string name;

    // Serialization for Person
    friend void serialize(std::ostream& os, const Person& person) {
        serialize(os, person.age);
        serialize(os, person.name);
    }

    // Deserialization for Person
    friend void deserialize(std::istream& is, Person& person) {
        deserialize(is, person.age);
        deserialize(is, person.name);
    }
};
```

## Initialize a schema
- The a Schema needs to be provided with key type and the data type that we just created.
- The constructor needs to be provived with a file(name) for storing the data and the indexing.
```cpp
Schema<int, Person> personSchema("person_db.bin", "person_db_index.bin");
```

## Use the database
Examples
```cpp
personSchema.insertRecord(1, {10, "Bob"});
std::cout << personSchema.retrieveRecord(1).name << std::endl;
personSchema.deleteRecord(1);
```
## Extra note
Don't worry about saving anything everythings automatically stored in the specified files.