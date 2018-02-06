#pragma once

#include <memory>

namespace sim
{
	class Item;
	class Device;
	class GameObject;
	typedef std::shared_ptr<GameObject> GameObjectPtr;
	typedef std::weak_ptr<GameObject> GameObjectWPtr;
	typedef std::shared_ptr<Device> DevicePtr;

	class Unit;
	class Inventory;
	class Weapon;
	class ObjectManager;
	class Mover;
	class Perception;
	class PerceptionClient;
	class PerceptionManager;
	class Controller;

}
