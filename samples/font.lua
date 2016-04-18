white = Color.new(255,255,255)
opensans = Font.load("cache0:/lpp/samples/OpenSans-Light.ttf")
Font.setPixelSizes(opensans,20)
while true do
	pad = Controls.read()
	Screen.initBlend()
	Screen.clear()
	if (Controls.check(pad,SCE_CTRL_START)) then
		System.exit()
	end
	Font.print(opensans,100,10,"Hello World with OpenSans-Light 20px",white)
	
	Screen.termBlend()
	Screen.flip()
end