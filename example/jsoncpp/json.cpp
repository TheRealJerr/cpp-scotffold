#include "json.h"


namespace Toolss
{
    std::string JsonTools::serialize(const Json::Value& value)
    {
        Json::StreamWriterBuilder builder;
        auto writer = builder.newStreamWriter();
        std::stringstream ss;
        writer->write(value, &ss);
        return ss.str();
    }

    Json::Value JsonTools::deserialize(const std::string& json)
    {
        Json::Reader reader;
        Json::Value value;
        if (!reader.parse(json, value))
        {
            
            throw std::runtime_error("Failed to parse JSON");
        }
        return value;
    }
    
}