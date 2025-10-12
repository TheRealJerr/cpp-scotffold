#include "json.h"
#include "log.h"
namespace Tools
{
    std::optional<std::string> JsonTools::serialize(const Json::Value& value)
    {
        Json::StreamWriterBuilder builder;
        auto writer = builder.newStreamWriter();
        std::stringstream ss;
        writer->write(value, &ss);
        return ss.str();
    }

    std::optional<Json::Value> JsonTools::deserialize(const std::string& json)
    {
        Json::Reader reader;
        Json::Value value;
        if (!reader.parse(json, value))
        {
            ERR("Failed to parse JSON: {}", json);
            return std::nullopt;
        }
        return value;
    }
    
}