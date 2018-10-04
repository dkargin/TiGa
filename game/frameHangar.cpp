#include "game.h"
#include "frameHangar.h"
#include "frameShipyard.h"
#include "frameBattle.h"

HangarWindow::HangarWindow(Game * game)
	:GUI::Object(Fx::Rect(0,0,0,0)),game(game)
{
	setAlign(GUI::AlignExpand, GUI::AlignExpand);
	std::shared_ptr<GUI::Image> background(new GUI::Image());
	background->sprite = game->fxManager->fxSprite("./data/garage.png",0,0,800,600);
	background->sprite->setZ(-0.5);
	background->setAlign(GUI::AlignExpand, GUI::AlignExpand);
	insert(background);
	currentBlueprint = 0;
	color = Fx::makeARGB(255,128,128,128);
	float y = 0;

	const float menuOffsetHor = 200;
	const float shipPanel = 200;
	
	btnEdit = std::make_shared<GUI::Button>();
	btnEdit->setDesiredPos(menuOffsetHor, 100 + (buttonHeight + buttonSpacing) * y++);
	btnEdit->setDesiredSize(buttonWidth, buttonHeight);
	btnEdit->setText("Edit", game->font);
	btnEdit->onPressed = [=]()
	{
		frameEdit();
	};
	btnEdit->setAlign(GUI::AlignManual, GUI::AlignManual);
	insert(btnEdit);

	btnRun = std::make_shared<GUI::Button>();
	btnRun->setDesiredPos(menuOffsetHor, 100 + (buttonHeight + buttonSpacing) * y++);
	btnRun->setDesiredSize(buttonWidth, buttonHeight);
	btnRun->setText("Run", game->font);
	btnRun->onPressed = [=]()
	{
		frameRun();
	};
	btnRun->setAlign(GUI::AlignManual, GUI::AlignManual); 
	insert(btnRun);

	btnBack = std::make_shared<GUI::Button>();
	btnBack->setDesiredPos(menuOffsetHor, 100 + (buttonHeight + buttonSpacing) * y++);
	btnBack->setDesiredSize(buttonWidth, buttonHeight);
	btnBack->setText("Back", game->font);
	btnBack->onPressed = [=]()
	{
		frameBack();
	};
	btnBack->setAlign(GUI::AlignManual, GUI::AlignManual); 
	insert(btnBack);

	std::shared_ptr<GUI::Image> currentShip(new GUI::Image);
	currentShip->setDesiredSize(shipPanel, game->getScreenHeight());
	currentShip->setAlign(GUI::AlignMax, GUI::AlignExpand);
	currentShip->sprite = game->fxManager->fxHolder();
	GenerateShipGraphics(currentShip->sprite, getSelectedBlueprint(), game);
	insert(currentShip);
	this->updateLayout();
}

void HangarWindow::frameBack()
{
	game->showMainMenu();
}

void HangarWindow::frameRun()
{
	game->selectedBlueprint = getSelectedBlueprint();
	game->makeActive(std::make_shared<GameplayWindow>(game));
}

void HangarWindow::frameEdit()
{
	game->selectedBlueprint = getSelectedBlueprint();
	auto shipyard = std::make_shared<ShipyardWindow>(game);
	game->makeActive(shipyard);
	if( game->selectedBlueprint )
		shipyard->setBlueprint(game->selectedBlueprint, false);
}

ShipBlueprint * HangarWindow::getSelectedBlueprint()
{
	// TODO: implement
	return &game->gameData->blueprints[currentBlueprint];
}
