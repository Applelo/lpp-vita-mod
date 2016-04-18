white = Color.new(255,255,255,255) -- Create new color
lang = System.getLanguage()--lang
max, free = System.getSize()-- Max and Free Size
mac = System.getMacAddress()--Mac address
ip = System.getIpAddress()--ip Address

while true do
	thetime = System.getTime()
	-- Draw string on the screen
	Screen.initBlend()
	Screen.clear()
	Screen.debugPrint(5, 5,"Language: "..lang.."\nMemory: "..max-free.."GB / "..max.."GB".."\nBattery Percentage: "..System.getBatteryPercentage().."%\nBattery Lifetime: "..System.getBatteryLifetime().."\nBattery Charging: "..tostring(System.isBatteryCharging()).."\nMac Address: "..mac.."\nIp Address: "..ip, white)
	Screen.debugPrint(580, 5,thetime.hour..":"..thetime.minutes..":"..thetime.seconds.."\n"..thetime.day.."/"..thetime.month.."/"..thetime.year.."\n\n"..thetime.microseconds, white)
	Screen.termBlend()
	
	-- Update screens (For double buffering)
	Screen.flip()
	
	-- Check controls for exiting
	if Controls.check(Controls.read(), SCE_CTRL_START) then
		System.exit()
	end
end