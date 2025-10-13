#include "com.hpp"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <cstring>

struct script_server::impl {
    SOCKET server_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;
    sockaddr_in server_addr{};
    WSADATA wsa_data{};
};

script_server::script_server()
    : pimpl(new impl())
{
}

script_server::~script_server() {
    close();
    delete pimpl;
}

bool script_server::initialize(int port) {
    if (WSAStartup(MAKEWORD(2, 2), &pimpl->wsa_data) != 0)
        return false;

    pimpl->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pimpl->server_socket == INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }

    pimpl->server_addr.sin_family = AF_INET;
    pimpl->server_addr.sin_addr.s_addr = INADDR_ANY;
    pimpl->server_addr.sin_port = htons(port);

    if (bind(pimpl->server_socket, reinterpret_cast<sockaddr*>(&pimpl->server_addr), sizeof(pimpl->server_addr)) == SOCKET_ERROR)
    {
        close();
        WSACleanup();
        return false;
    }

    if (listen(pimpl->server_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        close();
        WSACleanup();
        return false;
    }

    return true;
}

std::string script_server::receive_script() {
    sockaddr_in client_addr{};
    int client_size = sizeof(client_addr);
    std::vector<char> buffer(1024);

    pimpl->client_socket = accept(pimpl->server_socket, reinterpret_cast<sockaddr*>(&client_addr), &client_size);
    if (pimpl->client_socket == INVALID_SOCKET)
        return "";

    std::string script;
    int bytes_received = 0;
    while ((bytes_received = recv(pimpl->client_socket, buffer.data(), static_cast<int>(buffer.size()), 0)) > 0)
    {
        script.append(buffer.data(), bytes_received);
    }

    closesocket(pimpl->client_socket);
    pimpl->client_socket = INVALID_SOCKET;

    return script;
}

void script_server::close() {
    if (pimpl->server_socket != INVALID_SOCKET)
    {
        closesocket(pimpl->server_socket);
        pimpl->server_socket = INVALID_SOCKET;
    }
    WSACleanup();
}
