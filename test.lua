xcb = require("xcb")
require("utils")

print(dump(xcb))

local root
-- Open the connection to the X server
local dpy, screen = xcb.connect("")
if (not dpy) then
    print("Error: Cannot open default display.")
    return 1;
end

-- get setup
setup = dpy:get_setup()
print('setup', dump(setup))
print('setup.vendor', dump(setup:vendor()))

-- Get the first screen
--screen = dpy:get_setup().roots[screen+1]

-- Ask for our window's Id
win = dpy:generate_id()
print('win id', dump(win))

--[[
-- Create the window
dpy:create_window(
	0, -- TODO: make XCB_COPY_FROM_PARENT contant
	win,
	screen.root,
	0, 0,
	150, 150,
	10,
	INPUT_OUTPUT,
	screen.root_visual,
	0, {}
	)

-- Map the window to the screen
dpy:map_window(win)
]]

-- Make sure commands are sent before we pause, so window is shown
print('flush:',dpy:flush())

-- wait
--io.read()

dpy:disconnect()
print('dump dpy', dump(dpy))
dpy = nil

return 0
