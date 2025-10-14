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

    std::vector<std::string> StringTools::split(const std::string& str, 
            const std::string& sep)
    {
        size_t pos = 0, idx = 0;
        std::vector<std::string> res;
        while ((pos = str.find(sep, pos)) != std::string::npos)
        {
            // 1,2,3,4,,保证空字符串不会添加进入res中
            if(pos != idx) res.push_back(str.substr(idx, pos - idx));
            idx = pos + sep.size();
            pos = idx;
        }
        res.push_back(str.substr(idx));
        return res;
    }
    bool StringTools::regex_match(const std::string& str, const std::string& pattern)
    {
        return std::regex_match(str, std::regex(pattern));
    }
    
}