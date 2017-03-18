loaf: lua, osc, and openFrameworks
==================================

<p align="center">
	<img src="http://danomatika.com/code/loaf/loaf_workflow.png"/>
</p>

Copyright (c) [Dan Wilcox](danomatika.com) 2016-17

GPL v3

For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "LICENSE.txt," in this distribution.

**loaf website**: [danomatika.com/code/loaf](http://danomatika.com/code/loaf)

DESCRIPTION
-----------

> cut off a slice of something nice

loaf is an interpreter for [openFrameworks](http://openframeworks.cc) which allows you to write OF applications in the [Lua](http://www.lua.org) scripting language. This means you can quickly create using openFrameworks but without having to compile C++ or use a heavy IDE like Xcode or Visual Studio. A built-in OSC (Open Sound Control) server enables loaf to communicate natively with other creative coding and music applications over a network connection.

> Is loaf a replacement for building a native C++ application?

No. If you need to be able to include whatever openFrameworks addon you need, stay with C++.

> So what is it for?

loaf is intended as a simple sandbox for sketching using the openFrameworks core API with a live-coding feel: make changes and see the result quickly. No compiling, no low level errors, just the basics. Think of loaf kind of like [Processing](http://processing.org) without the run button.

### Background

loaf is the result of the author's need for a visual coding tool for performance that can run almost anywhere and communicate with tools such as [Pure Data](http://puredata.info) or [Max MSP](https://cycling74.com/products/max/). Using Lua allows for loaf to quickly react to script changes so the process of creation is quick and matches that of a dataflow environment like Pd. The native inclusion of OSC allows for quick coupling of loaf and other creative applications for experimentation.

loaf's design is influenced by the [LÖVE](https://love2d.org) Lua game engine and the [Fluxus](http://www.pawfal.org/fluxus/) live-coding environment. It is essentially the 3rd (or 4th) iteration of a similar tool for the author's [robotcowboy](http://robotcowboy.com) wearable performance project (called "[rc-visual](https://github.com/danomatika/rc-visual)") and was originally a C++ application using [SDL](http://libsdl.org) and blitting to a console framebuffer. loaf is now used as the interpreter for rc-visual which is now implemented completely in Lua and works with it's input complement, [joyosc](https://github.com/danomatika/joyosc).

QUICK START
-----------

Download a release build from [docs.danomatika.com/releases/loaf](http://docs.danomatika.com/releases/loaf/) (currently macOS only) or build loaf after git cloning from [Github](https://github.com/danomatika/loaf)

Once you have a copy of loaf, drag the loaf.app into your Applications folder (macOS) or simple run it from the loaf folder by double-clicking.

_Note: A more comprehensive User Guide will be added in the future._

### Basic usage

* drag a .lua script onto loaf to run it
* either save the script somehow (in a text editor) and/or use Super+R in loaf to reload it automatically
* toggle fullscreen with Super+F
* script errors are shown on the app window and on the console if you run it in a terminal application
* prints are shown in the console (useful for debugging)

### Using Lua and OF

A quick overview of using Lua and the Lua bindings for openFrameworks can be found in the [ofxLua readme](https://github.com/danomatika/ofxLua#of-api-bindings).

The best place to start is to look at the examples included with loaf zip and on the [loaf Github repo](https://github.com/danomatika/loaf/tree/master/examples). Another useful source is the [ofxLua example scripts](https://github.com/danomatika/ofxLua/tree/master/luaExample/bin/data/scripts).

### loaf Commandline Options

loaf has a number of options which can be set when running it from the commandline, the most basic being a script to run:

    loaf path/to/script.lua

Use -h to print a usage help print:

~~~
Usage: loaf [options] [PATH [args]]

  lua, osc, and openFrameworks

Options:
  -h, --help       print usage and exit
  --version        print version and exit
  -a, --address    OSC host address to send to (default: localhost)
  -p, --port       OSC port to send to (default: 8880)
  -l, --listen     OSC port to listen on (default: 9990)
  -s, --start      start listening for OSC messages
  -f, --fullscreen start in fullscreen
  -i, --ignore     ignore script changes
  -v, --verbose    verbose printing

Arguments:
  PATH             optional lua script or folder to run
  args...          arguments to pass to the lua script
~~~

You can also pass arguments to the script itself by passing them after the script file:

    loaf script.lua hello 123

See the argument test script for more details: `examples/tests/arg.lua`

#### macOS

On macOS, you can run a .app from Terminal by calling the binary hidden inside the application bundle:

    /Applications/loaf.app/Contents/MacOS/loaf -h

To make this less cumbersome, you can add an alias in your .bash_profile:

    alias loaf="/Applications/loaf.app/Contents/MacOS/loaf"

which allows you to call loaf with just the short alias instead:

    loaf -h

BUILDING LOAF
-------------

To build loaf, you will need a copy of openFrameworks: [http://openframeworks.cc/download/](http://openframeworks.cc/download/)

loaf requires the following addons:

* [ofxLua](https://github.com/danomatika/ofxLua)
* ofxOsc (included with openFrameworks)

Project files for building loaf on Windows or Linux are not included so you will need to generate them for your operating system and development environment using the OF ProjectGenerator which is included with the openFrameworks distribution.

To (re)generate project files for an existing project:

* click the "Import" button in the ProjectGenerator
* navigate the to base folder for the project ie. "loaf"
* click the "Update" button

If everything went Ok, you should now be able to open the generated project and build/run the example.

### OSX

Open the Xcode project, select the "loaf Release" scheme, and hit "Run". Once build, the loaf.app is found in the `bin` directory.

### Linux

Open the Code::Blocks .cbp and hit F9 to build. Optionally, you can build the example with the Makefile.

To build and run it on the terminal:

    make
    make run

### Windows

_Instructions for Visual Studio to be added here._

DEVELOPING
----------

You can help develop ofxLua on GitHub: https://github.com/danomatika/loaf

Create an account, clone or fork the repo, then request a push/merge.

If you find any bugs or suggestions please log them to GitHub as well.
