#pragma once

class ShipyardWindow;

const int ShipyardCellsMax = 6;
/*
class GameStateShipyard : public GameState
{
public:
	ShipBlueprint * blueprint;
	virtual void onEnter();
	virtual void onExit();
	virtual void onResume( GameState * from );
};
*/

using vec2i = math3::vec2i;

class ShipyardArea : public GUI::Object
{
public:
	ShipyardArea(ShipyardWindow * shipyard);
	~ShipyardArea();
	/*
	class Listener
	{
	public:
		virtual void onDesignFinished(ShipyardArea * shipyard, ShipBlueprint * blueprint) = 0;
	};
	std::shared_ptr<ShipyardListener> listener;
	*/
	struct Cell
	{
		size_t blockId;
		size_t objectId;
	};

	Cell cells[ShipyardCellsMax * ShipyardCellsMax];
	int cellsCenterX, cellsCenterY, cellsWidth, cellsHeight;

	vec2i selectedCell;

	ShipBlueprint blueprint;

	float tileSize;
	float tileOffsetX, tileOffsetY;
	std::weak_ptr<ShipyardWindow> shipyard;
	
	// blueprints
	void setBlueprint( ShipBlueprint * blueprint, bool locked );
	void getBlueprint( ShipBlueprint * blueprint );
	// view
	size_t findTile( int x, int y );
	void addTile( int x, int y, size_t pickedBlock );	
	vec2i screenToLocal( const uiVec & vec );

	bool canPlaceTile( int x, int y, size_t pickedBlock );
	bool canPlaceObject( int x, int y, size_t picledObject);

	void updateCells();	

	void clearContents();
	// UI handlers
	virtual void onRender();
	virtual bool onMouse(int mouseId, int key, int state, const uiVec & vec);
	virtual bool onMouseMove(int mouseId,  const uiVec & vec, MoveState state );
	// Helpers
	vec2i screenCellCenter( const vec2i & cell ) const;
	Fx::Rect screenCellRect( const vec2i & cell ) const;

	const TileSectionDesc & getSectionDesc(size_t id) const;
};

enum ShipyardEditMode
{
	Normal,
	CreateTile,
	SelectedTile,
	DragTile,
	DragDevice,
};

class ShipyardWindow : public GUI::Object
{
public:
	ShipyardEditMode controlMode;

	size_t pickedSection;

	ShipyardWindow(Game * core);

	GUI::FontPtr font;
	Game* game;
	GUI::ButtonPtr menu, save;
	std::shared_ptr<GUI::Frame> toolbox;
	GUI::ButtonPtr tiles, objects, designTest, designSave;
	std::shared_ptr<ShipyardArea> shipyardArea;
	std::shared_ptr<GUI::Slider> toolboxSlider;
	
	std::list<GUI::ObjectPtr> contents;
//	std::list<GUI::Button*> tileTypes;
//	std::list<GUI::Button*> objectTypes;
// shipyard specific
	void showTiles();
	void showObjects();
	void setControlMode(ShipyardEditMode mode);
	void selectTile(size_t tileId);
	void clearContents();
	void addListboxItem(GUI::ObjectPtr object);

	void frameBack();

	void setBlueprint( ShipBlueprint * blueprint, bool locked );
	void getBlueprint( ShipBlueprint * blueprint );

	Fx::EntityPtr getSectionSprite(size_t sectionId);
};
