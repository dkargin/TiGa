exec('./script/levelEditor.lua')
exec('./script/editor.lua')

function StartMainMenu()
	if wndMainMenu == nil then	
		function StartScene(scene)

			function WorldRun()
				onControl = ControlModes.Game				
				core:worldPause(false)
			end

			function WorldPause()
				onControl = ControlModes.EditScene				
				core:worldPause(true)				
			end

			function WorldReset()
				testScenes:load(activeScene)
				WorldRun()
			end

			wndMainMenu:destroy()
			if worldStarted == false then
				core:startHGE()
				StartWorld()
				cursor_object = Models.cursor_red
				cursor_empty = Models.cursor_green				
			end
			print("StartScene: loading level")
			testScenes:load(scene)
			WorldRun()
			world:setCursor( cursor_empty )
			print("StartScene: loading level - finished")
			--iup.Destroy(wndMainMenu)
			core:runHGE()
			iup.ExitLoop()
		end
		local buttonSize = "100x17"
		-- enum all levels
		function GenerateLevelChoise()
			local levelButtons = {}
			for _, scene in ipairs(testScenes) do
				local item = iup.button
				{
					size = buttonSize,
					title = scene.name,
					action = function(self)	
						StartScene(scene)						
						return iup.CLOSE
					end,
				}
				table.insert(levelButtons, item)
			end
			return levelButtons
		end	
		wndMainMenu = iup.dialog
		{
			title = "Main Menu",
			resize = "NO",
			MINBOX = "NO",
			iup.vbox
			{
				iup.frame
				{
					title = "Scenes",					
					iup.vbox(GenerateLevelChoise()),
				},
				iup.frame
				{
					title = "Editors",
					iup.vbox
					{
						iup.button
						{
							size = buttonSize,
							title = "Level Editor",
							action = function(self)
								wndMainMenu:destroy()								
								StartLevelEditor()
								return iup.DEFAULT														
							end,
						},
						iup.button
						{
							size = buttonSize,
							title = "Pack Editor",
							action = function(self)								
								wndMainMenu:destroy()
								return iup.CLOSE
							end,
						},
					},
				},
			},
		}
	end
	wndMainMenu:show()
end