#include "json.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using json = nlohmann::json;

struct Object {
    bool my_bool;
    float my_float;
};

struct Example {
    int my_number;
    std::string my_string;
    std::vector<int> my_array;
    Object my_object;
};

void parse_example(std::string file_path, Example& example) {
    /**
     * TODO: Parse the JSON file at file_path and populate the example struct.
     * Use the nlohmann::json library to parse the JSON file. Assume that the 
     * JSON file has all of the required fields and types.
     * 
     * nlohmann::json GitHub: https:
     * nlohmann::json API documentation: https:
     */
    
    
    std::ifstream file(file_path);
    json j;
    file >> j;
    
    
    example.my_number = j["my_integer"];
    example.my_string = j["my_string"];
    example.my_array = j["my_array"].get<std::vector<int>>();
    
    
    example.my_object.my_bool = j["my_object"]["my_bool"];
    example.my_object.my_float = j["my_object"]["my_float"];
}