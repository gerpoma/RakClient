#include "bridge.h"
#include "RakPeerInterface.h"

class Client {
public:
    Client();
	~Client();
    static Client& getInstance()
	{
        static Client    instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return instance;
    }
	void init();
	void message(char* str);
	void setUsername(const char* name);
	void shutdown();
	RakNet::RakPeerInterface* client;
private:
    const char* name;
};
