white = Color.new(255,255,255)
green = Color.new(0,255,0)
red = Color.new(255,0,0)
blue = Color.new(0,0,255)
yellow = Color.new(0,255,255)
while true do
	Screen.initBlend()
	Screen.clear()
	x,y = Controls.readTouch(SCE_TOUCH_PORT_FRONT)
	x2,y2 = Controls.readTouch(SCE_TOUCH_PORT_FRONT,2)

	Screen.fillCircle(x,y,50,green)
	Screen.fillRect(x2,y2,50,50,blue)
	
	Screen.debugPrint(10,30,"X: "..x.."\nY: "..y,red)
	
	Screen.termBlend()
	if Controls.check(Controls.read(), SCE_CTRL_START) then
		System.exit()
	elseif Controls.check(Controls.read(), SCE_CTRL_CIRCLE) then
		forme = "rond"
	end
	Screen.flip()
end