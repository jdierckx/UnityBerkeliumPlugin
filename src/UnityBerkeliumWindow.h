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

// Forward declarations
typedef std::vector<Berkelium::Rect> RectVector;

/** Class Description */
class UnityBerkeliumWindow: Berkelium::WindowDelegate
{
public:
	// Constructors and destructor
	UnityBerkeliumWindow(int uniqueID, float *buffer, int width, int height, const string &url);
	virtual ~UnityBerkeliumWindow();

	// Information
	Berkelium::Window *getBerkeliumWindow() const { return m_pWindow; }

	// Dirty rectangles
	bool isDirty() const { return !m_dirtyRects.empty(); }
	void clearDirty() { m_dirtyRects.clear(); }
	RectVector getDirtyRects();

protected:
	// Berkelium::WindowDelegate functions
	virtual void onPaint(Berkelium::Window *pWindow, const unsigned char *sourceBuffer, const Berkelium::Rect &rect, int dx, int dy, const Berkelium::Rect &scrollRect);
	virtual void onAddressBarChanged(Berkelium::Window *win, const char* newURL, size_t newURLSize);
	virtual void onStartLoading(Berkelium::Window *win, const char *newURL, size_t newURLSize);
	virtual void onLoad(Berkelium::Window *win);
	virtual void onLoadError(Berkelium::Window *win, const char* error, size_t errorLength);
	virtual void onBeforeUnload(Berkelium::Window *win, bool *proceed);
	virtual void onCancelUnload(Berkelium::Window *win);
	virtual void onCrashed(Berkelium::Window *win);
	virtual void onUnresponsive(Berkelium::Window *win);
	virtual void onResponsive(Berkelium::Window *win);
	virtual void onChromeSend(Berkelium::Window *win, Data message, const Data* content, size_t numContents);
	virtual void onCreatedWindow(Berkelium::Window *win, Berkelium::Window *newWindow);
	virtual void onWidgetCreated(Berkelium::Window *win, Berkelium::Widget *newWidget, int zIndex);
	virtual void onWidgetDestroyed(Berkelium::Window *win, Berkelium::Widget *newWidget);
	virtual void onWidgetResize(Berkelium::Window *win, Berkelium::Widget *wid, int newWidth,int newHeight);
	virtual void onWidgetMove(Berkelium::Window *win, Berkelium::Widget *wid, int newX, int newY);
	virtual void onWidgetPaint(Berkelium::Window *win, Berkelium::Widget *wid, const unsigned char *sourceBuffer, const Berkelium::Rect &rect,int dx, int dy, const Berkelium::Rect &scrollRect);
	virtual void onCursorUpdated(const Berkelium::Cursor& newCursor);
	virtual void onShowContextMenu(Berkelium::Window *win, const Berkelium::ContextMenuEventArgs& args);

	// Member variables
	Berkelium::Window *m_pWindow;
	int m_id;
	float *m_buffer;
	int m_width, m_height;
	string m_url;

	RectVector m_dirtyRects;
};

#endif // UNITYBERKELIUMWINDOW_H
