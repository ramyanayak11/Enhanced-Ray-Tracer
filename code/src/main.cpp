// Ramya Nayak
// 
// Final Project - Enhanced Ray Tracer
// CS 116A - Introduction to Computer Graphics
//
// December 13, 2024



// Acknowledgements: 
//		Some starter code was provided by Professor Kevin Smith during past projects.



#include "ofMain.h"
#include "ofApp.h"
//========================================================================
int main() {
	
	// Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLWindowSettings settings;
	settings.setSize(1024, 768);
	settings.windowMode = OF_WINDOW; // can also be OF_FULLSCREEN

	auto window = ofCreateWindow(settings);

	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();
}
