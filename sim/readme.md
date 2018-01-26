# Generic 2d simulation #

Notable classes:
 - GameObject
 - SimScene - a single physical instance

## Game ##

- contains a set of simulation scenes
- contains network manager that deals with sync when sim finishes its update
- contains DB for object definitions. Maybe I should move it to a separate class

## Scene ##

Simulation scene
It can use physics system. Or do without physics sim
It contains a set of game objects and deals with updating them

## GameObject ##
	




## Update process ##

For each SimScene:
	