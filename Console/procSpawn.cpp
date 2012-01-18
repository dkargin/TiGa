#include <tchar.h>
#include <process.h>
#include <conio.h>
#include <map>
#include <functional>
#include <errno.h>
#include <stdlib.h>

#pragma warning(disable: 4996)

const char port[] = "15600";
const char client_adress[] = "localhost";

const char * server_args[] = { "", port}; 
const char * client_args[] = { client_adress, port};

const size_t MaxClients = 16;
intptr_t serverId = 0;
size_t lastClientId = 0;

std::map<size_t, intptr_t> clients;

const char * processError()
{
	int err = errno;
	return strerror(err);
}

size_t spawnClient()
{	
	intptr_t ptr = _spawnl(_P_NOWAITO, "TiGaClient.exe", ".\\TiGaClient.exe", "127.0.0.1", "15600" );
	if( ptr != -1 )
	{
		size_t id = ++lastClientId;
		clients[id] = ptr;
		return id;
	}
	else 
		printf("Error: %s\n", processError());
	return 0;
}

typedef std::pair<const char *, std::function<void (void)> > MenuEntry;
typedef std::map<int, MenuEntry > KeyMap;

#define MENU_BEGIN(key, name) keyMap[(key)] = std::make_pair((name),[&]()->void{
#define MENU_END() })

void printMenu(KeyMap & keyMap)
{
	for( auto it = keyMap.begin(); it != keyMap.end(); ++it)
	{
		printf("%c.\t%s\n",it->first, it->second.first);
	}
}

void startConsole()
{	
	//intptr_t serverId = _spawnl(_P_NOWAIT, "TiGaServ.exe", "TiGaServ.exe", "15600" ); 
	//intptr_t clientId = _spawnl(_P_NOWAIT, "TiGaClient.exe", "TiGaServ.exe", "localhost", "15600" );
	bool exit = false;
	KeyMap keyMap;

	MENU_BEGIN('1',"Print menu")
		printMenu(keyMap);
	MENU_END();
	MENU_BEGIN('2',"Start server")
		system("start TiGaServ.exe 15600" ); 
		//if( serverId == 0 )
		//	serverId = _spawnl(_P_NOWAITO, "TiGaServ.exe", "TiGaServ.exe", "15600" ); 
	MENU_END();

	MENU_BEGIN('3',"Start client")
		system("start TiGaClient.exe 127.0.0.1 15600" ); 
		//size_t id = spawnClient();
		//printf("Client %d spawned\n",id);
	MENU_END();

	MENU_BEGIN(' ',"Exit")
		exit = true;
	MENU_END();
	printMenu(keyMap);
	while(exit != true)
	{
		int ch = getch();		
		KeyMap::iterator it = keyMap.find(ch);
		if( it != keyMap.end() )
			it->second.second();
	}
}