// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// Part of this source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
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
	if(it == windows.end())
	{
		cerr << "Warning: no berkelium window found with ID " << id << "!" << endl;
		return 0;
	}

	return it->second;
}


/*******************
* Global functions *
*******************/

PLUGIN_API void Berkelium_init()
{
	if(refCount == 0)
	{
		// Redirect stderr to log file
		::freopen(logFilename, "w", stderr);

		// Initialize the Berkelium engine
		cerr << "First reference to Berkelium initialized: initializing the library" << endl;
		Berkelium::init(Berkelium::FileString::empty());
		cerr << "... done initializing Berkelium" << endl;
	}

	++refCount;
}

PLUGIN_API void Berkelium_destroy()
{
	--refCount;

	if(refCount == 0)
	{
		cerr << "Last reference to Berkelium destroyed: destroying the library" << endl;

		// Destroy the windows that still exist
		// Note: we update Berkelium after each destroy to ensure no messages are left in the message loop
		for(WindowMap::iterator it = windows.begin(); it != windows.end(); ++it)
		{
			delete it->second;
			Berkelium::update();
		}
		windows.clear();

		// Close the berkelium backend
		Berkelium::destroy();
		cerr << "... done destroying Berkelium" << endl;

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

PLUGIN_API bool Berkelium_Window_create(int uniqueID, float *colors, bool transparency, int width, int height, char *url)
{
	if(windows.find(uniqueID) != windows.end())
	{
		cerr << "Error: a berkelium window with ID " << uniqueID << " already exists!" << endl;
		return false;
	}

	UnityBerkeliumWindow *pWindow = new UnityBerkeliumWindow(uniqueID, colors, transparency, width, height, url);

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

		cerr << "Berkelium window destroyed: " << windowID << endl;
	}
}

PLUGIN_API void Berkelium_Window_navigateTo(int windowID, char *url)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->navigateTo(url);
}

PLUGIN_API void Berkelium_Window_setPaintFunctions(int windowID, UnityBerkeliumWindow::SetPixelsFunc setPixelsFunc, UnityBerkeliumWindow::ApplyTextureFunc applyTextureFunc)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->setPaintFunctions(setPixelsFunc, applyTextureFunc);
}

// Note: we need this function because I can't get parameters in function pointers to work properly
PLUGIN_API Berkelium::Rect Berkelium_Window_getLastDirtyRect(int windowID)
{
	Berkelium::Rect result;

	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		result = pWindow->getLastDirtyRect();

	return result;
}

PLUGIN_API void Berkelium_Window_focus(int windowID)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->focus();
}

PLUGIN_API void Berkelium_Window_unfocus(int windowID)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->unfocus();
}

PLUGIN_API void Berkelium_Window_mouseDown(int windowID, int button)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->mouseButton(button, true);
}

PLUGIN_API void Berkelium_Window_mouseUp(int windowID, int button)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->mouseButton(button, false);
}

PLUGIN_API void Berkelium_Window_mouseMove(int windowID, int x, int y)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->mouseMoved(x, y);
}

PLUGIN_API void Berkelium_Window_textEvent(int windowID, wchar_t c)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->textEvent(&c, 1);
}

PLUGIN_API void Berkelium_Window_keyEvent(int windowID, bool pressed, int mods, int vk_code, int scancode)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->getBerkeliumWindow()->keyEvent(pressed, mods, vk_code, scancode);
}


//-- Javascript functionality --//

/// Thanks to agentdm
PLUGIN_API void Berkelium_Window_executeJavascript(int windowID, char* javaScript)
{
	cerr << "Javascript call made: " << javaScript << endl;
	UnityBerkeliumWindow *pWindow = getWindow(windowID);

	size_t scriptLength = ::strlen(javaScript);

	// Convert to Wchar ( is there an easier way to do this? )
	wchar_t *wctStrJScript = new wchar_t[scriptLength + 1];
	MultiByteToWideChar( CP_ACP, 0, javaScript, scriptLength, wctStrJScript, scriptLength);
	wctStrJScript[scriptLength] = 0;

	std::wcerr << "Javascript converted: " << wctStrJScript << endl;

	if(pWindow)
		pWindow->getBerkeliumWindow()->executeJavascript(Berkelium::WideString::point_to(wctStrJScript, scriptLength));

	delete[] wctStrJScript;
}


PLUGIN_API void Berkelium_Window_setExternalHostCallback(int windowID, UnityBerkeliumWindow::ExternalHostFunc callback)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		pWindow->setExternalHostCallback(callback);
}

// TEMP This should be a parameter in the callback function
PLUGIN_API const wchar_t *Berkelium_Window_getLastExternalHostMessage(int windowID)
{
	UnityBerkeliumWindow *pWindow = getWindow(windowID);
	if(pWindow)
		return pWindow->getLastExternalHostMessage().c_str();
	else
		return L"";
}
