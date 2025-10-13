#pragma once
#include <string>

class script_server
{
public:
    script_server();
    ~script_server();

    bool initialize(int port);
    void close();
    std::string receive_script();

private:
    struct impl;
    impl* pimpl;
};