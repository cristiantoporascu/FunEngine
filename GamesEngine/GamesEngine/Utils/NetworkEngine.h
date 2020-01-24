#pragma once
#include "EventHandler.h"
#include "EventQueue.h"
#include <enet/enet.h>
#include "../Mathematics/Transform.h"

// Forward declarations
class WindowDisplay;
class GameObject;

struct ServerInstance {
	ENetAddress address;
	ENetHost* host;

	ENetEvent enetEvent;
	ENetPacket* dataPacket;

	int clientCount = 0;
};

struct ClientInstance {
	ENetAddress address;
	ENetHost* host;
	ENetPeer* peer;

	ENetEvent enetEvent;
	ENetPacket* dataPacket;

	int clientNumber = 0;
};

struct RemotePlayerData {
	int packetIndex = 0;
	int dataActive = 0;
	int networkSource = 0;
	int clientNumber = 0;
	Transform transform;
};

struct RemoteSceneData {
	int packetIndex = 1;
	int sceneSelected = -1;
	int networkSource = 0;
	int clientRef = 0;
};

enum NetworkType {
	NONE,
	SERVER,
	CLIENTS
};

class NetworkEngine {
private:
	WindowDisplay* m_graphicEngineEntity;

	NetworkType m_networkType;

	ServerInstance* m_server;
	ClientInstance* m_client;

	RemotePlayerData* m_playerData;
	RemoteSceneData* m_sceneData;

	std::vector<GameObject*> m_remoteCharacter = std::vector<GameObject*>(2);

	bool m_networkAdapterActive = false;

public:
	/**
	 * Singletone for the assetmanager to be accesable from everywhere
	 */
	static NetworkEngine* s_networkEngine;

	static NetworkType ConvertIntToType(const int& id) {
		switch (id) {
		case 0:
			return NetworkType::NONE;
		case 1:
			return NetworkType::SERVER;
		case 2:
			return NetworkType::CLIENTS;
		}
	}

	NetworkEngine() {
		this->m_playerData = new RemotePlayerData();
		this->m_sceneData = new RemoteSceneData();
	}
	~NetworkEngine();

	/**
	 * Initalise the network adapter after choosing which type it will sustain
	 * @param networkType						The specific type of the network that
	 *											this adapter will sustain
	 */
	void InitialiseNetworkAdapter(const NetworkType& networkType);

	/**
	 * Initalise the server instance of the network
	 */
	void InitialiseServer();

	/**
	 * Initalise the client instance of the network
	 */
	void InitialiseClient();

	/**
	 * Update the data packets after each individual frame
	 */
	void UpdatePacketData();

	/**
	 * Handle events that come from the network adapter
	 * @param deltaTime							The time between each individual frame
	 */
	void UpdateEventPool(const float& deltaTime);

	/**
	 * Update the graphics of the host or client based on the remote host or client
	 */
	void DrawRemoteData();

	/**
	 * Event handlers for all the event sets present in the network engine
	 */
	void OnEvent(Event& e);

	void OnLevelFinish();

	void OnServerConnect();
	void OnServerDisconnect();
	void OnServerReceive();

	void OnClientReceive();

	/**
	 * Getters and setters
	 */
public:
	inline const NetworkType& GetNetworkType() { return this->m_networkType; }
	inline ServerInstance* GetServerInstance() { return this->m_server; }
	inline ClientInstance* GetClientInstance() { return this->m_client; }

	inline void SetNetworkType(const NetworkType& newType) { this->m_networkType = newType; }
	inline void SetWindowEntity(WindowDisplay* newWindow) { this->m_graphicEngineEntity = newWindow; }
};