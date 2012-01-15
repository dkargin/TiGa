-- first init level rules

-- spawning position
spawnPos=
{
	[0]={pos={-200,-50,5},dir={0,1,0}},
	[1]={pos={ 200,-50,5},dir={0,1,0}},
}

-- dunno who uses it
function cmdImpl(command)
	if command:type()==NetCmd.cmdHello then return NetCmd.toHello(command) end
	if command:type()==NetCmd.cmdTakeControl then return NetCmd.toTakeControl(command) end
	return nil
end
Net={}
-- also something i don't remember
Net.exec=function(self,client,command)
	debugTrace('executing world.exec')
	local type=command:type()
	local handler=self[type]
	if handler==nil then
		debugTrace('-no cmd handler')
	else
		handler(self,client,cmdImpl(command))
	end
end
--[[
-- reply to new connection
--	spawn unit for new client
Net[TiGa.NetCmd.cmdHello]=function(self,client,cmd)
	debugTrace('server executing cmdHello')
	unit=createUnit('basic',spawnPos[client].pos,spawnPos[client].dir,client)
	local newMsg=NetCmdTakeControl:new(unit)
	world:sendCmd(client,newMsg)
end
]]--
