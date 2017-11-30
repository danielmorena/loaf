/*==============================================================================

	loaf: lua, osc, and openFrameworks
  
	Copyright (c) 2016 Dan Wilcox <danomatika@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
	
	See https://github.com/danomatika/loaf for documentation

==============================================================================*/
#include "ofApp.h"

#include "config.h"
#include "Loaf.h"

// choose modifier key based on platform
#ifdef __APPLE__
	#define MOD_KEY OF_KEY_SUPER
#else
	#define MOD_KEY OF_KEY_CONTROL
#endif

// OF APP

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	// osc
	sender.set(SEND_HOST, SEND_PORT);
	listener.setPort(LISTEN_PORT);
	listener.setCallback([this](const ofxOscMessage &message) {
		oscReceived(message);
	});
	
	// apply any commandline options
	bool watch = true;
	if(options) {
		if(options->verbose) {
			setVerbose(options->verbose);
		}
		if(options->ignoreChanges) {
			watch = false;
			ofLogVerbose(PACKAGE) << "ignoring script changes";
		}
		if(options->errorExit) {
			script.errorExit = options->errorExit;
			ofLogVerbose(PACKAGE) << "exit after a script error";
		}
		else if(options->errorReload > -1) {
			script.errorReload = options->errorReload * 1000; // s to ms
			ofLogVerbose(PACKAGE) << "reload " << options->errorReload
			                      << "s after a script error";
		}
		
		// load script set via commandline
		if(options->path != "") {
			script.load(options->path, &options->args);
			watcher.addPath(options->path);
		}
		
		// the following should override script
		if(options->sendHost != "") {
			setSendHost(options->sendHost);
		}
		if(options->sendPort > 0) {
			setSendPort(options->sendPort);
		}
		if(options->listenPort > 0) {
			setListenPort(options->listenPort);
		}
		if(options->startListening) {
			startListening();
		}
		if(options->fullscreen) {
			ofSetFullscreen(options->fullscreen);
			ofLogVerbose(PACKAGE) << "starting fullscreen";
		}
		// cleanup
		delete options;
		options = nullptr;
	}
	
	// path watching
	if(watch) {
		watcher.start();
	}

	// print OF binding and current opengl versions
	ofLogVerbose(PACKAGE) << "openframeworks version: "
	                      << OF_VERSION_MAJOR << "."
	                      << OF_VERSION_MINOR << "."
	                      << OF_VERSION_PATCH;
	ofLogVerbose(PACKAGE) << "open gl version: " << glGetString(GL_VERSION);
	
	// print the current osc communication settings
	ofLogVerbose(PACKAGE) << "send host: " << sender.getHost();
	ofLogVerbose(PACKAGE) << "send port: " << sender.getPort();
	ofLogVerbose(PACKAGE) << "listen port: " << listener.getPort();
	if(listener.isListening()) {
		ofLogVerbose(PACKAGE) << "started listening";
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	// process received OSC events
	listener.update();
	
	// process any change events
	while(watcher.waitingEvents()) {
		pathChanged(watcher.nextEvent());
	}

	/// update the script
	script.update();
	script.lua.scriptUpdate();
}

//--------------------------------------------------------------
void ofApp::draw() {
	script.lua.scriptDraw();
	script.drawError();
}

//--------------------------------------------------------------
void ofApp::exit() {
	script.lua.scriptExit();
	listener.stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// handle special key events
	if(ofGetKeyPressed(MOD_KEY)) {
		switch(key) {
			case 'f':
				ofToggleFullscreen();
				return;
			case 'r':
				script.reload();
				return;
			case 'c':
				script.clear();
				return;
		}
	}

	// pass everything else to lua
	script.lua.scriptKeyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	script.lua.scriptKeyReleased(key);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	script.setMouseGlobals(x, y);
	script.lua.scriptMouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	script.setMouseGlobals(x, y);
	script.lua.scriptMouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	script.setMouseGlobals(x, y);
	script.lua.scriptMousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	script.setMouseGlobals(x, y);
	script.lua.scriptMouseReleased(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
	script.lua.scriptMouseEntered(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
	script.lua.scriptMouseExited(x, y);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	script.lua.scriptWindowResized(w, h);
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
	string path = dragInfo.files[0];
	if(Script::isLoadablePath(path)) {
		script.load(path);
		watcher.removeAllPaths();
		watcher.addPath(path);
	}
	else {
		script.lua.scriptDragEvent(dragInfo);
	}
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
	script.lua.scriptGotMessage(msg);
}

// UTILS

//--------------------------------------------------------------
void ofApp::setVerbose(bool verbose) {
	if(verbose) {
		ofSetLogLevel(PACKAGE, OF_LOG_VERBOSE);
		ofLogVerbose(PACKAGE) << "verbose " << (verbose ? "on" : "off");
	}
	else {
		ofLogVerbose(PACKAGE) << "verbose " << (verbose ? "on" : "off");
		ofSetLogLevel(PACKAGE, OF_LOG_NOTICE);
	}
	this->verbose = verbose;
}

//--------------------------------------------------------------
bool ofApp::isVerbose() {
	return verbose;
}

// OSC

//--------------------------------------------------------------
void ofApp::startListening() {
	bool wasListening = listener.isListening();
	listener.start();
	if(!wasListening && !options) {
		ofLogVerbose(PACKAGE) << "started listening";
	}
}

//--------------------------------------------------------------
void ofApp::stopListening() {
	bool wasListening = listener.isListening();
	listener.stop();
	if(wasListening && !options) {
		ofLogVerbose(PACKAGE) << "stopped listening";
	}
}

//--------------------------------------------------------------
void ofApp::setListenPort(int port) {
	if(port < 1024) {
		ofLogWarning(PACKAGE) << "listen port must be > 1024";
		return;
	}
	if(listener.getPort() == port) {
		return; // silently ignore
	}
	listener.setPort(port);
	if(!options) {
		ofLogVerbose(PACKAGE) << "listen port: " << port;
	}
}

//--------------------------------------------------------------
void ofApp::setSendHost(const string &host) {
	if(sender.getHost() == host) {
		return; // silently ignore
	}
	sender.setHost(host);
	if(!options) {
		ofLogVerbose(PACKAGE) << "send host: " << host;
	}
}

//--------------------------------------------------------------
void ofApp::setSendPort(int port) {
	if(port < 1024) {
		ofLogWarning(PACKAGE) << "send port must be > 1024";
		return;
	}
	if(sender.getPort() == port) {
		return; // silently ignore
	}
	sender.setPort(port);
	if(!options) {
		ofLogVerbose(PACKAGE) << "send port: " << port;
	}
}

//--------------------------------------------------------------
void ofApp::oscReceived(const ofxOscMessage & message) {
	if(message.getAddress() == baseAddress + "/load") {
		if(message.getNumArgs() > 0 && message.getArgType(0) == OFXOSC_TYPE_STRING) {
			vector<string> args;
			for(int i = 1; i < message.getNumArgs(); ++i) {
				switch(message.getArgType(i)) {
					case OFXOSC_TYPE_INT32:
						args.push_back(ofToString(message.getArgAsInt32(i)));
						break;
					case OFXOSC_TYPE_INT64:
						args.push_back(ofToString(message.getArgAsInt64(i)));
						break;
					case OFXOSC_TYPE_FLOAT:
						args.push_back(ofToString(message.getArgAsFloat(i)));
						break;
					case OFXOSC_TYPE_DOUBLE:
						args.push_back(ofToString(message.getArgAsDouble(i)));
						break;
					case OFXOSC_TYPE_STRING:
						args.push_back(message.getArgAsString(i));
						break;
					case OFXOSC_TYPE_SYMBOL:
						args.push_back(message.getArgAsSymbol(i));
						break;
					default:
						ofLogWarning(PACKAGE) << "dropping script arg of type: "
						                      << (char)message.getArgType(1);
						break;
				}
					
			}
			script.load(message.getArgAsString(0), &args);
		}
	}
	else if(message.getAddress() == baseAddress + "/reload") {
		script.reload();
	}
	else if(message.getAddress() == baseAddress + "/quit") {
		ofLogVerbose(PACKAGE) << "received quit message, exiting...";
		ofExit();
	}
	else {
		// forward message to lua
		script.oscReceived(message);
	}
}

// PATCH WATCHER

//--------------------------------------------------------------
void ofApp::pathChanged(const PathWatcher::Event &event) {
	switch(event.change) {
		case PathWatcher::CREATED:
			ofLogVerbose(PACKAGE) << "path created " << event.path;
			break;
		case PathWatcher::MODIFIED:
			ofLogVerbose(PACKAGE) << "path modified " << event.path;
			break;
		case PathWatcher::DELETED:
			ofLogVerbose(PACKAGE) << "path deleted " << event.path;
			return;
		default: // NONE
			return;
	}
	if(Script::isLoadablePath(event.path)) {
		script.load(event.path);
	}
}
