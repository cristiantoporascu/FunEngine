#include "NetworkEngine.h"
#include "WindowDisplay.h"
#include "../SceneLoader/Scene.h"
#include <iostream>

NetworkEngine* NetworkEngine::s_networkEngine = new NetworkEngine();

NetworkEngine::~NetworkEngine() {
	delete m_playerData;
	delete m_sceneData;

	if (this->m_networkAdapterActive) {
		if (this->m_networkType == NetworkType::SERVER) {
			enet_host_destroy(this->m_server->host);
			delete this->m_server;
		}
		else {
			enet_host_destroy(this->m_client->host);
			delete this->m_client;
		}
		atexit(enet_deinitialize);
	}
}

void NetworkEngine::InitialiseNetworkAdapter(const NetworkType& networkType) {
	this->m_networkAdapterActive = true;
	this->m_networkType = networkType;

	if (enet_initialize() != 0)
	{
		std::cout << "ERROR: Enet failed to initialise!" << "\n\n";
		this->m_networkAdapterActive = false;
	}
	else {
		std::cout << "SUCCESS: Enet succesfuly has been initialise!" << "\n\n";
	}

	if (this->m_networkType == NetworkType::SERVER) {
		this->InitialiseServer();
	}
	else {
		this->InitialiseClient();
	}
}

void NetworkEngine::InitialiseServer() {
	if (this->m_networkAdapterActive) {
		this->m_server = new ServerInstance();
		
		// We set the address values to essentially be a local host
		this->m_server->address.host = ENET_HOST_ANY;
		this->m_server->address.port = 1234;

		// Now we create the server. We pass in the pointer to the address variable,
		// the maximum number of clients we intend to support, the maximum number of
		// because our host server will also be a client in this case, we only need one
		// channel for our data packets to be sent to
		// upstream and downstream bandwidth (0 is uncapped).
		this->m_server->host = enet_host_create(&this->m_server->address, 32, 2, 0, 0);

		// Now we check server initialisation, before assigning some starting values
		// to our position variable.
		if (this->m_server->host == NULL)
		{
			std::cout << "ERROR: Server failed to initialise!" << "\n\n";
		}
		else {
			std::cout << "SUCCESS: Server succesfuly has been initialise!" << "\n\n";
		}
	}
}

void NetworkEngine::InitialiseClient() {
	if (this->m_networkAdapterActive) {
		this->m_client = new ClientInstance();

		this->m_client->host = enet_host_create(NULL, 1, 2, 0, 0);

		if (this->m_client->host == NULL)
		{
			std::cout << "ERROR: Client failed to initialise!" << "\n\n";
		}
		else {
			std::cout << "SUCCESS: Client succesfuly has been initialise!" << "\n\n";
		}

		enet_address_set_host(&this->m_client->address, "localhost");
		this->m_client->address.port = 1234;

		// Now we're trying to connect to the server. We include who we are,
		// the address to establish the connection to, the number of channels, and a
		// generic data variable we can use to say something to the server (currently 0).
		this->m_client->peer = enet_host_connect(this->m_client->host, &this->m_client->address, 2, 0);

		if (this->m_client->peer == NULL) {
			std::cout << "ERROR: No available peers for initializing an ENet connection.\n\n";
		}
		else {
			std::cout << "SUCCESS: Peers found for initializing an ENet connection.\n\n";
		}
	}
}

void NetworkEngine::UpdatePacketData() {
	switch (this->m_networkType)
	{
	case NetworkType::SERVER:
		if (this->m_graphicEngineEntity->GetSceneManager() != nullptr) {
			this->m_sceneData->sceneSelected = this->m_graphicEngineEntity->GetSceneManager()->GetCurrentLevel();
		}

		if (this->m_sceneData->sceneSelected != -1) {
			this->m_playerData->dataActive = 1;
			this->m_playerData->networkSource = 1;

			this->m_server->dataPacket = enet_packet_create(this->m_playerData, sizeof(RemotePlayerData), ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(this->m_server->host, 0, this->m_server->dataPacket);
		}
		break;
	case NetworkType::CLIENTS:
		if (this->m_sceneData->sceneSelected != -1) {
			this->m_playerData->dataActive = 1;
			this->m_playerData->transform = *this->m_graphicEngineEntity
				->GetGUIEngine()
				->GetLevelDataById(this->m_sceneData->sceneSelected - 1)->GetScenePlayer()->GetTransform();

			this->m_playerData->networkSource = 2;
			this->m_playerData->clientNumber = this->m_client->clientNumber;

			this->m_client->dataPacket = enet_packet_create(this->m_playerData, sizeof(RemotePlayerData), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(this->m_client->peer, 0, this->m_client->dataPacket);
		}
		break;
	}
}

void NetworkEngine::UpdateEventPool(const float& deltaTime) {
	if (this->m_networkAdapterActive) {
		switch (this->m_networkType)
		{
		case NetworkType::SERVER:
			while (enet_host_service(this->m_server->host, &this->m_server->enetEvent, 0) > 0)
			{
				switch (this->m_server->enetEvent.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					EventQueue::s_eventQueue->AddEventToQueue(
						new Event(EventTypes::NETWORK_SERVER_CONNECT, deltaTime)
					);
					this->OnServerConnect();
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					EventQueue::s_eventQueue->AddEventToQueue(
						new Event(EventTypes::NETWORK_SERVER_DISCONNECT, deltaTime)
					);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					EventQueue::s_eventQueue->AddEventToQueue(
						new Event(EventTypes::NETWORK_SERVER_RECEIVE, deltaTime)
					);
					this->OnServerReceive();
					break;
				}
			}
			break;
		case NetworkType::CLIENTS:
			while (enet_host_service(this->m_client->host, &this->m_client->enetEvent, 0) > 0)
			{
				switch (this->m_client->enetEvent.type)
				{
				case ENET_EVENT_TYPE_RECEIVE:
					EventQueue::s_eventQueue->AddEventToQueue(
						new Event(EventTypes::NETWORK_CLIENT_RECEIVE, deltaTime)
					);
					this->OnClientReceive();
					break;
				}
			}
			break;
		}
	}
}

void NetworkEngine::DrawRemoteData() {
	if (this->m_networkType == NetworkType::SERVER) {
		// Update the specific graphic entity from the network
	}
	else if(this->m_networkType == NetworkType::CLIENTS) {
		// Update the specific graphic entity from the network
	}

	// Update the position of the player coming from a certain peer
	for (unsigned int i = 0; i < this->m_remoteCharacter.size(); i++) {
		if (this->m_playerData->dataActive && this->m_remoteCharacter[i] != nullptr) {
			this->m_remoteCharacter[i]->DrawMesh();
		}
	}
}

void NetworkEngine::OnEvent(Event& e) {
	EventHandler eh(&e);

	if (this->m_networkAdapterActive) {
		// The events require to have the pool directly on entry for this
		// events to fire so it can access the events, because after they go out
		// of scope, the memory is lost

		// eh.Handle(EventTypes::NETWORK_SERVER_CONNECT, std::bind(&NetworkEngine::OnServerConnect, this));
		// eh.Handle(EventTypes::NETWORK_SERVER_RECEIVE, std::bind(&NetworkEngine::OnServerReceive, this));
		// eh.Handle(EventTypes::NETWORK_CLIENT_RECEIVE, std::bind(&NetworkEngine::OnClientReceive, this));

		eh.Handle(EventTypes::NETWORK_SERVER_DISCONNECT, std::bind(&NetworkEngine::OnServerDisconnect, this));
		eh.Handle(EventTypes::PLAYER_FINISH, std::bind(&NetworkEngine::OnLevelFinish, this));
	}
}

void NetworkEngine::OnLevelFinish() {
	if (this->m_networkType == NetworkType::CLIENTS) {
		this->m_sceneData->sceneSelected++;

		this->m_client->dataPacket = enet_packet_create(this->m_sceneData, sizeof(RemoteSceneData), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(this->m_client->peer, 0, this->m_client->dataPacket);
	}
}

void NetworkEngine::OnServerConnect() {
	std::cout << "NETWORK ADAPTER SUCCESS: A client connected from local address.\n\n";

	this->m_server->clientCount++;
	this->m_sceneData->clientRef = this->m_server->clientCount;
	this->m_server->dataPacket = enet_packet_create(this->m_sceneData, sizeof(RemoteSceneData), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(this->m_server->enetEvent.peer, 0, this->m_server->dataPacket);
}

void NetworkEngine::OnServerDisconnect() {
	std::cout << "NETWORK ADAPTER SUCCESS: A client has disconnected from local address.\n\n";
}

void NetworkEngine::OnServerReceive() {
	int tempPacketIndex = -1;
	memcpy(&tempPacketIndex, this->m_server->enetEvent.packet->data, sizeof(int));

	if (tempPacketIndex == 0) {
		memcpy(this->m_playerData, this->m_server->enetEvent.packet->data, sizeof(RemotePlayerData));

		if (this->m_remoteCharacter[this->m_playerData->clientNumber - 1] == nullptr) {
			this->m_remoteCharacter[this->m_playerData->clientNumber - 1] = new GameObject(
				new Transform(),
				this->m_graphicEngineEntity->GetSceneManager()->GetScenePlayer()->GetMeshes(),
				false,
				0,
				ObjectType::PLAYER
			);
		}

		if (this->m_networkType != NetworkEngine::ConvertIntToType(this->m_playerData->networkSource)) {
			this->m_remoteCharacter[this->m_playerData->clientNumber - 1]->GetTransform()->SetPos(this->m_playerData->transform.GetPos());
			this->m_remoteCharacter[this->m_playerData->clientNumber - 1]->GetTransform()->SetRot(this->m_playerData->transform.GetRot());
			this->m_remoteCharacter[this->m_playerData->clientNumber - 1]->GetTransform()->SetScale(this->m_playerData->transform.GetScale());
		}
	}
	else if (tempPacketIndex == 1) {
		memcpy(this->m_sceneData, this->m_server->enetEvent.packet->data, sizeof(RemoteSceneData));
		this->m_graphicEngineEntity->SetSceneManager(this->m_graphicEngineEntity
			->GetGUIEngine()
			->GetLevelDataById(this->m_sceneData->sceneSelected - 1));

		this->m_server->dataPacket = enet_packet_create(this->m_sceneData, sizeof(RemoteSceneData), ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(this->m_server->host, 0, this->m_server->dataPacket);
	}
}

void NetworkEngine::OnClientReceive() {
	int tempPacketIndex = -1;
	memcpy(&tempPacketIndex, this->m_client->enetEvent.packet->data, sizeof(int));

	if (tempPacketIndex == 0) {
		memcpy(this->m_playerData, this->m_client->enetEvent.packet->data, sizeof(RemotePlayerData));

		if (this->m_playerData->clientNumber != 0 
			&& this->m_playerData->clientNumber != this->m_client->clientNumber) {
			if (this->m_remoteCharacter[this->m_playerData->clientNumber - 1] == nullptr) {
				this->m_remoteCharacter[this->m_playerData->clientNumber - 1] = new GameObject(
					new Transform(),
					this->m_graphicEngineEntity->GetSceneManager()->GetScenePlayer()->GetMeshes(),
					false,
					0,
					ObjectType::PLAYER
				);
			}

			if (this->m_networkType != NetworkEngine::ConvertIntToType(this->m_playerData->networkSource)) {
				this->m_remoteCharacter[this->m_playerData->clientNumber - 1]->GetTransform()->SetPos(this->m_playerData->transform.GetPos());
				this->m_remoteCharacter[this->m_playerData->clientNumber - 1]->GetTransform()->SetRot(this->m_playerData->transform.GetRot());
				this->m_remoteCharacter[this->m_playerData->clientNumber - 1]->GetTransform()->SetScale(this->m_playerData->transform.GetScale());
			}
		}
	}
	else if (tempPacketIndex == 1){
		memcpy(this->m_sceneData, this->m_client->enetEvent.packet->data, sizeof(RemoteSceneData));
		this->m_graphicEngineEntity->SetSceneManager(this->m_graphicEngineEntity
			->GetGUIEngine()
			->GetLevelDataById(this->m_sceneData->sceneSelected - 1));

		if (this->m_client->clientNumber == 0) {
			this->m_client->clientNumber = this->m_sceneData->clientRef;
		}
	}
}
