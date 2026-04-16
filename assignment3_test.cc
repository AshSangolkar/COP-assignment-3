#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "leveldb/db.h"

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, "testdb_assignment3", &db);
    assert(status.ok());

    leveldb::WriteOptions write_options;
    leveldb::ReadOptions read_options;

    std::cout << "--- Testing Scan ---" << std::endl;
    db->Put(write_options, "a", "1");
    db->Put(write_options, "b", "2");
    db->Put(write_options, "c", "3");
    db->Put(write_options, "d", "4");

    std::vector<std::pair<std::string, std::string>> scan_results;
    status = db->Scan(read_options, "b", "d", &scan_results);
    assert(status.ok());
    
    std::cout << "Scan results [b, d):" << std::endl;
    for (const auto& kv : scan_results) {
        std::cout << kv.first << " => " << kv.second << std::endl;
    }
    assert(scan_results.size() == 2);
    assert(scan_results[0].first == "b");
    assert(scan_results[1].first == "c");

    std::cout << "\n--- Testing DeleteRange ---" << std::endl;
    status = db->DeleteRange(write_options, "b", "d");
    assert(status.ok());

    scan_results.clear();
    status = db->Scan(read_options, "a", "z", &scan_results);
    assert(status.ok());
    std::cout << "Scan results after DeleteRange [b, d):" << std::endl;
    for (const auto& kv : scan_results) {
        std::cout << kv.first << " => " << kv.second << std::endl;
    }
    assert(scan_results.size() == 2);
    assert(scan_results[0].first == "a");
    assert(scan_results[1].first == "d");

    std::cout << "\n--- Testing ForceFullCompaction ---" << std::endl;
    // Add more data to ensure there's something to compact
    for (int i = 0; i < 1000; i++) {
        db->Put(write_options, "key" + std::to_string(i), "value" + std::to_string(i));
    }
    status = db->ForceFullCompaction();
    assert(status.ok());

    std::cout << "\nAll Assignment 3 features verified successfully!" << std::endl;

    delete db;
    // Cleanup
    leveldb::DestroyDB("testdb_assignment3", options);
    return 0;
}
