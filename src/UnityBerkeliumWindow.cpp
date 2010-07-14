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

UnityBerkeliumWindow::UnityBerkeliumWindow(int uniqueID, float *buffer, bool transparency, int width, int height, const string &url, PaintFunc paintFunc)
: m_id(uniqueID), m_buffer(buffer), m_transparency(transparency), m_width(width), m_height(height), m_url(url), m_paintFunc(paintFunc)
{
	assert(m_buffer);
	assert(width > 0 && height > 0);

	m_pWindow = Berkelium::Window::create();
	m_pWindow->setDelegate(this);
	m_pWindow->setTransparent(transparency);
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


/**************************************
* Berkelium::WindowDelegate functions *
**************************************/

void UnityBerkeliumWindow::onPaint(Berkelium::Window *pWindow, const unsigned char *sourceBuffer, const Berkelium::Rect &rect, int dx, int dy, const Berkelium::Rect &scrollRect)
{
#ifdef DEBUG
	cerr << "[UnityBerkeliumWindow] onPaint called (window: " << pWindow << ")" << endl;
	cerr << "  rect: (left=" << rect.left() << ", width=" << rect.width() << ", top=" << rect.top() << ", height=" << rect.height() << ")" << endl;
	cerr << "  sourceBuffer: " << (void *) sourceBuffer << endl;
#endif

	// Scrolling
	if(dx != 0 || dy != 0)
	{
#if 0
		// scroll_rect contains the Rect we need to move
		// First we figure out where the the data is moved to by translating it
		//Berkelium::Rect scrolled_rect = scrollRect.translate(-dx, -dy);
		Berkelium::Rect scrolled_rect = scrollRect;
		scrolled_rect.mLeft -= dx;
		scrolled_rect.mTop -= dy;

		// Next we figure out where they intersect, giving the scrolled
		// region
		Berkelium::Rect scrolled_shared_rect = scrollRect.intersect(scrolled_rect);

		// Only do scrolling if they have non-zero intersection
		if(scrolled_shared_rect.width() > 0 && scrolled_shared_rect.height() > 0)
		{
			// And the scroll is performed by moving shared_rect by (dx,dy)
			//Berkelium::Rect shared_rect = scrolled_shared_rect.translate(dx, dy);
			Berkelium::Rect shared_rect = scrolled_shared_rect;
			shared_rect.mLeft += dx;
			shared_rect.mTop += dy;

			for(int y = scrolled_shared_rect.top(); y < shared_rect.bottom(); ++y)
			{
				::memcpy(
					m_buffer + y * m_width * 4 /*+ scrolled_shared_rect.left() * 4*/,
					m_buffer + ((scrolled_shared_rect.top() + y) * m_width /*+ scrolled_shared_rect.left()*/) * 4,
					scrolled_shared_rect.width() * 4
				);
			}

			// Copy the data out of the texture
			/*size_t scrollBuffSize = scrolled_shared_rect.width() * scrolled_shared_rect.height() * 4;
			unsigned char *scrollBuffer = new unsigned char[scrollBuffSize];
			
			for(int line = 0; line < scrolled_shared_rect.height(); ++line)
			{
				::memcpy(
					scrollBuffer + line * scrolled_shared_rect.width() * 4,

			}*/
#if 0
			glGetTexImage(
				GL_TEXTURE_2D, 0,
				GL_BGRA, GL_UNSIGNED_BYTE,
				scroll_buffer
				);

			// Annoyingly, OpenGL doesn't provide convenient primitives, so
			// we manually copy out the region to the beginning of the
			// buffer
			int wid = scrolled_shared_rect.width();
			int hig = scrolled_shared_rect.height();
			for(int jj = 0; jj < hig; jj++) {
				memcpy(
					scroll_buffer + (jj*wid * 4),
					scroll_buffer + ((scrolled_shared_rect.top()+jj)*dest_texture_width + scrolled_shared_rect.left()) * 4,
					wid*4
					);
			}

			// And finally, we push it back into the texture in the right
			// location
			glTexSubImage2D(GL_TEXTURE_2D, 0,
				shared_rect.left(), shared_rect.top(),
				shared_rect.width(), shared_rect.height(),
				GL_BGRA, GL_UNSIGNED_BYTE, scroll_buffer
				);
#endif
		}
#endif
	}

	// Apply the dirty rectangle
	convertColors(rect, sourceBuffer);

	// Call the paint callback
	m_lastDirtyRect = rect;
	m_paintFunc(/*rect.left(), rect.top(), rect.width(), rect.height()*/);
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

void UnityBerkeliumWindow::onCursorUpdated(const Berkelium::Cursor& newCursor)
{
	cerr << "[UnityBerkeliumWindow] onCursorUpdated called" << endl;
}

void UnityBerkeliumWindow::onShowContextMenu(Berkelium::Window *win, const Berkelium::ContextMenuEventArgs& args)
{
	cerr << "[UnityBerkeliumWindow] onShowContextMenu called (window: " << win << ")" << endl;
}


/**********************
* Protected functions *
**********************/

void UnityBerkeliumWindow::convertColors(const Berkelium::Rect &rect, const unsigned char *sourceBuffer)
{
	// Note: we convert from BGRA bytes to RGBA floats. RGB24 textures in Unity are still 32bit in memory.
	for(int x = rect.left(); x < rect.right(); ++x)
	{
		for(int y = rect.top(); y < rect.bottom(); ++y)
		{
			int idx = (y - rect.top()) * rect.width() + (x - rect.left());

			m_buffer[idx * 4 + 0] = sourceBuffer[idx * 4 + 2] / 255.0f; // R
			m_buffer[idx * 4 + 1] = sourceBuffer[idx * 4 + 1] / 255.0f; // G
			m_buffer[idx * 4 + 2] = sourceBuffer[idx * 4 + 0] / 255.0f; // B
			
			if(m_transparency)
				m_buffer[idx * 4 + 3] = sourceBuffer[idx * 4 + 3] / 255.0f; // A
		}
	}
}
