#ifndef _FRAME_MAIN_MENU_H_
#define _FRAME_MAIN_MENU_H_

class Game;

class MenuWindow : public GUI::Frame
{
public:
	MenuWindow(Game * core);
	~MenuWindow();

	void addTestScene(const char * scene);
	bool onMouseMove(int mouseId, const uiVec & vec, MoveState state);
	Instance<GUI::Button> options, exit, start;
	std::list<GUI::Button*> scenes;
};

#endif