#pragma once

#include <mutex>
#include <string>
#include <Eigen/Dense>

#include "miniosc.h"

inline std::unique_lock<std::mutex> server_alive_mutex;

class OscServer
{
public:
    explicit OscServer(const std::wstring& ipAddress, uint32_t port);
    ~OscServer() = default;

public:
    int Tick();
    void BeginPacket();
    int FlushData();
    bool IsAlive();

    int SendPacket_Vector3(const std::string& address, float elem1, float elem2, float elem3);
    int SendPacket_Vector3(const std::string& address, Eigen::Vector3d vector);
    int SendPacket_Quat(const std::string& address, const Eigen::Quaterniond& orientation);

    void Cleanup();

private:
    static void Server_Callback(const char* address, const char* type, void** parameters);

private:
    mobundle m_packetBundle;
    miniosc* m_oscServer;
};
