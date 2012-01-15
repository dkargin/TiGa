#ifndef _SHIPYARD_H_
#define _SHIPYARD_H_

class ShipyardWindow;

struct SectionDesc
{
	FxSpritePtr sprite;
	int sizeX;
	int sizeY;
	int type;
};

class ShipyardArea : public GUI::Object
{
public:
	ShipyardArea(ShipyardWindow * shipyard);
	~ShipyardArea();

	struct Tile
	{
		int x; 
		int y;
		FxSpritePtr sprite;
		size_t links[4];
	};

	struct Link
	{
		size_t tiles[2];
	};

	vec2i selectedCell;

	std::vector<Tile> tiles;
	std::vector<Link> links;

	float tileSize;
	float tileOffsetX, tileOffsetY;
	WeakPtr<ShipyardWindow> shipyard;

	size_t findTile( int x, int y );
	void addTile( int x, int y, FxSpritePtr sprite );	
	vec2i screenToLocal( const uiVec & vec );
	// UI handlers
	virtual void onRender();
	virtual bool onMouse(int key, int state, const uiVec & vec);
	virtual bool onMouseMove( const uiVec & vec );

	vec2i screenCellCenter( const vec2i & cell ) const;
	hgeRect screenCellRect( const vec2i & cell ) const;
};


class ShipyardWindow : public GUI::Object
{
public:
	enum Mode
	{
		Normal,
		CreateTile,
		DragTile,
		DragDevice,
	}controlMode;

	size_t pickedSection;

	ShipyardWindow(Game * core);
	hgeFont * font;
	Game * game;
	Instance<GUI::Button> menu;
	Instance<GUI::Frame> toolbox;	
	Instance<GUI::Button> tiles, objects, designTest, designSave;
	Instance<ShipyardArea> shipyardArea;
	Instance <GUI::Slider> toolboxSlider;
	std::vector<SectionDesc> sections;
	std::list<SharedPtr<GUI::Object> > contents;
//	std::list<GUI::Button*> tileTypes;
//	std::list<GUI::Button*> objectTypes;
// shipyard specific
	void showTiles();
	void showObjects();
	void setControlMode( Mode mode );
	void selectTile(size_t tileId);
	void clearContents();
	void addItem( GUI::Object * object );
};
#endif