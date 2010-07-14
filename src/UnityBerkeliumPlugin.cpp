// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// This source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
//
// Contributors (Unity forum usernames): reissgrant, agentdm
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "global.h"
#include "UnityBerkeliumWindow.h"
#include <map>

static const char *logFilename = "BerkeliumUnityTest.log";

// We reference-count the Berkelium backend
static int refCount = 0;

// A map that holds the created windows
typedef std::map<int, UnityBerkeliumWindow *> WindowMap;
static WindowMap windows;

UnityBerkeliumWindow *getWindow(int id)
{
	WindowMap::const_iterator it = windows.find(id);
	return (it == windows.end()) ? 0 : it->second;
}


/*******************
* Global functions *
*******************/

PLUGIN_API void Berkelium_init()
{
	if(refCount == 0)
	{
		// Initialize the Berkelium engine
		Berkelium::init();

		// Redirect stderr to log file
		::freopen(logFilename, "w", stderr);
	}

	++refCount;
}

PLUGIN_API void Berkelium_destroy()
{
	--refCount;

	if(refCount == 0)
	{
		// Destroy the windows that still exist
		for(WindowMap::iterator it = windows.begin(); it != windows.end(); ++it)
			delete it->second;
		windows.clear();

		// Close the berkelium backend
		Berkelium::destroy();

		// Close the log file
		fclose(stderr);
	}
}


PLUGIN_API void Berkelium_update()
{
	//! @todo We need to call this only once, not for each object
	Berkelium::update();
}


/****************************
* Window-specific functions *
****************************/

PLUGIN_API bool Berkelium_Window_create(int uniqueID, float *colors, bool transparency, int width, int height, char *url, UnityBerkeliumWindow::PaintFunc paintFunc)
{
	if(windows.find(uniqueID) != windows.end())
	{
		cerr << "Error: a berkelium window with ID " << uniqueID << " already exists!" << endl;
		return false;
	}

	UnityBerkeliumWindow *pWindow = new UnityBerkeliumWindow(uniqueID, colors, transparency, width, height, url, paintFunc);

	cerr << "Berkelium window created: " << pWindow << " (size=" << width << ", " << height << "; url=" << url << ")" << endl;
	windows[uniqueID] = pWindow;

	return true;
}

PLUGIN_API void Berkelium_Window_destroy(int windowID)
{
	WindowMap::iterator it = windows.find(windowID);
	if(it != windows.end())
	{
		delete it->second;
		windows.erase(it);
	}
}

// Note: we need this function because I can't get parameters in function pointers to work properly
PLUGIN_API Berkelium::Rect Berkelium_Window_getLastDirtyRect(int windowID)
{
	Berkelium::Rect result;

	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		result = pWindow->getLastDirtyRect();
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;

	return result;
}

PLUGIN_API void Berkelium_Window_mouseDown(int windowID, int button)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->mouseButton(button, true);
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
}

PLUGIN_API void Berkelium_Window_mouseUp(int windowID, int button)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->mouseButton(button, false);
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
}

PLUGIN_API void Berkelium_Window_mouseMove(int windowID, int x, int y)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->mouseMoved(x, y);
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
}

PLUGIN_API void Berkelium_Window_textEvent(int windowID, wchar_t c)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->textEvent(&c, 1);
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
}

PLUGIN_API void Berkelium_Window_keyEvent(int windowID, bool pressed, int mods, int vk_code, int scancode)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->keyEvent(pressed, mods, vk_code, scancode);
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
}

/// Thanks to agentdm
PLUGIN_API void Berkelium_Window_executeJavascript(int windowID, char* javaScript)
{
	cerr << "Javascript call made: " << javaScript << endl;
	UnityBerkeliumWindow *pWindow = getWindow(windowID);

	size_t scriptLength = ::strlen(javaScript);

	// Convert to Wchar ( is there an easier way to do this? )
	const int strlength = 100;
	wchar_t wctStrJScript[strlength];
	MultiByteToWideChar( CP_ACP, 0, javaScript, scriptLength, wctStrJScript, strlength);
	wctStrJScript[scriptLength] = 0;

	std::wcerr << "Javascript converted: " << wctStrJScript << endl;

	if(pWindow)
		pWindow->getBerkeliumWindow()->executeJavascript(wctStrJScript, scriptLength);
	else
		cerr << "Warning: no berkelium window found with ID " << windowID << "!" << endl;
}
