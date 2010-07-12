// Copyright (c) 2010 Jeroen Dierckx - Expertise Centre for Digital Media. All Rights reserved.
// This source code is developed for the flemish (Belgian) OSMA project (http://osma.phl.be/)
//
// Contributors (Unity forum usernames): reissgrant, agentdm
//
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Includes
#include "UnityBerkeliumWindow.h"


/******************************
* Constructors and destructor *
******************************/

UnityBerkeliumWindow::UnityBerkeliumWindow(int uniqueID, float *buffer, int width, int height, const string &url)
: m_id(uniqueID), m_buffer(buffer), m_width(width), m_height(height), m_url(url)
{
	assert(m_buffer);
	assert(width > 0 && height > 0);

	m_pWindow = Berkelium::Window::create();
	m_pWindow->setDelegate(this);
	m_pWindow->resize(width, height);
	m_pWindow->navigateTo(url.data(), url.length());
}

UnityBerkeliumWindow::~UnityBerkeliumWindow()
{
	delete m_pWindow;
}


/******************
* Other functions *
******************/

RectVector UnityBerkeliumWindow::getDirtyRects()
{
	RectVector result = m_dirtyRects;
	m_dirtyRects.clear();
	return result;
}


/**************************************
* Berkelium::WindowDelegate functions *
**************************************/

void UnityBerkeliumWindow::onPaint(Berkelium::Window *pWindow, const unsigned char *sourceBuffer, const Berkelium::Rect &rect, int dx, int dy, const Berkelium::Rect &scrollRect)
{
	cerr << "[UnityBerkeliumWindow] onPaint called (window: " << pWindow << ")" << endl;
	cerr << "  rect: (left=" << rect.left() << ", width=" << rect.width() << ", top=" << rect.top() << ", height=" << rect.height() << ")" << endl;
	cerr << "  sourceBuffer: " << (void *) sourceBuffer << endl;

	// Add to the dirty rectangles vector
	//! @todo How are we going to handle scrolling?
	m_dirtyRects.push_back(rect);

	//! @todo Scrolling
	if(dx != 0 || dy != 0)
	{
	}

#if 1
	// Apply the dirty rectangle
	for(int x = rect.left(); x < rect.right(); ++x)
	{
		for(int y = rect.top(); y < rect.bottom(); ++y)
		{
			int srcIdx = (y - rect.top()) * rect.width() + (x - rect.left());
			//int idx = x * m_height + y;
			int idx = y * m_width + x;
			
			m_buffer[idx * 4 + 0] = sourceBuffer[srcIdx * 4 + 2] / 255.0f; // R
			m_buffer[idx * 4 + 1] = sourceBuffer[srcIdx * 4 + 1] / 255.0f; // G
			m_buffer[idx * 4 + 2] = sourceBuffer[srcIdx * 4 + 0] / 255.0f; // B
			m_buffer[idx * 4 + 3] = sourceBuffer[srcIdx * 4 + 3] / 255.0f; // A
		}
	}
#endif
}

void UnityBerkeliumWindow::onAddressBarChanged(Berkelium::Window *win, const char* newURL, size_t newURLSize)
{
	cerr << "[UnityBerkeliumWindow] onAddressBarChanged called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onStartLoading(Berkelium::Window *win, const char *newURL, size_t newURLSize)
{
	cerr << "[UnityBerkeliumWindow] onStartLoading called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onLoad(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onLoad called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onLoadError(Berkelium::Window *win, const char* error, size_t errorLength)
{
	cerr << "[UnityBerkeliumWindow] onLoadError called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onBeforeUnload(Berkelium::Window *win, bool *proceed)
{
	cerr << "[UnityBerkeliumWindow] onBeforeUnload called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onCancelUnload(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onCancelUnload called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onCrashed(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onCrashed called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onUnresponsive(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onUnresponsive called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onResponsive(Berkelium::Window *win)
{
	cerr << "[UnityBerkeliumWindow] onResponsive called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onChromeSend(Berkelium::Window *win, Data message, const Data* content, size_t numContents)
{
	cerr << "[UnityBerkeliumWindow] onChromeSend called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onCreatedWindow(Berkelium::Window *win, Berkelium::Window *newWindow)
{
	cerr << "[UnityBerkeliumWindow] onCreatedWindow called (window: " << win << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetCreated(Berkelium::Window *win, Berkelium::Widget *newWidget, int zIndex)
{
	cerr << "[UnityBerkeliumWindow] onWidgetCreated called (widget: " << newWidget << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetDestroyed(Berkelium::Window *win, Berkelium::Widget *newWidget)
{
	cerr << "[UnityBerkeliumWindow] onWidgetDestroyed called (widget: " << newWidget << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetResize(Berkelium::Window *win, Berkelium::Widget *wid, int newWidth,int newHeight)
{
	cerr << "[UnityBerkeliumWindow] onWidgetResize called (widget: " << wid << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetMove(Berkelium::Window *win, Berkelium::Widget *wid, int newX, int newY)
{
	cerr << "[UnityBerkeliumWindow] onWidgetMove called (widget: " << wid << ")" << endl;
}

void UnityBerkeliumWindow::onWidgetPaint(Berkelium::Window *win, Berkelium::Widget *wid, const unsigned char *sourceBuffer, const Berkelium::Rect &rect,int dx, int dy, const Berkelium::Rect &scrollRect)
{
	cerr << "[UnityBerkeliumWindow] onWidgetPaint called (widget: " << wid << ")" << endl;
}

	
/**********************
* Protected functions *
**********************/

