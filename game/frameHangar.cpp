#include "stdafx.h"
#include "game.h"
#include "frameHangar.h"
#include "frameShipyard.h"
#include "frameBattle.h"

HangarWindow::HangarWindow(Game * game)
	:GUI::Object(hgeRect(0,0,0,0)),game(game)
{
	setAlign(GUI::AlignExpand, GUI::AlignExpand);

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
	btnEdit->setAlign(GUI::AlignManual, GUI::AlignManual);
	insert(btnEdit);

	btnRun = new GUI::Button();
	btnRun->setDesiredPos(menuOffsetHor, 100 + (buttonHeight + buttonSpacing) * y++);
	btnRun->setDesiredSize(buttonWidth, buttonHeight);
	btnRun->setText("Run", game->font);
	btnRun->onPressed = [=]()
	{
		frameRun();
	};
	btnRun->setAlign(GUI::AlignManual, GUI::AlignManual); 
	insert(btnRun);

	btnBack = new GUI::Button();
	btnBack->setDesiredPos(menuOffsetHor, 100 + (buttonHeight + buttonSpacing) * y++);
	btnBack->setDesiredSize(buttonWidth, buttonHeight);
	btnBack->setText("Back", game->font);
	btnBack->onPressed = [=]()
	{
		frameBack();
	};
	btnBack->setAlign(GUI::AlignManual, GUI::AlignManual); 
	insert(btnBack);
}

void HangarWindow::frameBack()
{
	game->showMainMenu();
}

void HangarWindow::frameRun()
{
	game->selectedBlueprint = getSelectedBlueprint();
	game->makeActive(new GameplayWindow(game));	
}

void HangarWindow::frameEdit()
{
	game->selectedBlueprint = getSelectedBlueprint();
	game->makeActive(new ShipyardWindow(game));		
}

ShipBlueprint * HangarWindow::getSelectedBlueprint()
{
	// TODO: implement
	return &game->gameData->blueprints[currentBlueprint];
}