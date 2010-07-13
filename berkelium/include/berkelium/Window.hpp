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

class Widget;
class WindowDelegate;

enum KeyModifier {
    SHIFT_MOD      = 1 << 0,
    CONTROL_MOD    = 1 << 1,
    ALT_MOD        = 1 << 2,
    META_MOD       = 1 << 3,
    KEYPAD_KEY     = 1 << 4, // If the key is on the keypad (use instead of keypad-specific keycodes)
    AUTOREPEAT_KEY = 1 << 5, // If this is not the first KeyPress event for this key
    SYSTEM_KEY     = 1 << 6 // if the keypress is a system event (WM_SYS* messages in windows)
};

/** Windows are individual web pages, the equivalent of a single tab in a normal
 *  browser.  Windows mediate interaction between the user and the
 *  renderer. They allow inspection of the page (access to UI widgets),
 *  injection of input (keyboard, mouse), injection of Javascript code,
 *  navigation controls (forward, back, loading URLs), and other utility methods
 *  (cut, copy, paste, etc).  Since the represent off-screen renderers, they
 *  also allow manipulation such as resizting.
 */
class BERKELIUM_EXPORT Window {
protected:
    typedef std::vector<Widget*> WidgetList;

public:
    typedef WidgetList::const_iterator BackToFrontIter;
    typedef WidgetList::const_reverse_iterator FrontToBackIter;

protected:
    /** Construct a completely uninitialized Window -- it will have no backing
     *  renderer or delegate and use a new Context.
     */
    Window();
    /** Construct a Window which uses the specified Context for rendering.
     *  \param otherContext an existing rendering Context to use
     */
    Window (const Context*otherContext);

public:
    /** Create a new Window with all default properties -- a zero sized window
     *  and a default local web page.
     */
    static Window* create();
    /** Create a new Window with all default properties which uses an existing
     *  Context for rendering.  It will be zero sized and use a default local
     *  web page.
     *  \param otherContext an existing context to use for rendering
     *  \returns a new Window or NULL on failure
     */
    static Window* create(const Context&otherContext);

    virtual ~Window();

    virtual Widget* getWidget() const=0; // could return NULL.

    /** Get the rendering context for this Window. */
    inline Context *getContext() const {
        return mContext;
    }

    /** Set the delegate for receiving events, such as paint events and alerts,
     *  from this Window.
     *  \param delegate the WindowDelegate that should receive events, or NULL
     *         to disable delegation of events
     */
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

    /** Look up which widget is at the specified point in the Window.
     *  \param xPos the position on the x-axis of the point to look up
     *  \param yPos the position on the y-axis of the point to look up
     *  \param returnRootIfOutside if true and the specified point lies outside
     *         the Window, returns the root Widget instead of NULL
     *  \returns the Widget at the specified location or NULL if no widgets are
     *           at that point
     */
    Widget *getWidgetAtPoint(int xPos, int yPos, bool returnRootIfOutside=false) const;

    virtual void setTransparent(bool istrans)=0;

    virtual void focus()=0;
    virtual void unfocus()=0;

    /** Inject a mouse movement event into the Window at the specified position.
     *  \param xPos the position along the x-axis of the mouse movement
     *  \param yPos the position along the y-axis of the mouse movement
     */
    virtual void mouseMoved(int xPos, int yPos)=0;
    /** Inject a mouse button event into the Window.
     *  \param buttonID index of the button that caused the event
     *  \param down if true, indicates the mouse button was pressed, if false
     *         indicates it was released
     */
    virtual void mouseButton(unsigned int buttonID, bool down)=0;
    /** Inject a mouse wheel scroll event into the Window.
     *  \param xScroll amount scrolled horizontally
     *  \param yScroll amount scrolled vertically
     */
    virtual void mouseWheel(int xScroll, int yScroll)=0;

    /** Inject a text event into the Window.
     *  \param evt pointer to text string to inject
     *  \param evtLength length of string
     */
    virtual void textEvent(const wchar_t *evt, size_t evtLength)=0;

    /** Inject an individual key event into the Window.
     *  \param pressed if true indicates the key was pressed, if false indicates
     *         it was released
     *  \param mods a modifier code created by a logical or of KeyModifiers
     *  \param vk_code the virtual key code received from the OS
     *  \param scancode the original scancode that generated the event
     */
    virtual void keyEvent(bool pressed, int mods, int vk_code, int scancode)=0;


    /** Resize the Window.
     *  \param width the new width
     *  \param height the new height
     */
    virtual void resize(int width, int height)=0;

    /** Execute Javascript in the context of the Window. This is equivalent to
     *  executing Javascript in the address bar of a regular browser. The
     *  javascript is copied so the caller retains ownership of the string.
     *  \param javascript pointer to a string containing Javascript code
     *  \param javascriptLength length of the Javascript string
     */
    virtual void executeJavascript(const wchar_t* javascript, size_t javascriptLength)=0;

    /** Request navigation to a URL.  The URL string is copied so the caller
     *  retains ownership of the string.
     *  \param url pointer to an ASCII string containing a URL
     *  \param urlLength the length of the URL string
     */
    virtual bool navigateTo(const char *url, size_t urlLength)=0;

    /** Request that the page be reloaded. */
    virtual void refresh()=0;

    /** Cut the currently selected data to the clipboard. */
    virtual void cut()=0;
    /** Copy the currently selected data to the clipboard. */
    virtual void copy()=0;
    /** Paste the current clipboard contents to the current cursoor position in
     *  the Window.
     */
    virtual void paste()=0;

    /** Request the last action be undone. */
    virtual void undo()=0;
    /** Request the last undone action be reperformed. */
    virtual void redo()=0;

    /** Request deletion action. */
    virtual void del()=0;

    /** Request all data be selected. */
    virtual void selectAll()=0;

protected:
    Context *mContext;
    WindowDelegate *mDelegate;

    WidgetList mWidgets;
};

}

#endif
