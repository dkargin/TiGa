This is an old sumulation/game platform that I used as a base for my graduation project

# Requirements #

 - SWIG. I use it to make lua bindings. I have used a patched version of SWIG, but this version is lost. So it will be quite hard to make it work again in the same way
 - SDL2, SDL2\_image, SDL2_ttf
 
I do use a local version of lua

# Project structure #

simengine/
 - fx - contains primitive 2d sprite engine
 - gui - GUI library based on `fx`
 - luabox - LUA 5.3 + additional utilities. Build as a single library.
 - sim - some game specific classes. Once upon a time it was used in my graduation project.
game/ - was containing some real tanks-pew-pew stuff before I broke everything badly.
 
# Current plan #

1. Fix all the parts
	- add all projects to cmake			DONE
	- fix FX/GUI library. 				DONE mostly
	- fix sim					In progress
	- fix game					Seems to be pointless to fix
	- replace HGE stuff by SDL
		- TextureManager.		Fixed partially
		- Image loading			Using SDL2_Image
		- SoundManager			TODO:
		- Font stuff			Will be using SDL2_TTF
1. Make a set of tests and examples:
	- example 'hello world' window application	->	DONE
	- demo test for Fx library
		- sprites, animations, beams, particles
		- sounds
	- visual test for UI library
	- scripter tests
	- network replication tests
1. Deal with SWIG/Lua. I want to get more direct bindings between simulation objects and lua scripts
1. Implement delta-sync for sim objects. Making a special 'state sync' packet type for every object is very very bad idea. Not so bad, but it takes too much effort to extend it with new data, especially from scripts
1. Implement spinner for lua coroutined scripts. Who needs scripts without proper async spinners?

