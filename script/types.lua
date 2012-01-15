--------------------------------------------------------------
-- таблица типов, описывает как каждый тип редактируется с помощью iup
MakeType("userdata")
{
	createControl = function (self,owner, ownerKey)
		return BasicControl(owner,ownerKey)
		{
			control = iup.text
			{
				value = "userdata",
				expand = "HORIZONTAL",
			}
		}
	end,
	-- do nothing
	default = function(self, owner, ownerKey)
	end,
}

MakeType("table")
{
	createControl = function (self, owner, ownerKey)
		--return iup.text{value = value,expand="HORIZONTAL"}
	end,
	default = function(self, owner, ownerKey)
		owner[ownerKey] = nil
	end,
}

MakeType("FileName")
{
	createControl = function (self, owner, ownerKey)
		self:check(owner, ownerKey)
		local result = BasicControl(owner,ownerKey)
		{
			updateDown = function(self)
				print("FileName:updateDown()")
				self.control[1].value = self:getValue()
			end,
		}
		result.control = iup.hbox
		{
			iup.text
			{
				expand = "HORIZONTAL",
				action = function(self, c, newvalue)
					print("FileName.text:action()")
					self.value = newvalue
					return iup.DEFAULT
				end,
				k_any = function(self, c)
					if c == iup.K_CR then
						result:updateUp()
					end
					return iup.DEFAULT
				end,
				killfocus_cb = function(self)
					result:updateUp()
					return iup.DEFAULT
				end
			},
			iup.button
			{
				--title = "select",	
				image = "IUP_FileOpen",
				action = function(self)
					local file, status = DlgOpenFile("Open Image", ".", "Image|*.png;*.bmp;*.jpg;*.pcx;*.tga")
					--local file, status = iup.GetFile(".\*.*")
					if status ~= -1 then
						print("FileName "..file .. " was selected")
						result.owner[result.ownerKey] =  toAppRelative(file)
						-- update textbox contents
						result:updateDown()
						-- send update through hierarchy
						result:updateUp()
					else
						print("FileName: choice canceled")
					end
				end,
			},
		}
		return result
	end,
	save = function( self, value )
		return "[[" .. value .. "]]"
	end,
	default = function (self, owner, ownerKey)
		owner[ownerKey] = ""
	end,
}

MakeType("number")
{
	-- create wrapped textbox, limited to number imput
	createControl = function (self, owner, ownerKey)
		self:check(owner, ownerKey)
		local result = BasicControl(owner,ownerKey)
		{			
			updateDown = function(self)
				self.control.value = self:getValue()
			end,
		}
		result.control = iup.text
		{
			expand="HORIZONTAL",
			multiline = "false",
			filter = "number",
			action = function(self,c,newvalue)
				print("assign[" .. ownerKey .. "..]="..newvalue)
				owner[ownerKey] = tonumber(newvalue)
			end,
			k_any = function(self, c)
				if c == iup.K_CR then
					result:updateUp()
				end
				return iup.DEFAULT
			end,
			killfocus_cb = function(self)
				result:updateUp()
				return iup.DEFAULT
			end,
		}
		return result
	end,
	save = function( self, value )
		return value
	end,
	default = function(self, owner, ownerKey)
		owner[ownerKey] = 0
	end,
}

MakeType("boolean")
{
	-- create wrapped checkbox
	createControl = function (self, owner, ownerKey)
		self:check(owner, ownerKey)
		local result = BasicControl(owner,ownerKey)
		{ 
			updateDown = function(self)
				self.control.value = bool2iup(self:getValue())
			end,
		}
		result.control = iup.toggle
		{
			flat = "yes", 
			expand="HORIZONTAL",
			action = function( self, value )
				local newvalue = iup2bool(value)
				if owner[ownerKey] ~= newvalue then
					owner[ownerKey] = newvalue
					result:updateUp()
				end				
				return iup.DEFAULT
			end,
		}
		return result
	end,
	save = function( self, value )
		if value then
			return "true"
		else
			return "false"
		end
	end,
	default = function (self, owner, ownerKey)
		owner[ownerKey] = false
	end,
}

MakeType("string")
{
	createControl = function (self, owner, ownerKey)
		self:check(owner, ownerKey)
		
		local result = BasicControl(owner, ownerKey)
		{
			updateDown = function(self)			
				self.control.value = self:getValue()
			end,			
		}
		result.control = iup.text
		{
			expand = "HORIZONTAL",
			action = function(self, c, newvalue)
				self.value = newvalue
				--result:updateUp()
				return iup.DEFAULT
			end,
			k_any = function(self, c)
				if c == iup.K_CR then
					result:updateUp()
				end
				return iup.DEFAULT
			end,
			killfocus_cb = function(self)
				result:updateUp()
				return iup.DEFAULT
			end,
		}
		return result
	end,
	save = function( self, value )
		return "\'" .. value .. "\'"
	end,
	default = function (self, owner, ownerKey)
		owner[ownerKey] = ""
	end,
}

MakeType("unknown")
{
	createControl = function (self, owner, ownerKey)
		return BasicControl(owner, ownerKey)
		{
			control = iup.text{readonly = "yes",value = "unknown",expand="HORIZONTAL"},
		}
	end,
}

-- map native types
MakeTypeAlias("boolean","bool")
MakeTypeAlias("number","float")
MakeTypeAlias("number","int")
MakeTypeAlias("number","unsigned int")

MakeType("vec2f")
{	
	createControl = function (self, owner, ownerKey)
		self:check(owner, ownerKey)		
		local result = BasicControl(owner, ownerKey)
		{				
			updateDown = function(self)
				local value = self:getValue()
				self.control[1].value = value.x
				self.control[2].value = value.y
			end,
		}
		result.control = iup.hbox
		{
			iup.text
			{
				expand = "HORIZONTAL",
				multiline = "false",
				filter = "number",
				action = function(self,c,newvalue)
					result.getValue().x = tonumber(newvalue)
					return iup.DEFAULT
				end,
				k_any = function(self, c)
					if c == iup.K_CR then
						result:updateUp()
					end
					return iup.DEFAULT
				end,
				killfocus_cb = function(self)
					result:updateUp()
					return iup.DEFAULT
				end,
			},
			iup.text
			{
				expand = "HORIZONTAL",
				multiline = "false",
				filter = "number",
				action = function(self,c,newvalue)
					result.getValue().y = tonumber(newvalue)
					return iup.DEFAULT
				end,
				k_any = function(self, c)
					if c == iup.K_CR then
						result:updateUp()
					end
					return iup.DEFAULT
				end,
				killfocus_cb = function(self)
					result:updateUp()
					return iup.DEFAULT
				end,
			},
		}
		return result
	end,
	save = function( self, value )
		return "TiGa.vec2f(" .. value.x .. "," .. value.y .. ")"
	end,
	default = function( self, owner, ownerKey )
		owner[ownerKey] = TiGa.vec2f(0,0)
	end,
}

MakeType("Pose")
{
	createControl = function (self, owner, ownerKey)
		self:check(owner, ownerKey)
		local result = BasicControl(owner, ownerKey)
		{			
			updateDown = function(self)
				local value = self:getValue()
				self.control[1].value = value.x
				self.control[2].value = value.y
				self.control[3].value = value.orientation
				self.control[4].value = value.z
			end,
		}
		result.control = iup.hbox
		{
			iup.text
			{
				expand = "HORIZONTAL",
				multiline = "false",
				filter = "number",
				action = function(self,c,newvalue)
					result.getValue().x = tonumber(newvalue)
				end,
				k_any = function(self, c)
					if c == iup.K_CR then
						result:updateUp()
					end
					return iup.DEFAULT
				end,
				killfocus_cb = function(self)
					result:updateUp()
					return iup.DEFAULT
				end,
			},
			iup.text
			{
				expand = "HORIZONTAL",
				multiline = "false",
				filter = "number",
				action = function(self,c,newvalue)
					result.getValue().y = tonumber(newvalue)
					return iup.DEFAULT
				end,
				k_any = function(self, c)
					if c == iup.K_CR then
						result:updateUp()
					end
					return iup.DEFAULT
				end,
				killfocus_cb = function(self)
					result:updateUp()
					return iup.DEFAULT
				end,
			},
			iup.text
			{
				expand="HORIZONTAL",
				multiline = "false",
				filter = "number",
				action = function(self,c,newvalue)
					result.getValue().orientation = tonumber(newvalue)
					return iup.DEFAULT
				end,
				k_any = function(self, c)
					if c == iup.K_CR then
						result:updateUp()
					end
					return iup.DEFAULT
				end,
				killfocus_cb = function(self)
					result:updateUp()
					return iup.DEFAULT
				end,
			},
			iup.text
			{
				expand="HORIZONTAL",
				multiline = "false",
				filter = "number",
				action = function(self,c,newvalue)
					result.getValue().z = tonumber(newvalue)
					return iup.DEFAULT
				end,
				k_any = function(self, c)
					if c == iup.K_CR then
						result:updateUp()
					end
					return iup.DEFAULT
				end,
				killfocus_cb = function(self)
					result:updateUp()
					return iup.DEFAULT
				end,
			},
		}
		return result
	end,
	save = function( self, value )
		return "TiGa.Pose(" .. value.x .. "," .. value.y .. "," .. value.orientation .. "," .. value.z .. ")"
	end,
	default = function( self, owner, ownerKey)
		owner[ownerKey].x = 0
		owner[ownerKey].y = 0
		owner[ownerKey].orientation = 0
		owner[ownerKey].z = 0
	end
}