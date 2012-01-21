#include "stdafx.h"
#include "game.h"
#include "frameMainMenu.h"

MenuWindow::MenuWindow(Game * game)
{
	setAlign(GUI::AlignCenter, GUI::AlignExpand);

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
	start->setAlign(GUI::AlignExpand, GUI::AlignManual); 
	insert(start);
	options->setDesiredPos(0, 100 + (buttonHeight + buttonSpacing) * y++);
	options->setDesiredSize(buttonWidth, buttonHeight);
	options->setText("Options", game->font);
	options->setAlign(GUI::AlignExpand, GUI::AlignManual);
	insert(options);
	
	exit->setDesiredPos(0, 100 + (buttonHeight + buttonSpacing) * y++);	
	exit->setDesiredSize(buttonWidth, buttonHeight);
	exit->onPressed = [&]()
	{
		game->exitHGE();
	};
	exit->setText("Exit",game->font);
	exit->setAlign(GUI::AlignExpand, GUI::AlignManual);
	insert(exit);
	
	
	for(auto it = game->sceneNames.begin(); it != game->sceneNames.end(); ++it)
	{
		GUI::Button * button = new GUI::Button;
		std::string sceneName = *it;
		button->setDesiredSize(buttonWidth, buttonHeight);
		button->setDesiredPos(0, 120 + (buttonHeight + buttonSpacing) * y++);
		button->setText(it->c_str(),game->font);
		button->onPressed = [=]()
		{
			game->loadTestScene(sceneName.c_str());
			this->visible = false;
		};
		button->setAlign(GUI::AlignExpand, GUI::AlignManual);
		insert(button);
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