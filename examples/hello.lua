require("lxcb")

local root
-- Open the connection to the X server
local dpy, screen = lxcb.connect("")
if (not dpy) then
    print("Error: Cannot open default display.")
    return 1;
end

-- Get the first screen
screen = dpy:get_setup().roots[screen+1]

-- Ask for our window's Id
win = dpy:generate_id()

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

-- Make sure commands are sent before we pause, so window is shown
dpy:flush()

-- wait
io.read()

dpy = nil

return 0
