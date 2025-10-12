#include "rigistry.h"

namespace RpcTools
{
    // channels的接口定义

    void Channels::add_node(const std::string& addr)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        if(channel_map_.count(addr)) // 节点已经存在
            return;
        
        std::shared_ptr<brpc::Channel> channel = std::make_shared<brpc::Channel>();

        brpc::ChannelOptions options;
        options.protocol = "baidu_std";

        if (channel->Init(addr.c_str(), &options) != 0) {
            std::cerr << "Fail to initialize channel to " << addr << std::endl;
            return;
        }

        channel_map_[addr] = channel;
        channel_pool_.push_back(channel);
    }


    Channels::ChannelPtr_t Channels::get_node()
    {
        std::unique_lock<std::mutex> lock(mtx_);

        if(channel_pool_.empty()) // 没有可用节点
            return nullptr;
        
        // RR轮询算法
        auto ret = channel_pool_[RR_index_];

        RR_index_ = (RR_index_ + 1) % channel_pool_.size();

        return ret;
    }

    void Channels::remove_node(const std::string& addr)
    {
        std::unique_lock<std::mutex> lock(mtx_);

        if(channel_map_.count(addr) == 0) // 节点不存在
            return;
        
       
        // 删除列表中的对应的Channel
        auto& erase_channel = channel_map_[addr];
        for(auto it = channel_pool_.begin(); it != channel_pool_.end(); ++it)
        {
            if(it->get() == erase_channel.get()) 
            {
                channel_pool_.erase(it);
                return;
            }
        }

        channel_map_.erase(addr);
        INF("{}节点{}下线", addr, service_name_);
    }

    void Channels::remove_all_nodes()
    {
        std::unique_lock<std::mutex> lock(mtx_);

        channel_map_.clear();
        channel_pool_.clear();

    }



    // channels池的接口定义

    void ChannelsPool::set_watch_services(const std::initializer_list<std::string>& services)   
    {
        std::unique_lock<std::mutex> lock(mtx_);

        for(auto& service_name : services)
        {
            // 由于存在mutex, 因此这里不能使用emplace, 否则会导致死锁
            if(channels_map_.count(service_name) == 0) channels_map_.emplace(service_name, Channels(service_name));
            else WAR("{}重复关注", service_name);
        }

    }

    void ChannelsPool::add_node(const std::string& service_name, const std::string& addr)
    {
        std::unique_lock<std::mutex> lock(mtx_);

        // 不是我们关心的服务, not_set_watch_services
        if(channels_map_.count(service_name) == 0) // 服务不存在
        {
            WAR("{}不在关注列表, 忽略", service_name);
            return;
        }

        channels_map_[service_name].add_node(addr);
    }

    Channels::ChannelPtr_t ChannelsPool::get_node(const std::string& service_name)
    {
        std::unique_lock<std::mutex> lock(mtx_);

        if(channels_map_.count(service_name) == 0) // 服务不存在
        {
            ERR("{}不存在", service_name);
            return nullptr;
        }

        return channels_map_[service_name].get_node();
    }

    void ChannelsPool::remove_node(const std::string& service_name, const std::string& addr)
    {
        std::unique_lock<std::mutex> lock(mtx_);
        if(channels_map_.count(service_name) == 0) // 服务不存在
            return;

        channels_map_[service_name].remove_node(addr);
    }


    RpcServerFactory::RpcServer_t RpcServerFactory::create_server(google::protobuf::Service* service,int port)
    {
        brpc::ServerOptions options;

        options.idle_timeout_sec = -1;
        
        RpcServer_t server = std::make_shared<brpc::Server>();

        int ret = server->AddService(service, brpc::SERVER_OWNS_SERVICE);
        if(ret == -1)
        {
            std::cout << "Add Service Failed" << std::endl;
            abort();
        }

        return server;
    }
    
    // // 注册服务

    // void RpcRigistry::register_service(const std::string& service_name, 
    //     const std::string& addr)
    // {
    //     channels_pool_->add_node(service_name, addr);
    // }


    // // 注销服务
    // void RpcRigistry::unregister_service(const std::string& service_name, 
    //     const std::string& addr)
    // {
    //     channels_pool_->remove_node(service_name, addr);
    // }

}