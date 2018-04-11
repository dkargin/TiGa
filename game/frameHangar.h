#pragma once

class HangarWindow : public GUI::Object
{
public:
	HangarWindow(Game * core);

	ShipBlueprint * getSelectedBlueprint();
	void frameBack();			/// back to menu
	void frameEdit();			
	void frameRun();

	std::shared_ptr<GUI::Button> btnBack, btnEdit, btnRun, btnNext, btnPrevious;
	/// current blueprints
	std::shared_ptr<GUI::Frame> viewBlueprints;
	Game * game;
	/// selected blueprint
	size_t currentBlueprint;
};
