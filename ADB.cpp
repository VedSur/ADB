#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

template <typename KeyType, typename DataType>
struct Record {
    KeyType id;
    DataType data;
};

template <typename KeyType, typename DataType>
class Schema {
public:
    Schema(const std::string& dataFilename, const std::string& indexFilename)
        : dataFilename(dataFilename), indexFilename(indexFilename) {
        loadIndexFromFile();  // Load index on initialization
    }

    ~Schema() {
        saveIndexToFile();  // Save index on destruction
    }

    bool insertRecord(const KeyType& id, const DataType& data);
    bool deleteRecord(const KeyType& id);
    DataType retrieveRecord(const KeyType& id);

private:
    std::unordered_map<KeyType, std::streampos> index;  // In-memory index
    std::string dataFilename;
    std::string indexFilename;

    void loadIndexFromFile();
    void saveIndexToFile();
};

// Serialization helpers
template <typename T>
void serialize(std::ostream& os, const T& data) {
    os.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

void serialize(std::ostream& os, const std::string& data) {
    size_t size = data.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    os.write(data.data(), size);
}

template <typename T>
void deserialize(std::istream& is, T& data) {
    is.read(reinterpret_cast<char*>(&data), sizeof(T));
}

void deserialize(std::istream& is, std::string& data) {
    size_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    data.resize(size);
    is.read(&data[0], size);
}

// Insert a record with indexing
template <typename KeyType, typename DataType>
bool Schema<KeyType, DataType>::insertRecord(const KeyType& id, const DataType& data) {
    if (index.find(id) != index.end()) {
        std::cout << "Record with ID " << id << " already exists.\n";
        return false;
    }

    // Open the data file in append mode to add the new record
    std::ofstream dataFile(dataFilename, std::ios::app | std::ios::binary);
    if (!dataFile) {
        std::cerr << "Unable to open data file.\n";
        return false;
    }

    // Record the position of the new data entry in the file
    std::streampos pos = dataFile.tellp();

    // Serialize the record (ID and Data)
    serialize(dataFile, id);
    serialize(dataFile, data);

    // Update the index with the new record's file position
    index[id] = pos;

    return true;
}

// Retrieve a record using the index
template <typename KeyType, typename DataType>
DataType Schema<KeyType, DataType>::retrieveRecord(const KeyType& id) {
    auto it = index.find(id);
    if (it == index.end()) {
        throw std::runtime_error("Record not found.");
    }

    // Open the data file to read the record at the indexed position
    std::ifstream dataFile(dataFilename, std::ios::binary);
    if (!dataFile) {
        throw std::runtime_error("Unable to open data file.");
    }

    // Move to the recorded file position
    dataFile.seekg(it->second);

    KeyType key;
    DataType data;

    deserialize(dataFile, key);
    deserialize(dataFile, data);

    return data;
}

// Delete a record (removes from index only)
template <typename KeyType, typename DataType>
bool Schema<KeyType, DataType>::deleteRecord(const KeyType& id) {
    if (index.erase(id) == 0) {
        std::cout << "No record found with ID " << id << ".\n";
        return false;
    }
    return true;
}

// Load the index from the index file
template <typename KeyType, typename DataType>
void Schema<KeyType, DataType>::loadIndexFromFile() {
    std::ifstream indexFile(indexFilename, std::ios::in | std::ios::binary);
    if (!indexFile) {
        std::cerr << "No index file found. Starting fresh.\n";
        return;
    }

    KeyType id;
    std::streampos pos;

    while (indexFile.peek() != EOF) {
        deserialize(indexFile, id);
        deserialize(indexFile, pos);
        index[id] = pos;
    }

    indexFile.close();
}

// Save the index to the index file
template <typename KeyType, typename DataType>
void Schema<KeyType, DataType>::saveIndexToFile() {
    std::ofstream indexFile(indexFilename, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!indexFile) {
        std::cerr << "Unable to open index file for saving.\n";
        return;
    }

    for (const auto& entry : index) {
        serialize(indexFile, entry.first);
        serialize(indexFile, entry.second);
    }

    indexFile.close();
}