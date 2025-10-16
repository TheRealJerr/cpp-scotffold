// buffer.h
#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>
#include <system_error>
#include <iconv.h>
#include <memory>

class Buffer {
public:
    using size_type = std::vector<char>::size_type;
    static constexpr size_type DEFAULT_INITIAL_SIZE = 1024;
    static constexpr size_type MAX_BUFFER_SIZE = 1024 * 1024 * 1024; // 1GB
    
    explicit Buffer(size_type initial_size = DEFAULT_INITIAL_SIZE);
    ~Buffer();
    
    // 禁止拷贝
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    
    // 允许移动
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;
    
    // 数据写入
    void append(const char* data, size_type len);
    void append(const std::string& str);
    void append(const Buffer& other);
    
    // 数据读取
    size_type read(char* dest, size_type len);
    std::string readString(size_type len);
    std::string readAllString();
    
    // 查看数据（不移动读指针）
    const char* peek() const { return data_.data() + read_index_; }
    size_type peekSize() const { return write_index_ - read_index_; }
    
    // 缓冲区管理
    void clear();
    void shrinkToFit();
    void ensureCapacity(size_type needed_capacity);
    
    // 状态查询
    bool empty() const { return read_index_ >= write_index_; }
    size_type size() const { return write_index_ - read_index_; }
    size_type capacity() const { return data_.capacity(); }
    
    // 搜索操作
    size_type find(const std::string& pattern) const;
    size_type find(char c) const;
    
    // 按行读取
    bool readLine(std::string& line);
    
    // 编码转换支持
    bool convertEncoding(const std::string& from_encoding, 
                        const std::string& to_encoding);

private:
    std::vector<char> data_;
    size_type read_index_ = 0;
    size_type write_index_ = 0;
    
    void makeSpace(size_type needed);
    bool iconvConvert(const std::string& from_encoding, 
                     const std::string& to_encoding,
                     const char* src, size_t src_len,
                     std::vector<char>& dest);
};

using buffer_ptr = std::shared_ptr<Buffer>;

class BufferFactory
{
public:
    static buffer_ptr create(size_t initial_size = Buffer::DEFAULT_INITIAL_SIZE)
    {
        return std::make_shared<Buffer>(initial_size);
    }
};
#endif // BUFFER_H