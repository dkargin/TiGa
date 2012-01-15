debugTrace('server script{')
dofile('./script/level.lua')

-- take control over some unit
world[NetCmd.cmdTakeControl]=function(self,client,cmd)
	debugTrace('server executing cmdTakeControl')	
	local unit=toUnit(self.gameObjects:getObject(cmd.unit))
	if unit~=nil then
		self.gameObjects:controlObj(cmd.unit,true)
		takeControl(unit) --turn it on
	else
		debugTrace('-wrong id')
	end			
end

dofile('./script/levelRules.lua')
debugTrace('}server script')