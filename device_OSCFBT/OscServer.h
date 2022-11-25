#pragma once

#include <string>

#include "miniosc.h"

class OscServer
{
public:
	OscServer() = default;
	OscServer(const std::wstring& ipAddress, uint32_t port);
	~OscServer();

public:
	int Tick();
	void BeginPacket();
	void SendPacket_Vector3(const std::string& address, float elem1, float elem2, float elem3);
	void FlushData();

private:
	static void Server_Callback(const char* address, const char* type, void** parameters);

private:
	mobundle m_packetBundle;
	miniosc* m_oscServer;
};

