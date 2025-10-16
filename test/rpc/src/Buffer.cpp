#include "../include/Buffer.h"
#include <cstring>
#include <stdexcept>
#include <iostream>

Buffer::Buffer(size_type initial_size) {
    data_.reserve(std::max(initial_size, DEFAULT_INITIAL_SIZE));
}

Buffer::Buffer(Buffer&& other) noexcept
    : data_(std::move(other.data_)),
      read_index_(other.read_index_),
      write_index_(other.write_index_) {
    other.read_index_ = 0;
    other.write_index_ = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        data_ = std::move(other.data_);
        read_index_ = other.read_index_;
        write_index_ = other.write_index_;
        other.read_index_ = 0;
        other.write_index_ = 0;
    }
    return *this;
}

Buffer::~Buffer() = default;

void Buffer::append(const char* data, size_type len) {
    if (len == 0) return;
    
    ensureCapacity(len);
    std::copy(data, data + len, data_.begin() + write_index_);
    write_index_ += len;
}

void Buffer::append(const std::string& str) {
    append(str.data(), str.length());
}

void Buffer::append(const Buffer& other) {
    append(other.peek(), other.size());
}

Buffer::size_type Buffer::read(char* dest, size_type len) {
    size_type readable = size();
    if (readable == 0) return 0;
    
    len = std::min(len, readable);
    std::copy(peek(), peek() + len, dest);
    read_index_ += len;
    
    // 如果所有数据都读完了，重置指针
    if (read_index_ == write_index_) {
        clear();
    }
    
    return len;
}

std::string Buffer::readString(size_type len) {
    len = std::min(len, size());
    std::string result(peek(), len);
    read_index_ += len;
    
    if (read_index_ == write_index_) {
        clear();
    }
    
    return result;
}

std::string Buffer::readAllString() {
    std::string result(peek(), size());
    clear();
    return result;
}

void Buffer::clear() {
    read_index_ = 0;
    write_index_ = 0;
    data_.clear();
}

void Buffer::shrinkToFit() {
    if (read_index_ > 0) {
        // 移动有效数据到头部
        if (read_index_ < write_index_) {
            std::copy(data_.begin() + read_index_, 
                     data_.begin() + write_index_,
                     data_.begin());
            write_index_ -= read_index_;
        } else {
            write_index_ = 0;
        }
        read_index_ = 0;
    }
    
    data_.shrink_to_fit();
}

void Buffer::ensureCapacity(size_type needed) {
    if (write_index_ + needed > data_.capacity()) {
        makeSpace(needed);
    }
}

void Buffer::makeSpace(size_type needed) {
    size_type total_needed = write_index_ + needed;
    
    if (total_needed > MAX_BUFFER_SIZE) {
        throw std::runtime_error("Buffer size exceeded maximum limit");
    }
    
    // 如果前面有空间，先移动数据
    if (read_index_ > 0) {
        size_type readable = size();
        if (readable > 0) {
            std::copy(data_.begin() + read_index_, 
                     data_.begin() + write_index_,
                     data_.begin());
        }
        write_index_ = readable;
        read_index_ = 0;
    }
    
    // 如果还不够，扩容
    if (write_index_ + needed > data_.capacity()) {
        size_type new_capacity = std::max(data_.capacity() * 2, total_needed);
        new_capacity = std::min(new_capacity, MAX_BUFFER_SIZE);
        data_.reserve(new_capacity);
    }
}

Buffer::size_type Buffer::find(const std::string& pattern) const {
    if (pattern.empty() || size() < pattern.length()) {
        return std::string::npos;
    }
    
    const char* start = peek();
    const char* end = start + size();
    const char* result = std::search(start, end, pattern.begin(), pattern.end());
    
    return result == end ? std::string::npos : result - start;
}

Buffer::size_type Buffer::find(char c) const {
    const char* start = peek();
    const char* result = std::find(start, start + size(), c);
    return result == start + size() ? std::string::npos : result - start;
}

bool Buffer::readLine(std::string& line) {
    size_type pos = find('\n');
    if (pos == std::string::npos) {
        return false;
    }
    
    // 包含换行符
    line = readString(pos + 1);
    
    // 移除可能的回车符
    if (!line.empty() && line.back() == '\n') {
        line.pop_back();
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
    }
    
    return true;
}

bool Buffer::convertEncoding(const std::string& from_encoding, 
                           const std::string& to_encoding) {
    if (empty()) return true;
    
    std::vector<char> converted;
    if (!iconvConvert(from_encoding, to_encoding, peek(), size(), converted)) {
        return false;
    }
    
    // 替换缓冲区内容
    clear();
    append(converted.data(), converted.size());
    return true;
}

bool Buffer::iconvConvert(const std::string& from_encoding, 
                         const std::string& to_encoding,
                         const char* src, size_t src_len,
                         std::vector<char>& dest) {
    iconv_t cd = iconv_open(to_encoding.c_str(), from_encoding.c_str());
    if (cd == (iconv_t)-1) {
        return false;
    }
    
    dest.resize(src_len * 2);  // 初始分配2倍空间
    
    char* in_ptr = const_cast<char*>(src);
    size_t in_left = src_len;
    char* out_ptr = dest.data();
    size_t out_left = dest.size();
    
    bool success = true;
    
    while (in_left > 0) {
        size_t result = iconv(cd, &in_ptr, &in_left, &out_ptr, &out_left);
        
        if (result == (size_t)-1) {
            if (errno == E2BIG) {
                // 输出空间不足，扩容
                size_t used = dest.size() - out_left;
                dest.resize(dest.size() * 2);
                out_ptr = dest.data() + used;
                out_left = dest.size() - used;
            } else {
                success = false;
                break;
            }
        }
    }
    
    // 调整大小到实际使用的大小
    dest.resize(dest.size() - out_left);
    
    iconv_close(cd);
    return success;
}