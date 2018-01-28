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
	
	
```
		TickN-------------------------- -----------------------------------TickN+1
World1	|ReadPhysics1|UpdateObjects1|WriteNet1|WritePhysics1|UpdatePhysics1|ReadPhysics2|UpdateObjects2|
```

Threaded approach

```
		TickN--------------------------------------------------------	-----------------------TickN+1
World1	|Read physics|UpdateObjects1|WritePhysics1|WriteNet1|Read physics|UpdateObjects 1|Write physics 1|Write net sync|
Physics1			  				                |UpdatePhysics1|                                            |UpdatePhysics2|
Network

```
```
