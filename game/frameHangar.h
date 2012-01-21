#ifndef _HANGAR_WINDOW_H_
#define _HANGAR_WINDOW_H_

class HangarWindow : public GUI::Object
{
public:	
	SharedPtr<GUI::Button> btnBack, btnEdit, btnRun, btnNext, btnPrevious;
	SharedPtr<GUI::Frame> viewBlueprints;		/// current blueprints
	Game * game;
	HangarWindow(Game * core);

	size_t currentBlueprint;	/// selected blueprint

	ShipBlueprint * getSelectedBlueprint();
	void frameBack();			/// back to menu
	void frameEdit();			
	void frameRun();

	/// messages from the shipyard
};

#endif