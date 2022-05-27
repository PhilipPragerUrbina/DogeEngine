//
// Created by Philip on 5/15/2022.
//

#ifndef DOGEENGINE_SCENEFILEMANAGER_HPP
#define DOGEENGINE_SCENEFILEMANAGER_HPP

#include "json.hpp"
class SceneFileManager {
public:
    SceneFileManager(){
        // create an empty structure (null)
        nlohmann::json j;

// add a number that is stored as double (note the implicit conversion of j to an object)
        j["pi"] = 3.141;

// add a Boolean that is stored as bool
        j["happy"] = true;

// add a string that is stored as std::string
        j["name"] = "Niels";

// add another null object by passing nullptr
        j["nothing"] = nullptr;

// add an object inside the object
        j["answer"]["everything"] = 42;

// add an array that is stored as std::vector (using an initializer list)
        j["list"] = { 1, 0, 2 };

// add another object (using an initializer list of pairs)
        j["object"] = { {"currency", "USD"}, {"value", 42.99} };
/*
 // read a JSON file
std::ifstream i("file.json");
json j;
i >> j;

// write prettified JSON to another file
std::ofstream o("pretty.json");
o << std::setw(4) << j << std::endl;
 */
    }
};

#endif DOGEENGINE_SCENEFILEMANAGER_HPP