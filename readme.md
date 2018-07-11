This is an old sumulation/game platform that I used as a base for my graduation project

# Requirements #

 - tecgraf iup. That hurts (
 - lua 5.1. Upgrading it to 5.3
 - SWIG. I use it to make lua bindings. I have used a patched version of SWIG, which I've lost. So it will be quite hard to make it work again in the same way
 - SDL2
 
There are some libraries, that are being stripped away

- HGE. It is being replaced on SDL2
- frosttools - my own little 'boost'. Every c++ programmer has his own 'little boost' toolset. I've made a copy of necessary stuff, like math3. The rest is ommited
- pathProject - my own pathfinding library. There were some interesting algorithms. But there is no need for such a complex stuff

# Current plan #

1. Make C++ part mostly buildable by cmake
	- add all projects to cmake			DONE
	- fix FX/GUI library. 				DONE mostly
		- strip direct references to HGE Mostly
		- implement proper wrappers for resource access: RenderContext, PolyBatch
	- add proper HGE version. 			HGE is not going to work properly. We will move to SDL
	- fix sim					In progress
	- fix game					Seems to be pointless to fix
	- replace HGE stuff by SDL
		- TextureManager
		- Image loading
		- SoundManager
		- Font stuff
	- where's my Scripter stuff?		DONE

1. Make it runnable again. Just in minimal way, without scripts
1. Deal with SWIG/Lua. I want to get more direct bindings between simulation objects and lua scripts
1. Blow half of it and refactor. Old idea of ObjectDef<->Object pair is total crap and I should rework it. Any sort of 'ObjectDef' stuff will be merged to appropriate class
1. Implement delta-sync for sim objects. Making a special 'state sync' packet type for every object is very very bad idea. Not so bad, but it takes too much effort to extend it with new data, especially from scripts
1. Implement spinner for lua coroutined scripts. Who needs scripts without proper async spinners?
1. Fix IUP-related stuff. I need some generic ugly UI to observe game/sim data

