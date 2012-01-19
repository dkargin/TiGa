#include "stdafx.h"
#include "game.h"
#include "frameBattle.h"

GameplayWindow::GameplayWindow(Game * game)
	:GUI::Object(hgeRect(0,0,0,0)),game(game)
{
}