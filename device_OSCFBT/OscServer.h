#pragma once

#include <string>
#include <Eigen/Dense>

#include "miniosc.h"

class OscServer
{
public:
    OscServer(const std::wstring& ipAddress, uint32_t port);

    OscServer() = default;
    ~OscServer();

public:
    int Tick();
    void BeginPacket();
    int FlushData();
    bool IsAlive();

    int SendPacket_Vector3(const std::string& address, float elem1, float elem2, float elem3);
    int SendPacket_Vector3(const std::string& address, Eigen::Vector3d vector);
    int SendPacket_Quat(const std::string& address, const Eigen::Quaterniond& orientation);

private:
    static void Server_Callback(const char* address, const char* type, void** parameters);

private:
    mobundle m_packetBundle;
    miniosc* m_oscServer;
};
