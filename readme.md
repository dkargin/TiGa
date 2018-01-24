This is an old sumulation/game platform that I used as a base for my graduation project

# Requirements #

 - HGE: Haaf's game engine
 - tecgraf iup. That hurts (
 - lua 5.1. Upgrading it to 5.3
 - SWIG. I use it to make lua bindings. I have used a patched version of SWIG, which I've lost. So it will be quite hard to make it work again in the same way
 - (github.com/dkargin/frosttools)[frosttools]. I want to strip it away
 - pathproject. Another my library for dealing with pathfinding stuff

# Current plan #

1. Make C++ part mostly buildable by cmake
1. Deal with SWIG/Lua. I want to get more direct bindings between simulation objects and lua scripts
1. Run it again
1. Blow half of it and refactor. Old idea of ObjectDef<->Object pair is total crap and I should rework it. Any sort of 'ObjectDef' stuff will be merged to appropriate class
1. Implement delta-sync for sim objects. Making a special 'state sync' packet type for every object is very very bad idea. Not so bad, but it takes too much effort to extend it with new data, especially from scripts
1. Fix IUP-related stuff. I need some generic ugly UI to observe game/sim data

