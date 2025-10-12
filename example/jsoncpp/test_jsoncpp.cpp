#include <iostream>
#include "json.h"

int main(int argc, char* argv[])
{
    Json::Value root;
    root["name"] = "John";
    root["age"] = 30;
    root["isMarried"] = true;
    root["Array"].append(1);
    root["Array"].append(2);
    root["Array"].append(3);
    root["Object"]["city"] = "New York";
    auto str = Tools::JsonTools::serialize(root);

    try 
    {
        auto value = Tools::JsonTools::deserialize(str);
        std::cout << value["name"].asString() << std::endl;
        std::cout << value["age"].asInt() << std::endl;
        std::cout << value["isMarried"].asBool() << std::endl;
        std::cout << value["Array"][0].asInt() << std::endl;
        std::cout << value["Array"][1].asInt() << std::endl;
        std::cout << value["Array"][2].asInt() << std::endl;
        std::cout << value["Object"]["city"].asString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }catch(...)
    {
        std::cerr << "Unknown error" << std::endl;
    }

    return 0;
}