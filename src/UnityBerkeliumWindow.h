// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// This source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
//
// Contributors (Unity forum usernames): reissgrant, agentdm
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UNITYBERKELIUMWINDOW_H
#define UNITYBERKELIUMWINDOW_H

// Includes
#include "global.h"
#include <berkelium/Berkelium.hpp>
#include <berkelium/Window.hpp>
#include <berkelium/WindowDelegate.hpp>
#include <vector>

/** Class Description */
class UnityBerkeliumWindow: Berkelium::WindowDelegate
{
public:
	// The callback function that is called when a paint event occurs
	// Note: Unity crashes when providing a struct or more than two standard parameters
	//typedef void (*SetPixelsFunc)(Berkelium::Rect);
	typedef void (*SetPixelsFunc)(/*int left, int top, int width, int height*/);
	typedef void (*ApplyTextureFunc)();

	// The callback function that is called from a javascript externalHost call
	typedef void (*ExternalHostFunc)(/*const wchar_t *message*/);

	// Constructors and destructor
	UnityBerkeliumWindow(int uniqueID, float *buffer, bool transparency, int width, int height, const string &url);
	virtual ~UnityBerkeliumWindow();

	// Information
	Berkelium::Window *getBerkeliumWindow() const { return m_pWindow; }

	// Callbacks
	void setPaintFunctions(SetPixelsFunc setPixelsFunc, ApplyTextureFunc applyTextureFunc);
	void setExternalHostCallback(ExternalHostFunc callback);

	// Current paint info
	const Berkelium::Rect &getLastDirtyRect() const { return m_lastDirtyRect; }

	// Last external host message
	const std::wstring &getLastExternalHostMessage() const { return m_lastExternalHostMessage; }

protected:
	// Berkelium::WindowDelegate functions
	virtual void onAddressBarChanged(Berkelium::Window *win, Berkelium::URLString newURL);
	virtual void onStartLoading(Berkelium::Window *win, Berkelium::URLString newURL);
	virtual void onLoad(Berkelium::Window *win);
	virtual void onCrashedWorker(Berkelium::Window *win);
	virtual void onCrashedPlugin(Berkelium::Window *win, Berkelium::WideString pluginName);
	virtual void onProvisionalLoadError(Berkelium::Window *win, Berkelium::URLString url, int errorCode, bool isMainFrame);
	virtual void onConsoleMessage(Berkelium::Window *win, Berkelium::WideString message, Berkelium::WideString sourceId, int line_no);
	virtual void onScriptAlert(Berkelium::Window *win, Berkelium::WideString message, Berkelium::WideString defaultValue, Berkelium::URLString url, int flags, bool &success, Berkelium::WideString &value);
	virtual void freeLastScriptAlert(Berkelium::WideString lastValue);
	virtual void onNavigationRequested(Berkelium::Window *win, Berkelium::URLString newUrl, Berkelium::URLString referrer, bool isNewWindow, bool &cancelDefaultAction);
	virtual void onLoadingStateChanged(Berkelium::Window *win, bool isLoading);
	virtual void onTitleChanged(Berkelium::Window *win, Berkelium::WideString title);
	virtual void onTooltipChanged(Berkelium::Window *win, Berkelium::WideString text);
	virtual void onCrashed(Berkelium::Window *win);
	virtual void onUnresponsive(Berkelium::Window *win);
	virtual void onResponsive(Berkelium::Window *win);
	virtual void onExternalHost(Berkelium::Window *win, Berkelium::WideString message, Berkelium::URLString origin, Berkelium::URLString target);
	virtual void onCreatedWindow(Berkelium::Window *win, Berkelium::Window *newWindow, const Berkelium::Rect &initialRect);
	virtual void onPaint(Berkelium::Window *win, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects, int dx, int dy, const Berkelium::Rect &scrollRect);
	virtual void onWidgetCreated(Berkelium::Window *win, Berkelium::Widget *newWidget, int zIndex);
	virtual void onWidgetDestroyed(Berkelium::Window *win, Berkelium::Widget *wid);
	virtual void onWidgetResize(Berkelium::Window *win, Berkelium::Widget *wid, int newWidth, int newHeight);
	virtual void onWidgetMove(Berkelium::Window *win, Berkelium::Widget *wid, int newX, int newY);
	virtual void onWidgetPaint(Berkelium::Window *win, Berkelium::Widget *wid, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects, int dx, int dy, const Berkelium::Rect &scrollRect);
	virtual void onCursorUpdated(Berkelium::Window *win, const Berkelium::Cursor& newCursor);
	virtual void onShowContextMenu(Berkelium::Window *win, const Berkelium::ContextMenuEventArgs& args);

	// Protected functions
	void convertColors(const Berkelium::Rect &rect, const unsigned char *sourceBuffer);

	// Member variables
	Berkelium::Window *m_pWindow;
	int m_id;
	float *m_buffer;
	bool m_transparency;
	int m_width, m_height;
	string m_url;

	SetPixelsFunc m_setPixelsFunc;
	ApplyTextureFunc m_applyTextureFunc;
	Berkelium::Rect m_lastDirtyRect;

	ExternalHostFunc m_externalHostFunc;
	std::wstring m_lastExternalHostMessage;
};

#endif // UNITYBERKELIUMWINDOW_H
