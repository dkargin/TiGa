#include "stdafx.h"
#include "game.h"
#include "frameHangar.h"
#include "frameShipyard.h"

HangarWindow::HangarWindow(Game * game)
	:GUI::Object(hgeRect(0,0,0,0)),game(game)
{
	currentBlueprint = 0;
	color = ARGB(255,128,128,128);		
	float y = 0;

	const float menuOffsetHor = 100;
	
	btnEdit = new GUI::Button();
	btnEdit->setDesiredPos(menuOffsetHor, 100 + (buttonHeight + buttonSpacing) * y++);
	btnEdit->setDesiredSize(buttonWidth, buttonHeight);
	btnEdit->setText("Edit", game->font);
	btnEdit->onPressed = [=]()
	{
		frameEdit();
	};
	insert(btnEdit, GUI::AlignManual, GUI::AlignManual); 

	btnRun = new GUI::Button();
	btnRun->setDesiredPos(menuOffsetHor, 100 + (buttonHeight + buttonSpacing) * y++);
	btnRun->setDesiredSize(buttonWidth, buttonHeight);
	btnRun->setText("Run", game->font);
	btnRun->onPressed = [=]()
	{
		frameRun();
	};
	insert(btnRun, GUI::AlignManual, GUI::AlignManual); 

	btnBack = new GUI::Button();
	btnBack->setDesiredPos(menuOffsetHor, 100 + (buttonHeight + buttonSpacing) * y++);
	btnBack->setDesiredSize(buttonWidth, buttonHeight);
	btnBack->setText("Back", game->font);
	btnBack->onPressed = [=]()
	{
		frameBack();
	};
	insert(btnBack, GUI::AlignManual, GUI::AlignManual); 
}

void HangarWindow::frameBack()
{
	game->showMainMenu();
}

void HangarWindow::frameRun()
{
	game->showGameplay();
}

void HangarWindow::frameEdit()
{
	game->showShipyard();
	game->shipyard->setBlueprint(getSelectedBlueprint(),false);
}

ShipBlueprint * HangarWindow::getSelectedBlueprint()
{
	// TODO: implement
	return &game->gameData->blueprints[currentBlueprint];
}