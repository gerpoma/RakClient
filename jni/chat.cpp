/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// ----------------------------------------------------------------------
// RakNet version 1.0
// Filename ChatExample.cpp
// Very basic chat engine example
// ----------------------------------------------------------------------

#include "MessageIdentifiers.h"

#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "PacketLogger.h"
#include <assert.h>
#include <cstdio>
#include <stdlib.h>
#include "RakNetTypes.h"
#ifdef _WIN32
#include "Kbhit.h"
#include "WindowsIncludes.h" // Sleep
#else
#include "Kbhit.h"
#include <unistd.h> // usleep
#endif
#include "Gets.h"
#include <string>
#include "bridge.h"
#include "client.h"
#include <memory>

#if LIBCAT_SECURITY==1
#include "SecureHandshake.h" // Include header for secure handshake
#endif

// We copy this from Multiplayer.cpp to keep things all in one file for this example
unsigned char GetPacketIdentifier(RakNet::Packet *p);

int clientmain()
{
	RakNet::RakNetStatistics *rss;
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	RakNet::RakPeerInterface *client=RakNet::RakPeerInterface::GetInstance();
	//set client here, what if we want to send messages via java
	Client::getInstance().client = client;
//	client->InitializeSecurity(0,0,0,0);
	//RakNet::PacketLogger packetLogger;
	//client->AttachPlugin(&packetLogger);

	
	// Holds packets
	RakNet::Packet* p;

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	// Just so we can remember where the packet came from
	bool isServer;

	// Record the first client that connects to us so we can pass it to the ping function
	RakNet::SystemAddress clientID=RakNet::UNASSIGNED_SYSTEM_ADDRESS;

	// Crude interface

	// Holds user data
	char ip[64], serverPort[30], clientPort[30];

	// A client
	isServer=false;

	strcpy(clientPort, "0");

	client->AllowConnectionResponseIPMigration(false);
	strcpy(ip, "147.185.221.16");
	// strcpy(ip, "natpunch.jenkinssoftware.com");

	strcpy(serverPort, "7196");

	// Connecting the client is very simple.  0 means we don't care about
	// a connectionValidationInteger, and false for low priority threads
	RakNet::SocketDescriptor socketDescriptor(atoi(clientPort),0);
	socketDescriptor.socketFamily=AF_INET;
	client->Startup(8,&socketDescriptor, 1);
	client->SetOccasionalPing(true);


#if LIBCAT_SECURITY==1
	char public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
	FILE *fp = fopen("publicKey.dat","rb");
	fread(public_key,sizeof(public_key),1,fp);
	fclose(fp);
#endif

#if LIBCAT_SECURITY==1
	RakNet::PublicKey pk;
	pk.remoteServerPublicKey=public_key;
	pk.publicKeyMode=RakNet::PKM_USE_KNOWN_PUBLIC_KEY;
	bool b = client->Connect(ip, atoi(serverPort), "Rumpelstiltskin", (int) strlen("Rumpelstiltskin"), &pk)==RakNet::CONNECTION_ATTEMPT_STARTED;	
#else
	RakNet::ConnectionAttemptResult car = client->Connect(ip, atoi(serverPort), "Rumpelstiltskin", (int) strlen("Rumpelstiltskin"));
	RakAssert(car==RakNet::CONNECTION_ATTEMPT_STARTED);
#endif

	printf("\nMy IP addresses:\n");
	unsigned int i;
	for (i=0; i < client->GetNumberOfAddresses(); i++)
	{
		printf("%i. %s\n", i+1, client->GetLocalIP(i));
	}

	printf("My GUID is %s\n", client->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
	puts("'quit' to quit. 'stat' to show stats. 'ping' to ping.\n'disconnect' to disconnect. 'connect' to reconnnect. Type to talk.");
    
    bool wawawa = true;
	
	Client::getInstance().message(""); //hacky fix to prevent crashing

	// Loop for input
	while (1)
	{
		// This sleep keeps RakNet responsive
#ifdef _WIN32
		Sleep(30);
#else
		usleep(30 * 1000);
#endif

		// Get a packet from either the server or the client

		for (p=client->Receive(); p; client->DeallocatePacket(p), p=client->Receive())
		{
			// We got a packet, get the identifier with our handy function
			packetIdentifier = GetPacketIdentifier(p);

			// Check if this is a network message packet
			switch (packetIdentifier)
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
                Bridge::getInstance().bridge_warn("Disconnected; Restart app.");
				Bridge::getInstance().callSFunc(Bridge::getInstance().lostConn);
				break;
			case ID_ALREADY_CONNECTED:
				// Connection lost normally
				printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", p->guid);
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				Bridge::getInstance().bridge_warn("Invalid protocol version! Are you sure this is the right one?");
				Bridge::getInstance().callSFunc(Bridge::getInstance().lostConn);
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n"); 
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				printf("We are banned from this server.\n");
				break;			
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				Bridge::getInstance().bridge_warn("Connection attempt failed!");
				Bridge::getInstance().callSFunc(Bridge::getInstance().lostConn);
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				// Sorry, the server is full.  I don't do anything here but
				// A real app should tell the user
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				Bridge::getInstance().bridge_warn("Server is full!");
				Bridge::getInstance().callSFunc(Bridge::getInstance().lostConn);
				break;

			case ID_INVALID_PASSWORD:
				printf("ID_INVALID_PASSWORD\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
                Bridge::getInstance().bridge_warn("Disconnected; Restart app.");
				Bridge::getInstance().callSFunc(Bridge::getInstance().lostConn);
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", p->systemAddress.ToString(true), p->guid.ToString());
				printf("My external address is %s\n", client->GetExternalID(p->systemAddress).ToString(true));
				Bridge::getInstance().callVFunc(Bridge::getInstance().tell, "you might have connected to the server successfully!");
				break;
			case ID_CONNECTED_PING:
			case ID_UNCONNECTED_PING:
				printf("Ping from %s\n", p->systemAddress.ToString(true));
				
				break;
			default:
				// It's a client, so just show the message
				Bridge::getInstance().callVFunc(Bridge::getInstance().tell, (const char*)p->data);
				break;
			}
		}
	}

	// Be nice and let the server know we quit.
	//client->Shutdown(300);

	// We're done with the network
	//RakNet::RakPeerInterface::DestroyInstance(client);
	
	//Bridge::getInstance().bridge_warn("Network shutdown");
}

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p==0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char) p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char) p->data[0];
}

//Bridge::getInstance().jvm->AttachCurrentThread(&Bridge::getInstance().env, NULL);
//Bridge::getInstance().bridge_init(Bridge::getInstance().env, Bridge::getInstance().thiz);
//Bridge::getInstance().jvm->DetachCurrentThread();

Client::Client() {
	
}

Client::~Client() {
	
}

void Client::init() {
	clientmain();
}

void Client::message(char* str) {
	if(this->name == NULL) {
		const char* example = Bridge::getInstance().bridge_getDeviceID();
		this->name = example;
	}
	//do some fucking with the str
	const char* aaaf = ": "; //how its like
	char* buffer = (char*)malloc(sizeof(char*)*(strlen(this->name)+strlen(aaaf)+strlen(str)));
	sprintf(buffer, "%s%s%s", this->name, aaaf, str);
	this->client->Send(buffer, (int) strlen(buffer)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	delete buffer;
}

void Client::setUsername(const char* naame) {
	this->name = naame;
}

void Client::shutdown() {
	this->client->Shutdown(300);

	// We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(this->client);
	
	Bridge::getInstance().bridge_warn("Disconnect successful");
	
	//this->client = NULL;
}
