/*
 * client.cpp
 *
 *  Created on: Feb 5, 2018
 *      Author: vrobot
 */


#include "client.h"
#include "objectManager.h"

namespace sim
{

ClientInfo::ClientInfo()
{
	lastSync = -1;
	sendObjects = false;
	messages = nullptr;
	updateObjects = false;
}

ClientInfo::~ClientInfo()
{
	cleanMessages();
}

bool ClientInfo::haveMessages() const
{
	return messages != nullptr && messages->size() != 0;
}

StreamOut & ClientInfo::getMessages()
{
	if(!messages)
		messages = new StreamOut(nullptr);
	return *messages;
}

void ClientInfo::cleanMessages()
{
	if(messages)
		delete messages;
	messages=NULL;
}

}

