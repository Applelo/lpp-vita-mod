white = Color.new(255, 255, 255)
yellow = Color.new(255, 255, 0)
red = Color.new(255, 0, 0)
scripts = System.listDirectory("cache0:/lpp")
i = 1
while true do
	y = 30
	Screen.initBlend()
	Screen.clear()
	Screen.debugPrint(5, 5,"Lua Player Plus MOD - Select script or a folder to start", yellow)
	for j, file in pairs(scripts) do
		x = 5
		if i == j then
			color = red
			x = 10
		else
			color = white
		end
		Screen.debugPrint(x, y, file.name, color)
		y = y + 20
	end
	Screen.termBlend()
	pad = Controls.read()
	if Controls.check(pad, SCE_CTRL_CROSS) then
		Screen.initBlend()
		Screen.clear()
		Screen.termBlend()
		Screen.flip()
		Screen.initBlend()
		Screen.clear()
		Screen.termBlend()
		System.wait(800000)
		if scripts[i].directory and System.doesFileExist("cache0:/lpp/"..scripts[i].name.."/script.lua") then
			dofile("cache0:/lpp/"..scripts[i].name.."/script.lua")
		elseif scripts[i].directory==false then
			dofile("cache0:/lpp/"..scripts[i].name)
		end
	elseif Controls.check(pad, SCE_CTRL_UP) and not Controls.check(oldpad, SCE_CTRL_UP) then
		i = i - 1
	elseif Controls.check(pad, SCE_CTRL_DOWN) and not Controls.check(oldpad, SCE_CTRL_DOWN) then
		i = i + 1
	elseif Controls.check(pad, SCE_CTRL_START) and not Controls.check(oldpad, SCE_CTRL_START) then
		System.exit(1)
	end
	if i > #scripts then
		i = 1
	elseif i < 1 then
		i = #scripts
	end
	oldpad = pad
	Screen.flip()
end