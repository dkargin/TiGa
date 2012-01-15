#ifndef _GAME_H_
#define _GAME_H_ 

#include "Core.h"

class MenuWindow;
class Game;
class World;
class ShipyardWindow;

class GameplayWindow : public GUI::Object
{
public:
	
	GameplayWindow(Game * core);
};

class MenuWindow : public GUI::Frame
{
public:
	MenuWindow(Game * core);
	~MenuWindow();

	void addTestScene(const char * scene);
	Instance<GUI::Button> options, exit, start;
	std::list<GUI::Button*> scenes;
};

class Game : public Core
{
public:
	Game();
	~Game();

	std::list<std::string> sceneNames;
	SharedPtr<MenuWindow> mainMenu;
	SharedPtr<GameplayWindow> gameplay;
	SharedPtr<ShipyardWindow> shipyard;
	World * world;
	WeakPtr<GUI::Object> active;
 
	virtual void onRender();
	virtual void onUpdate();

	virtual void registerTestScene(const char * scene);

	virtual void createWorld();

	void showMainMenu();
	void showShipyard();
protected:
	void makeActive(GUI::Object * active);
};

// common UI style constants
const float buttonWidth = 100;
const float buttonHeight = 20;
const float buttonSpacing = 5;

const DWORD clrButtons = ARGB(255,128,128,128);
#endif