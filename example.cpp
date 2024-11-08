#include "ADB.cpp"

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

int main() {
    Schema<int, Person> personSchema("person_db.bin", "person_db_index.bin");
    personSchema.insertRecord(1, {10, "Bob"});
    std::cout << personSchema.retrieveRecord(1).name << '\n';
    return 0;
}