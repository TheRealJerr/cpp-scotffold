#pragma once
// 实现对于jsoncpp的封装
#include <jsoncpp/json/json.h>
#include <optional>
// 统一包含在命名空间
namespace Tools
{
    // json工具类
    class JsonTools
    {
    public:
        // 序列化Json::Value对象为字符串
        static std::optional<std::string> serialize(const Json::Value& value);

        // 反序列化字符串为Json::Value对象
        static std::optional<Json::Value> deserialize(const std::string& str);

    };
}