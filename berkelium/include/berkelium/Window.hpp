/*  Berkelium - Embedded Chromium
 *  Window.hpp
 *
 *  Copyright (c) 2009, Daniel Reiter Horn
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _BERKELIUM_WINDOW_HPP_
#define _BERKELIUM_WINDOW_HPP_

#include <vector>

#include "berkelium/Context.hpp"
#include "berkelium/Rect.hpp"

namespace Berkelium {

class WindowImpl;
class Widget;
class WindowDelegate;

enum KeyModifier {
	SHIFT_MOD	= 1 << 0,
	CONTROL_MOD	= 1 << 1,
	ALT_MOD 	= 1 << 2,
	META_MOD	= 1 << 3,
	KEYPAD_KEY	= 1 << 4, // If the key is on the keypad (use instead of keypad-specific keycodes)
	AUTOREPEAT_KEY = 1 << 5, // If this is not the first KeyPress event for this key
	SYSTEM_KEY	= 1 << 6 // if the keypress is a system event (WM_SYS* messages in windows)
};

class BERKELIUM_EXPORT Window {
protected:
    typedef std::vector<Widget*> WidgetList;
public:
    typedef WidgetList::const_iterator BackToFrontIter;
    typedef WidgetList::const_reverse_iterator FrontToBackIter;
protected:
    Window();
    Window (const Context*otherContext);

public:
    static Window* create();
    static Window* create(const Context&otherContext);
    virtual ~Window();

    virtual Widget* getWidget() const=0; // could return NULL.

    inline Context *getContext() const {
        return mContext;
    }

    void setDelegate(WindowDelegate *delegate) {
        mDelegate = delegate;
    }

    void appendWidget(Widget *wid) {
        mWidgets.push_back(wid);
    }
    void removeWidget(Widget *wid) {
        for (WidgetList::iterator it = mWidgets.begin();
             it != backEnd();
             ++it)
        {
            if (*it == wid) {
                mWidgets.erase(it);
                return;
            }
        }
    }
    BackToFrontIter backIter() const {
        return mWidgets.begin();
    }
    BackToFrontIter backEnd() const {
        return mWidgets.end();
    }

    FrontToBackIter frontIter() const {
        return mWidgets.rbegin();
    }
    FrontToBackIter frontEnd() const {
        return mWidgets.rend();
    }

    Widget *getWidgetAtPoint(int xPos, int yPos, bool returnRootIfOutside=false) const;

    virtual void setTransparent(bool istrans)=0;

    virtual void focus()=0;
    virtual void unfocus()=0;

    virtual void mouseMoved(int xPos, int yPos)=0;
    virtual void mouseButton(unsigned int buttonID, bool down)=0;
    virtual void mouseWheel(int xScroll, int yScroll)=0;

    virtual void textEvent(const wchar_t *evt, size_t evtLength)=0;
    virtual void keyEvent(bool pressed, int mods, int vk_code, int scancode)=0;


    virtual void resize(int width, int height)=0;
    virtual void executeJavascript(const wchar_t* javascript, size_t javascriptLength)=0;
    virtual bool navigateTo(const char *url, size_t urlLength)=0;
    virtual void refresh()=0;

    virtual void cut()=0;
    virtual void copy()=0;
    virtual void paste()=0;
    virtual void undo()=0;
    virtual void redo()=0;
    virtual void del()=0;
    virtual void selectAll()=0;

    virtual WindowImpl*getImpl()=0;
protected:
    Context *mContext;
    WindowDelegate *mDelegate;

    WidgetList mWidgets;
};

}

#endif
