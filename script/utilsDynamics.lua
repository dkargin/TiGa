-- utils.lua
-- Core script
-- File contains Box2D tools

--
function BodyDesc(source)
	local default = { frozen = false }
	local result = TiGa.BodyDesc(world.gameObjects)
	local data = mergeTables(source,default)
	for key, creator in ipairs(source) do
		creator(result)
	end
	SWIG_assign(result,data)
	return result
end

function toPoint(source)
	if source.x ~= nil and source.y~= nil then
		return {source.x, source.y}
	end
	if source[1] ~= nil and source[2] ~= nil then
		return { source[1] , source[2] }
	end
	error("toPoint - incorrect argument")
end
-- makes creator for box
function solidBox(sizeX,sizeY,mass)
	return
		function(bodyDesc)
			local object = bodyDesc:addBox(sizeX,sizeY,mass)
		end
end
-- makes creator for polygon
function solidPoly(source)
	return
		function(bodyDesc)
			local poly = TiGa.PolyDesc()
			for key,point in ipairs(source) do
				local pt = toPoint(point)
				poly:addPoint(pt[1],pt[2])
			end
			local object = bodyDesc:addPolygon(poly,source.density)
		end
end
--
function solidSphere(D,mass)
	return
		function(bodyDesc)
			local object = bodyDesc:addSphere(D,mass)
		end
end
--[[

FxAnimation=SomeType
{
	file=fileName(),
	size=typeSize(),	// таблица {width,height}
	animMode=enum(Hold,Once,Loop,Bounce),
	blend=enum(colorAdd,colorMul,alphaAdd,alphaMul,zWrite,nozWrite),
}

]]--
