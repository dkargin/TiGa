add_subdirectory(luabox)

add_library(sim
	solidobject.cpp
	gameObject.cpp
	inventory.cpp
	device.cpp
	mover.cpp
	moverCharacter.cpp
	moverVehicle.cpp
	moverVehicleDriver.cpp
	objectManager.cpp
	perception.cpp
	perceptionAI.cpp
	projectile.cpp
	thruster.cpp
	turret.cpp
	weapon.cpp
	weaponBeam.cpp
	weaponProjectile.cpp
	weaponTurret.cpp
	
)

target_link_libraries(sim lua)
