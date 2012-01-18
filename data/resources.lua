function core_setRootPath(path)
end
function core_createSprite(path, x, y, width, height)
end
function core_createSound(path)
end
function core_createBitmap(path)
	print("Bitmap from " .. path)
end
MetaSprite =
{
	_type = "sprite",
	generate = function( self, source, name )
		source.x = source.x or 0
		source.y = source.y or 0
		source.width = source.width or 0
		source.height = source.height or 0
		source.file = source.file or ""
		core_createSprite( name, source.file, source.x, source.y, source.width, source.height  )
	end,
}

MetaSound =
{
	_type = "sound",
	generate = function( self, source, name )
		source.file = source.file or ""
		core_createSound( name, source.file )
	end,
}

MetaBitmap =
{
	_type = "bitmap",
	generate = function( self, source, name )
		source.file = source.file or ""
		core_createBitmap( name, source.file )
	end,
}

function fxSprite( source )
	setmetatable(source, MetaSprite)
	return source
end

function fxSound( source )
	setmetatable(source, MetaSound)
	return source
end

function fxBitmap( source )
	setmetatable(source, MetaBitmap)
	return source
end

function BuildObjects(sourceTable)
	sourceTable = sourceTable or Models
	for resName, resource in pairs(sourceTable) do
		local typedef = getmetatable(resource)
		if typedef ~= nil and type(typedef.generate) == "function" and resource ~= nil then
			typedef:generate(resource, resName)
		end
	end
end

core_setRootPath([[c:\dev\projects\tiga2d\data]])

Models =
{
	new_bitmap = fxBitmap
	{
		file = [[effects\beams_01.png]]
	},
	new_bitmap1 = fxBitmap
	{
		file = [[effects\beams_02.png]]
	},
	new_bitmap2 = fxBitmap
	{
		file = [[effects\beams_03.png]]
	},
}

BuildObjects()
