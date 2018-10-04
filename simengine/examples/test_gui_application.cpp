#include <exception>
#include <functional>

#include "../application.h"
#include "gui/guictrls.h"

const float buttonWidth = 100;
const float buttonHeight = 20;
const float buttonSpacing = 5;

const Fx::FxRawColor clrButtons = Fx::makeARGB(255,128,128,128);

class MenuWindow : public GUI::Frame
{
public:
	MenuWindow(GUI::FontPtr font)
	{
		setAlign(GUI::AlignCenter, GUI::AlignExpand);

		color = Fx::makeARGB(255,128,128,128);
		setDesiredSize(300,300);

		float y = 0;

		start.reset(new GUI::Button());
		start->setDesiredPos(0, 100 + (buttonHeight + buttonSpacing) * y++);
		start->setDesiredSize(buttonWidth, buttonHeight);
		start->setText("Start", font);
		start->onPressed = [=]()
		{
			//game->showHangar();
		};
		start->setAlign(GUI::AlignExpand, GUI::AlignManual);
		insertWidget(start.get());

		options.reset(new GUI::Button());
		options->setDesiredPos(0, 100 + (buttonHeight + buttonSpacing) * y++);
		options->setDesiredSize(buttonWidth, buttonHeight);
		options->setText("Options", font);
		options->setAlign(GUI::AlignExpand, GUI::AlignManual);
		insertWidget(options.get());

		exit.reset(new GUI::Button());
		exit->setDesiredPos(0, 100 + (buttonHeight + buttonSpacing) * y++);
		exit->setDesiredSize(buttonWidth, buttonHeight);
		exit->onPressed = [&]()
		{
			//game->exit();
			// TODO: Exit somehow
		};
		exit->setText("Exit", font);
		exit->setAlign(GUI::AlignExpand, GUI::AlignManual);
		insertWidget(exit.get());

		for(std::string sceneName : {"first", "second", "third"})
		{
			auto button = std::make_shared<GUI::Button>();

			button->setDesiredSize(buttonWidth, buttonHeight);
			button->setDesiredPos(0, 120 + (buttonHeight + buttonSpacing) * y++);
			button->setText(sceneName.c_str(), font);
			button->onPressed = [=]()
			{
				//game->loadTestScene(sceneName.c_str());
				//this->visible = false;
			};
			button->setAlign(GUI::AlignExpand, GUI::AlignManual);
			insertWidget(button.get());
			sceneButtons.push_back(button);
		}
	}

	~MenuWindow()
	{
		auto tmp = std::move(sceneButtons);
	}

	void addTestScene(const char * scene);
	bool onMouseMove(int mouseId, const uiVec & vec, MoveState state)
	{
		//if( state == GUI::Object::MoveEnter)
		//	_RPT0(0,"mouseEnter\n");
		//if( state == GUI::Object::MoveLeave)
		//	_RPT0(0,"mouseLeave\n");
		return false;
	}

	std::shared_ptr<GUI::Button> options, exit, start;
	std::list< GUI::ButtonPtr> sceneButtons;
};


class TestApplication : public sim::Application
{
public:
	TestApplication()
	{

	}
	void onInit() override
	{
		font.reset(new Fx::Font("./data/font1.fnt"));
		main = std::make_shared<MenuWindow>(font);
		guiRoot->insertWidget(main.get());
	}

	GUI::FontPtr font;
	std::shared_ptr<MenuWindow> main;
};

int main(int argc, char** argv)
{
	TestApplication app;

	if (!app.init())
	{
		printf("Failed to init application. Exiting\n");
		return -1;
	}
	app.run();
	printf("done\n");
	return 0;
}
