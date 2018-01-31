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
	unit.cpp
	weapon.cpp
#	weaponBeam.cpp
#	weaponProjectile.cpp
	weaponTurret.cpp
	controllers/forceController.cpp
	controllers/RVO2Controller.cpp
	controllers/vo2Controller.cpp
	controllers/voController.cpp
)

target_link_libraries(sim lua)
