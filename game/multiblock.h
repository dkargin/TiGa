#ifndef _MULTIBLOCK_H_
#define _MULTIBLOCK_H_
#include "../sim/gameObject.h"

enum AdjacentDirection	{	DirLeft = 0, DirRight = 1, DirTop = 2, DirBottom = 3, DirSame, DirInvalid	};

inline int adjacentX( AdjacentDirection dir, int x = 0 )
{
	switch( dir )
	{
	case DirLeft: return x - 1;
	case DirRight: return x + 1;					
	};
	return x;
}

inline int adjacentY( AdjacentDirection dir, int y = 0 )
{
	switch( dir )
	{
	case DirTop: return y - 1;
	case DirBottom: return y + 1;					
	};
	return y;
}

inline AdjacentDirection getOppositeDirection( AdjacentDirection dir )
{
	AdjacentDirection results [] = { DirRight, DirLeft, DirBottom, DirTop, DirInvalid, DirInvalid};
	return results[dir];
}

inline AdjacentDirection getDirection( int x1, int y1, int x2, int y2)
{	
	AdjacentDirection table[3][3] = 
	{
		{DirInvalid, DirTop, DirInvalid},
		{DirLeft, DirSame, DirRight},
		{DirInvalid, DirBottom, DirInvalid}
	};

	int x = x2 - x1;
	int y = y2 - y1;	

	if( abs(x) > 1 || abs(y) > 1)
		return DirInvalid;
	return table[ y + 1][ x + 1 ];
}

template< class Type, int N > class Array
{
public:
	Type data[N];
	Type & operator[]( size_t index )
	{
		assert( index < N );
		return data[index];
	}
	const Type & operator[]( size_t index ) const
	{
		assert( index < N );
		return data[index];
	}
};
template< class CellData, class LinkData >
class LinkedGrid
{
public:
	typedef LinkedGrid<CellData, LinkData> grid_type;
	enum { MaxSize = 5 };
	// links = 2 * width * height - width - height
	enum 
	{ 
		MaxCells = MaxSize * MaxSize, 
		MaxLinks = 2 * MaxSize * MaxSize - 2 * MaxSize 
	};
	enum State
	{
		Free,
		Occupied,
		Destroying,		// cell is being removed
		Moving,			// cell is being moved to other object
	};

	struct Cell
	{
		CellData data;
		State state;	
		int x,y;
		bool isFree() const { return state == Free;}
	};

	struct Link 
	{
		State state;
		LinkData data;
		bool isFree() const { return state == Free;}
	};

	struct CellRef
	{
		Cell * cell;
		int x, y;		// coordinates
	};

	struct LinkRef
	{
		Link * link;
		int x, y;
		AdjacentDirection dir;
	};

	int cellsCount, linksCount;
	int centerX, centerY;
public:
	LinkedGrid(): cellsCount(0), linksCount(0), centerX(-1), centerY(-1)
	{
		Clean();
	}
	CellRef getAdjacentCell( int x, int y, AdjacentDirection dir) const
	{
		CellRef result = { NULL, adjacentX(dir, x), adjacentY(dir, y) };
		result.cell = getCell( result.x, result.y );
		return result;
	}
	Link * getLink( int x, int y,  AdjacentDirection dir) const
	{		
		const int width = MaxSize;
		const int rowWidth = width + width - 1;
		if( dir == DirLeft )
		{
			x -= 1;
			dir = DirRight;
		}
		else if( dir == DirTop )
		{
			y -= 1;
			dir = DirBottom;
		}
		if( x < 0 || y < 0 )
			return NULL;
		
		if( dir == DirRight )// 1st row			
		{
			return (x < MaxSize - 1 && y < MaxSize) ? (Link*)&links[x + y * rowWidth] : NULL;
		}
		if( dir == DirBottom )// 2nd row
		{
			return (x < MaxSize && y < MaxSize - 1) ? (Link*)&links[x + y * rowWidth + rowWidth / 2] : NULL;
		}
		return NULL;
	}
	LinkRef getLinkRef( const Link * link ) const
	{		
		return getLinkRef( link - &links[0] );
	}
	LinkRef getLinkRef( size_t linkIndex ) const
	{
		LinkRef result = {NULL, 0, 0, DirInvalid};
		if( linkIndex >= 0 && linkIndex < MaxLinks )
		{
			result.link = (Link*)&links[linkIndex];
			const int width = MaxSize;
			const int doubleRow = width + width - 1;
			int rowPos = linkIndex % doubleRow;
			result.y = linkIndex / doubleRow;
			if( rowPos >= doubleRow / 2 )
			{
				result.x = rowPos - (width - 1);				
				result.dir = DirBottom;
			}
			else
			{
				result.x = rowPos;
				result.dir = DirRight;
			}
		}
		return result;
	}
	
	bool createLink( int x, int y, AdjacentDirection dir)
	{		
		Link * link = getLink(x,y, dir);
		// check if link already exists
		if( link == NULL )
			return false;
		if( link->state == Occupied )
			return false;
		// check if both cells are valid
		Cell * cell1 = getAdjacentCell( x, y, dir ).cell;
		Cell * cell2 = getCell( x, y );
		if( cell1 == NULL || cell2 == NULL || cell1->state != Occupied || cell2->state != Occupied )
			return false;		
		link->state = Occupied;
		// call event
		onLinkCreated(link, x, y, dir);	
		LinkRef ref = getLinkRef(link);
		return true;
	}

	Cell * createCell( int x, int y, const CellData &data, bool move = false)
	{
		Cell * cell = getCell(x,y);
		if( cell && cell->isFree() ) 
		{
			// assign new center position
			if( centerX == -1 || centerY == -1)
			{
				centerX = x;
				centerY = y;
			}
			CellRef ref;
			ref.cell = cell;
			ref.x = x;
			ref.y = y;
			// 1. create cell
			onCellCreated( ref, data );	
			cell->state = Occupied;
			Cell * adj = NULL;
			// 2. create links with adjacent cells
			if( !move )	// do not create links now, while we are moving
				for( size_t i = 0; i < 4; ++i)
					createLink(x,y, (AdjacentDirection)i);	
			cellsCount++;
		}
		return cell;
	}

	bool destroyCell( int x, int y )
	{
		Cell * cell = getCell(x,y);
		if( cell == NULL || cell->state != Occupied)
			return false;
		CellRef ref = {cell, x, y};	
		cell->state = Destroying;	
		cellsCount--;
		// 1. find new center
		bool findCenter = false;
		if( x == centerX && y == centerY )
		{
			findCenter = true;
			centerX = -1;
			centerY = -1;
		}
		onCellDestroyed( ref );
		// 2. remove links with adjacent cells
		LinkRef linkRef = {NULL, x, y, DirInvalid};			
		for( size_t i = 0; i < 4; ++i)
		{
			linkRef.dir = (AdjacentDirection)i;
			linkRef.link = getLink( x, y, linkRef.dir );

			if( findCenter )
			{
				if( linkRef.link && linkRef.link->state == Occupied )
				{
					centerX = adjacentX( linkRef.dir, x );
					centerY = adjacentY( linkRef.dir, y );
					findCenter = false;
				}
			}
			destroyLink(linkRef);
		}

		cell->state = Free;
		return true;
	}

	void destroyLink( LinkRef & ref)
	{
		Link * link = ref.link;
		int cx = ref.x;
		int cy = ref.y;
		AdjacentDirection dir = ref.dir;
		
		if( link == NULL || link->state != Occupied )
			return;
		link->state = Destroying;
		
		onLinkDestroyed( link, cx, cy, dir );
		int marks[MaxCells];
		for( int i = 0; i < MaxCells; i++)
			marks[i] = 0;
		int selected = waveMark( centerX, centerY, 1, marks);
		// split		
		if( selected != cellsCount )
		{
			// 1. Calculate mass center
			int averageX = 0, averageY = 0;
			int count = 0;
			for( int y = 0; y < MaxSize; y++ )
				for(int x = 0; x < MaxSize; x++ )
				{
					size_t index = x + y * MaxSize;
					if( marks[ index ] != 0 || getCell(x,y)->state != Occupied )
						continue;					
					averageX += x;
					averageY += y;
					count++;
					// mark cells as 'moving'
					cells[ index ].state = Moving;
					for( size_t i = 0; i < 4; ++i)
					{
						Link * link = getLink( x, y, (AdjacentDirection)i);
						if( link && link->state == Occupied )
							link->state = Moving;
					}
				}
			if( count > 0 )
			{
				averageX /= count;
				averageY /= count;

				int w = 0;
				// 2. Create new body
				grid_type * fractured = fracture( averageX - MaxSize/2, averageY - MaxSize/2 );
				if( fractured != NULL )
				{
					// 3. mark cells and links as 'moving'
					const int NewSize = MaxSize;
					const int offsetX = (NewSize) / 2;
					const int offsetY = (NewSize) / 2;
					size_t i = 0;
					for( int y = 0; y < MaxSize; y++ )
						for(int x = 0; x < MaxSize; x++, i++)
						{
							Cell & cell = cells[i];			
							if( cell.state == Moving )
							{
								fractured->createCell( offsetX + (x - averageX), offsetY + (y - averageY), cell.data, true);
								CellRef cellRef = {&cell, x, y};
								onCellDestroyed(cellRef, true);
								cell.state = Free;
							}
						}

					for( i = 0; i < MaxLinks; i++ )
					{
						Link & link = links[i];
						if( link.state == Moving )
						{
							LinkRef & linkRef = getLinkRef(i);
							fractured->createLink( offsetX + (linkRef.x - averageX),  offsetY + (linkRef.y - averageY), linkRef.dir);
							onLinkDestroyed( linkRef.link, linkRef.x, linkRef.y, linkRef.dir, true);
							link.state = Free;
						}					
					}
				}
			}
		}
		link->state = Free;
	}
	
	int waveMark( int x, int y, int mark, int marks[])
	{
		int & m = marks[ x + y * MaxSize ];
		int marked = 0;
		if( m != mark)
		{			
			m = mark;
			marked = cells[ x + y * MaxSize].state == Occupied ? 1 : 0;
			for( size_t i = 0; i < 4; ++i)
			{				
				const AdjacentDirection dir = (AdjacentDirection)i;			
				const Link * link = getLink(x,y, dir);
				if( link && link->state == Occupied )
					marked += waveMark( adjacentX(dir, x) , adjacentY(dir, y), mark, marks );
			}
		}
		return marked;
	}

	Cell * getCell( int x, int y ) const
	{
		if( x < 0 || y < 0 || x >= MaxSize || y >= MaxSize)
			return NULL;
		return (Cell*)&cells[x + y * MaxSize];
	}
	// called when cell is created
	virtual void onCellCreated(CellRef & ref, const CellData & data) {}	
	virtual void onLinkCreated(Link * link, int x, int y, AdjacentDirection dir) {}
	// called when cell is completely
	virtual void onCellDestroyed(CellRef & ref, bool move = false) {}
	// called when link between cell pair is destroyed
	virtual void onLinkDestroyed(Link * link, int x, int y, AdjacentDirection dir, bool move = false)	{}
	// create 'fractured' object, based on cell (x,y) from original object. This coorninates are necesarry to
	// calculate new pose for derived grid_type class
	virtual grid_type * fracture(int x, int y) = 0;
public:
	void Clean()
	{
		for( size_t i = 0; i < MaxCells; ++i)
		{
			cells[i].state = Free;
			cells[i].x = i % MaxSize;
			cells[i].y = i / MaxSize;
		}
		for( size_t i = 0; i < MaxLinks; ++i)
			links[i].state = Free;
		cellsCount = 0;
		linksCount = 0;
		centerX = -1;
		centerY = -1;
	}
	Array<Cell, MaxCells> cells;
	Array<Link, MaxLinks> links;
	//Cell cells[MaxCells];
	//Link links[MaxLinks];
};

struct CellDesc 
{
	b2Fixture * fixture;
	int health;
	vec2f totalImpulse;
};

struct LinkDesc 
{
	int health;
	float stress[2];	// { normal, tangent }
};

class ObjectManager;

class Multiblock : public LinkedGrid<CellDesc, LinkDesc>, public GameObject
{	
public:	
	float cellWidth;	
	Cell * selection;
	//b2Body * body;
	//Application * application;
public:
	Multiblock(ObjectManager * manager);
	~Multiblock();
	
	void init(const Pose2 & pose, b2World & world);

	Cell * rayHit( );
	CellRef pick( const vec2f & worldPos ) const;
	
	void hit( Multiblock::Cell * cell, const vec2f & pos, const vec2f & dir, float &impulse );
	void clearStress();
	virtual grid_type * fracture(int x, int y);	
	virtual Multiblock * createFractureBody();	
	/// LinkedGrid events
	virtual void onCellCreated(CellRef & ref, const CellDesc & source);
	virtual void onLinkCreated(Link * link, int x, int y, AdjacentDirection dir, bool move = false);
	virtual void onCellDestroyed(CellRef & ref, bool move = false);	
	/// for debug purposes
	void markSelected( Cell * cell );
};

#endif