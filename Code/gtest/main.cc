#include <gtest/gtest.h>
#include <iostream>
#include <unordered_map>
// /*
//     简单使用gtest框架
// */
// TEST(unordered_map_test,insert)
// {
//     std::unordered_map<int, int> map;
//     map.insert(std::make_pair(1, 2));
//     map.insert(std::make_pair(3, 4));
//     map.insert(std::make_pair(5, 6));
//     EXPECT_EQ(map[1], 2);
//     EXPECT_EQ(map.find(1), map.end());

//     std::cout << "unordered_map insert test passed" << std::endl;
// }




// auto main(int argc, char** argv)
// -> int
// {
//     /*
//         1. 定义测试用例
//         2. 初始化测试环境
//         3. 运行测试用例
//     */
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }



// 测试类
class TestObject
{
public:
    TestObject(int data) : data_(data) {}


private:
    int data_;
};
// 构建全局的测试环境
class GlobalEnvironment : public testing::Environment {
public:
    virtual void SetUp() override
    {
        // 进行全局的初始化工作
        TestObject* obj_ptr = 
    }

    virtual void TearDown() override
    {
        // 进行全局的清理工作
    }
private:
};