#pragma once
class InetClients
{
public:	
	///Вызывается у обоих, только клиенту от сервера приходит про другого клиента
	virtual void onClientNew(int Id)=0;
	virtual void onClientLost(int Id)=0;
	virtual void onDisconnect()=0;
};

namespace netLowHigh // low and high level network IO
{
	bool init(lua_State *L,InetClients *Reader);
	void done();

/// средний уровень - регистрация юзеров и рассылка данных
/// использовать для отправки сокет такой-то 0-1
	void switchSock(int SockN);

	// - при этом если вызывать луа - то нужно поместить в стэк таблицу, функции которой дергать
	// коннектится
	bool doConnect(char *Name,int Port,bool attachLua=false);
	// запускает сервак
	bool doListen(int Port,bool attachLua=false);

///  получить мой адрес (первый), 0 - всем
	int getMyAddr();

/// шлет адресату такому-то сообщение Ch (0-255)
	void send(int Addr,int Ch,char *Data, int Len);
// надо определится, вызывать его снаружи или изнутри в новом триде
	void Update();
}

namespace netLowHigh
{
	bool isConnected();
	void send(char const *data,size_t sz);
	std::string readBuffer();
	//void freeBuffer();
	void run();
}

