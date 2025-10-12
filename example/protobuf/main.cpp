#include <iostream>
#include "Person.pb.h"
#include <unistd.h>
// 将protobuf转换为Json格式
#include <google/protobuf/util/json_util.h>
using namespace example;

template <typename T>
std::string protobuf_serialize(T&& msg)
{
    return msg.SerializeAsString();
}

template <class T>
void protobuf_deserialize(const std::string& str, T& msg)
{
    bool ret = msg.ParseFromString(str);
    if(ret == false) throw std::runtime_error("parseFromString failed");
}

Person create_protobuf_person()
{
    Person p;
    p.set_name("Alice");
    p.set_age(25);
    p.set_gender(Gender::female);
    p.add_scores(85.5);
    p.add_scores(92.5); 
    auto* tags = p.mutable_tags();
    tags->insert(google::protobuf::MapPair<std::string, std::string>("tag1", "value1"));
    p.scores();
    p.mutable_scores()->Add(95.5);
    return p;
}

void print_protobuf_person(const Person& p)
{
    std::cout << "name: " << p.name() << std::endl;
    std::cout << "age: " << p.age() << std::endl;
    std::cout << "gender: " << Gender_Name(p.gender()) << std::endl;
    for(int i = 0; i < p.scores_size(); i++)
    {
        std::cout << "scores[" << i << "]: " << p.scores(i) << std::endl;
    }
    for(auto it = p.tags().begin(); it != p.tags().end(); it++)
    {
        std::cout << "tags[" << it->first << "]: " << it->second << std::endl;
    }
}
Person json_to_protobuf_person()
{
    std::string json_str = R"(
        {
            "name": "Bob",
            "age": 30,
            "gender": "male",
            "scores": [80.5, 90.5],
            "tags": {
                "tag1": "value1",
                "tag2": "value2"
            }
        }
    )";

    Person p;
    // 将json字符串反序列化为Person对象
    auto status = google::protobuf::util::JsonStringToMessage(json_str, &p);
    if(status.ok() == false)
    {
        std::cout << status.error_message() << std::endl;
    }else std::cout << "json to protobuf success" << std::endl;

    std::string json_str2;
    google::protobuf::util::MessageToJsonString(p, &json_str2);
    std::cout << json_str2 << std::endl;
    return p;
}

int main(int argc,char* argv[])
{
    Person p = create_protobuf_person();
    // 序列化Person对象
    std::string str = protobuf_serialize(p);
    // 将序列化后的字符串反序列化为Person对象
    Person p2;
    protobuf_deserialize<Person>(str, p2);
    // 打印Person对象
    // print_protobuf_person(p2);
    std::cout << p2.GetTypeName();
    Person p3 = json_to_protobuf_person();
    print_protobuf_person(p3);
    return 0;
};