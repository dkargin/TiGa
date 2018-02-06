#pragma once

#include <set>
#include <list>

#include "basetypes.h"
/*
 * Contains data for network client
 */

namespace sim
{

/// Why the hell is this structure doing here?
	class ClientInfo
	{
	public:
		typedef size_t ID;
		bool sendObjects;			//< send objectCreate msg for all objects
		bool updateObjects;		//< send objectUpdate msg for all objects
		long lastSync;
		std::set<ID> objects;
		std::list<ID> destroyed;
		StreamOut* messages;		// additional data to send
	public:
		ClientInfo();
		~ClientInfo();
		bool haveMessages()const;
		StreamOut & getMessages();
		void cleanMessages();
	};


} // namespace sim
