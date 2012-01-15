#include "stdafx.h"
#include "core.h"
#include "world.h"
#include "game.h"

#include "shipyard.h"

Game game;

Game::Game()
	:Core("./script/startup_Game.lua"), mainMenu(NULL), gameplay(NULL), shipyard(NULL), active(NULL)
{}

Game::~Game()
{
	mainMenu = NULL;
	shipyard = NULL;
	/*if(mainMenu)
		delete mainMenu;
	if(shipyard)
		delete shipyard;*/
}
class TestWindow : public GUI::Object
{
public:
	Instance<GUI::Frame> frame;
	SharedPtr<GUI::Button> button;
	TestWindow()
		:Object(hgeRect())
	{
		//frame = new GUI::Frame;
		insert(frame);
		button = new GUI::Button;
		frame->insert(button);
	}
	~TestWindow()
	{
	}
};

void Game::createWorld()
{
	Core::createWorld();	
	{
		//TestWindow window;
	}
	showMainMenu();
}

void Game::registerTestScene(const char * scene)
{
	sceneNames.push_back(scene);	
}

void Game::onRender()
{
	Core::onRender();
}

void Game::onUpdate()
{
	Core::onUpdate();
}

void Game::showMainMenu()
{
	if(!mainMenu)
	{
		mainMenu = new MenuWindow(this);
		guiRoot->insert(mainMenu, GUI::AlignCenter, GUI::AlignExpand);
	}
	makeActive(mainMenu);
}

void Game::showShipyard()
{
	if(!shipyard)
	{
		shipyard = new ShipyardWindow(this);
		guiRoot->insert(shipyard, GUI::AlignExpand, GUI::AlignExpand);
	}
	makeActive(shipyard);
}

void Game::makeActive(GUI::Object * object)
{
	if(active == object)
		return;

	if(active)	
		active->show(false);
	active = object;

	if(active)
		active->show(true);
}

///////////////////////////////////////////////////////////////////////////////////
MenuWindow::MenuWindow(Game * game)
{
	color = ARGB(255,128,128,128);
	setDesiredSize(300,300);
	
	float y = 0;
	
	start->setDesiredPos(0, 100 + (buttonHeight + buttonSpacing) * y++);
	start->setDesiredSize(buttonWidth, buttonHeight);
	start->setText("Start", game->font);
	start->onPressed = [=]()
	{
		game->showShipyard();
	};
	insert(start, GUI::AlignExpand, GUI::AlignManual); 
	options->setDesiredPos(0, 100 + (buttonHeight + buttonSpacing) * y++);
	options->setDesiredSize(buttonWidth, buttonHeight);
	options->setText("Options", game->font);
	insert(options, GUI::AlignExpand, GUI::AlignManual);
	
	exit->setDesiredPos(0, 100 + (buttonHeight + buttonSpacing) * y++);	
	exit->setDesiredSize(buttonWidth, buttonHeight);
	exit->onPressed = [&]()
	{
		game->exitHGE();
	};
	exit->setText("Exit",game->font);	 
	insert(exit, GUI::AlignExpand, GUI::AlignManual);
	
	
	for(auto it = game->sceneNames.begin(); it != game->sceneNames.end(); ++it)
	{
		GUI::Button * button = new GUI::Button;
		std::string sceneName = *it;
		button->setDesiredSize(buttonWidth, buttonHeight);
		button->setDesiredPos(0, 120 + (buttonHeight + buttonSpacing) * y++);
		button->setText(it->c_str(),game->font);
		button->onPressed = [=]()
		{
			core->loadTestScene(sceneName.c_str());
			this->visible = false;
		};
		insert(button, GUI::AlignExpand, GUI::AlignManual);
		scenes.push_back(button);
	}	
}

MenuWindow::~MenuWindow()
{
	while(!scenes.empty())
	{
		delete scenes.front();
		scenes.pop_front();
	}
}