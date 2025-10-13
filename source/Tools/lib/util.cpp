#include "../include/util.h"
#include "../include/log.h"

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
    bool FileTools::read_from_file(const std::string& file_path, std::string* content)
    {
        std::ifstream ifs(file_path, std::ios::in | std::ios::binary);
        if (!ifs.is_open())
        {
            ERR("Failed to open {}: {}", file_path, strerror(errno));
            return false;
        }

        size_t size = ifs.seekg(0, std::ios::end).tellg();
        ifs.seekg(0, std::ios::beg);

        content->resize(size);

        ifs.read(&(*content)[0], size);
        if(ifs.good() == false){
            ERR("Failed to read {}: {}", file_path, strerror(errno));
            return false;
        }

        return true;
    }

    bool FileTools::write_to_file(const std::string& file_path, const std::string& content)
    {
        std::ofstream ofs(file_path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!ofs.is_open())
        {
            ERR("Failed to open {}: {}", file_path, strerror(errno));
            return false;
        }

        ofs.write(content.data(), content.size());
        if(ofs.good() == false){
            ERR("Failed to write {}: {}", file_path, strerror(errno));
            return false;
        }

        return true;
    }
}