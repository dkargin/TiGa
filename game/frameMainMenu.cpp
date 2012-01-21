#include "stdafx.h"
#include "game.h"
#include "frameMainMenu.h"

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
		game->showHangar();
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

bool MenuWindow::onMouseMove(int mouseId, const uiVec & vec, GUI::Object::MoveState state)
{
	//if( state == GUI::Object::MoveEnter)
	//	_RPT0(0,"mouseEnter\n");
	//if( state == GUI::Object::MoveLeave)
	//	_RPT0(0,"mouseLeave\n");
	return false;
}