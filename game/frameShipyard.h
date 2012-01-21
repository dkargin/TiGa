#ifndef _SHIPYARD_H_
#define _SHIPYARD_H_

class ShipyardWindow;

const int ShipyardCellsMax = 6;

class ShipyardArea : public GUI::Object
{
public:
	ShipyardArea(ShipyardWindow * shipyard);
	~ShipyardArea();

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
	WeakPtr<ShipyardWindow> shipyard;
	
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
	hgeRect screenCellRect( const vec2i & cell ) const;

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
	GUI::hgeFontPtr font;
	Game * game;
	Instance<GUI::Button> menu, save;
	Instance<GUI::Frame> toolbox;	
	Instance<GUI::Button> tiles, objects, designTest, designSave;
	Instance<ShipyardArea> shipyardArea;
	Instance <GUI::Slider> toolboxSlider;

	ShipBlueprint * editedBlueprint;		/// blueprint selected to edit
	
	std::list<SharedPtr<GUI::Object> > contents;
//	std::list<GUI::Button*> tileTypes;
//	std::list<GUI::Button*> objectTypes;
// shipyard specific
	void showTiles();
	void showObjects();
	void setControlMode( ShipyardEditMode mode );
	void selectTile(size_t tileId);
	void clearContents();
	void addListboxItem( GUI::Object * object );

	void frameBack();

	void setBlueprint( ShipBlueprint * blueprint, bool locked );
	void getBlueprint( ShipBlueprint * blueprint );

	FxEffect::Pointer getSectionSprite(size_t sectionId);	
};
#endif