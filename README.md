# Longshot
This is a blocky sci-fi roguelike.

This was my first cpp project. I started it in 2010 or so. I haven't done anything meaningful to it in a while and figured it was probably time to unleash it on the world at large in case someone wants to make some progress on it or learn how not to program.

Get more info here: http://www.giantcitygames.com/games/longshot.html

## Building and Running
Last I checked, this works on at least Windows and Mac OSX, which means it probably works on Linux ... someone care to confirm?

It makes use of SDL2 and OpenGL, so there is not reason that it shouldn't run just about anywhere.

### Windows
I developed this primarily in Visual Studio. The last version that I was using was VS 2013, so maybe start there? You'll need to download all the SDL development libs and runtime libs. I only think the Release build plan will likely work.

### Mac OS
Use Brew to install the dependencies (basically SDL2 and all the helpers that are needed).

Then run ```build.sh``` to produce the executable.

### Assets
You'll probably want the amazing assets for this. Get them here in unzip them in the same directory as the executable (they will be in subdirectories):

http://www.giantcitygames.com/files/longshot_assets.zip

Better yet, make good assets send them my way!

## How to Play
You start on board your ship. In space. Hit 'esc' to bring up the menu:

'Planet Map' will allow you to teleport to the surface of the planet that you are currently orbiting.
'Galaxy Map' will allow you to visit other planets.
'Merchant' will allow you to buy some random gear. For free.
'Dungeon Map' will draw a pretty dungeon layout. You should really head to the surface to check them out in person.

### Controls
```
esc - menu
w,a,s,d - move
space - jump
left mouse button - use primary
right mouse button - use secondary
e - pick up items
g - get some guns and stuff
F1 - respawn
F5 - show world chunks
p - pause
  n - next frame
'[' / ']' - increase / decrease draw distance

c - character sheet/inventory
  mouse wheel - change selection
  u - use selected item
  left mouse button - swap current selection into primary
  right mouse button - swap current selection into secondary
  m - destroy current selection
  esc, c - hide character sheet
```

## Contribute
Please fork and improve this code! Send me a PR and I'll be happy to take a look.
