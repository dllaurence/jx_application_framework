/******************************************************************************
 JXDisplay.cpp

	Represents a single X Display.  We consider a display to be an X server
	plus a particular screen.

	Since cursors should be the same on every display, but each X server will
	provide different cursor id's, we hide cursors behind JCursorIndex.

	Cursors are bulky and difficult to compare, so we require that each
	one be given a unique name.  This way, every client that needs a particular
	cursor can use GetCursor() with only the name.  Only if GetCursor()
	fails does one of the cursor creation routines need to be called.  There
	are two such routines because X makes a distinction between the default
	cursor font and custom cursors.

	Cursor names can be anything, as long as they don't conflict.  All objects
	that use a particular cursor must know the name.  Since the X cursor font
	can be used by all objects, it is suggested that one uses the name of
	the associated XC_* constant as the cursor name.

	We cannot sort the cursor array because the indices that we give out must
	remain valid.  Appending new items is the only option.  This is not much
	of a problem, however, both because the most common cursors have predefined
	indices, and because looking up cursors only has to be done once in each
	constructor.

	Because we are receiving events over a network which introduces an arbitrary
	delay between the occurrence of an event and our processing of it,
	mouse and keyboard events can be reported for the wrong (original) window
	while we are switching a grab from one window to another.  (e.g. The user
	releases the mouse so soon after we switch the grab that the server hasn't
	received our message and therefore thinks that the original window is still
	grabbing.  We therefore get the ButtonRelease for the original window
	instead of for the new window.)  Our solution is to tell JXDisplay about
	who has grabbed what so JXDisplay can re-route the message to the current
	grabber.  This is what itsMouseGrabber and itsKeyboardGrabber are used for.

	BASE CLASS = virtual JBroadcaster

	Copyright � 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDisplay.h>
#include <JXGC.h>
#include <JXColormap.h>
#include <JXWindow.h>
#include <JXWindowDirector.h>
#include <JXFontManager.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXMenuManager.h>
#include <JXWDManager.h>
#include <JXImageCache.h>
#include <JXCursor.h>
#include <jXGlobals.h>
#include <jXKeysym.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xproto.h>		// for error request codes
#ifdef _J_HAS_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#include <sstream>
#include <jAssert.h>

const JSize kMaxSleepTime = 50;		// 0.05 seconds (in milliseconds)

static const JCharacter* kStandardXAtomNames[ JXDisplay::kStandardXAtomCount ] =
{
	"WM_STATE",
	"WM_CLIENT_MACHINE",
	"WM_PROTOCOLS",
	"WM_DELETE_WINDOW",
	"WM_SAVE_YOURSELF",
	"_NET_WM_PING",
	"_NET_WM_PID",
	"_NET_WM_USER_TIME",
	"_NET_ACTIVE_WINDOW",
	"_NET_WM_DESKTOP",
	"_NET_CURRENT_DESKTOP",

	// http://standards.freedesktop.org/wm-spec/latest/ar01s05.html

	"_NET_WM_WINDOW_TYPE",
	"_NET_WM_WINDOW_TYPE_NORMAL",
	"_NET_WM_WINDOW_TYPE_DIALOG",
	"_NET_WM_WINDOW_TYPE_DROPDOWN_MENU",
	"_NET_WM_WINDOW_TYPE_POPUP_MENU",
	"_NET_WM_WINDOW_TYPE_TOOLTIP",

	// private

	"JXWindowManagerBehaviorV0"
};

// prototypes

static int JXDebugAfterFunction(Display* xDisplay);

// JBroadcaster message types

const JCharacter* JXDisplay::kXEventMessage = "XEventMessage::JXDisplay";
const JCharacter* JXDisplay::kXError        = "XError::JXDisplay";

/******************************************************************************
 Constructor function (static)

	By forcing everyone to use this function, we avoid having to worry
	about XOpenDisplay succeeding within the class itself.

	Note that this prevents one from creating derived classes.

 ******************************************************************************/

JBoolean
JXDisplay::Create
	(
	const JCharacter*	displayName,
	JXDisplay**			display
	)
{
	const JCharacter* name = displayName;
	if (JStringEmpty(name))
		{
		name = NULL;
		}

	Display* xDisplay = XOpenDisplay(name);
	if (xDisplay != NULL)
		{
		*display = new JXDisplay(XDisplayName(name), xDisplay);
		return JConvertToBoolean( *display != NULL );
		}
	else
		{
		*display = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

#ifdef _MSC_VER
#pragma warning ( disable : 4355 )
#endif

JXDisplay::JXDisplay
	(
	const JCharacter*	displayName,
	Display*			xDisplay
	)
	:
	itsName(displayName),
	itsMaxStringLength( 4*(XMaxRequestSize(xDisplay)-3) * 4/5 ),
	itsLatestKeyModifiers(this)
{
	itsXDisplay             = xDisplay;
	itsColormap             = JXColormap::Create(this);
	itsLastEventTime        = 0;
	itsLastIdleTime         = 0;
	itsLastMotionNotifyTime = 0;

	itsBounds                        = NULL;
	itsShrinkDisplayToScreenRefCount = 0;

	itsModifierKeymap = NULL;
	UpdateModifierMapping();

	itsCursorList = new JArray<CursorInfo>;
	assert( itsCursorList != NULL );

	itsWindowList = new JArray<WindowInfo>(10);
	assert( itsWindowList != NULL );
	itsWindowList->SetCompareFunction(CompareXWindows);

	itsDefaultGC = new JXGC(this, GetRootWindow());
	assert( itsDefaultGC != NULL );

	itsNeedsUpdateFlag = kJFalse;
	itsMouseContainer  = NULL;
	itsMouseGrabber    = NULL;
	itsKeyboardGrabber = NULL;

	assert( kStandardXAtomCount == (sizeof(kStandardXAtomNames) / sizeof(JCharacter*)) );
	RegisterXAtoms(kStandardXAtomCount, kStandardXAtomNames, itsStandardXAtoms);

	CreateBuiltInCursor("XC_left_ptr", XC_left_ptr);
	CreateBuiltInCursor("XC_xterm",    XC_xterm);
	CreateBuiltInCursor("XC_tcross",   XC_tcross);
	CreateBuiltInCursor("XC_watch",    XC_watch);
	CreateBuiltInCursor("XC_X_cursor", XC_X_cursor);

	itsFontManager = new JXFontManager(this);
	assert( itsFontManager != NULL );

	itsSelectionManager = new JXSelectionManager(this);
	assert( itsSelectionManager != NULL );

	itsDNDManager = new JXDNDManager(this);
	assert( itsDNDManager != NULL );

	itsMenuManager = new JXMenuManager;
	assert( itsMenuManager != NULL );

	itsWDManager = NULL;

	itsImageCache = new JXImageCache(this);
	assert( itsImageCache != NULL );

	int major_opcode, first_event, first_error;
	itsIsOSXFlag = JI2B(XQueryExtension(itsXDisplay, "Apple-WM",
										&major_opcode, &first_event, &first_error));

	(JXGetApplication())->DisplayOpened(this);

	if (_Xdebug)
		{
		XSetAfterFunction(itsXDisplay, JXDebugAfterFunction);
		}

	if (!itsWMBehavior.Load(this))
		{
		JXWindow::AnalyzeWindowManager(this);
		itsWMBehavior.Save(this);
		}
}

/******************************************************************************
 Destructor

	We should only be deleted after all windows have been closed.

 ******************************************************************************/

JXDisplay::~JXDisplay()
{
	assert( itsWindowList->IsEmpty() );

	(JXGetApplication())->DisplayClosed(this);

	delete itsWDManager;
	delete itsMenuManager;
	delete itsSelectionManager;
	delete itsDNDManager;
	delete itsFontManager;
	delete itsImageCache;

	delete itsWindowList;
	delete itsDefaultGC;
	delete itsColormap;
	delete itsBounds;

	const JSize count = itsCursorList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CursorInfo info = itsCursorList->GetElement(i);
		delete (info.name);
		XFreeCursor(itsXDisplay, info.xid);
		}
	delete itsCursorList;

	XFreeModifiermap(itsModifierKeymap);

	XCloseDisplay(itsXDisplay);
}

/******************************************************************************
 Close

	This is the safe way to delete a JXDisplay.

 ******************************************************************************/

JBoolean
JXDisplay::Close()
{
	while (!itsWindowList->IsEmpty())
		{
		WindowInfo info = itsWindowList->GetLastElement();
		if (!((info.window)->GetDirector())->Close())
			{
			return kJFalse;
			}
		}

	delete this;
	return kJTrue;
}

/******************************************************************************
 RaiseAllWindows

	Raises all visible, non-iconified windows and maintains the stacking order.

 ******************************************************************************/

void
JXDisplay::RaiseAllWindows()
{
JIndex i;

	// get list of all top level windows

	Window root, parent, *childList;
	unsigned int childCount;
	if (!XQueryTree(itsXDisplay, GetRootWindow(),
				   &root, &parent, &childList, &childCount))
		{
		return;
		}

	// initialize the mapping of X windows to JXWindows

	JPtrArray<JXWindow> childMapping(JPtrArrayT::kForgetAll, childCount);
	for (i=1; i<=childCount; i++)
		{
		childMapping.Append(NULL);
		}

	// fill in the mapping of X windows to JXWindows

	const JSize windowCount = itsWindowList->GetElementCount();
	for (i=1; i<=windowCount; i++)
		{
		const WindowInfo info = itsWindowList->GetElement(i);
		Window rootChild;
		if ((info.window)->GetRootChild(&rootChild))
			{
			// find the ancestor in the list of top level windows

			for (JIndex j=0; j<childCount; j++)
				{
				if (childList[j] == rootChild)
					{
					childMapping.SetElement(j+1, info.window, JPtrArrayT::kForget);
					break;
					}
				}
			}
		}

	// raise the windows (first one is on the bottom)

	for (i=1; i<=childCount; i++)
		{
		JXWindow* w = childMapping.NthElement(i);
		if (w != NULL && w->IsVisible() && !w->IsIconified())
			{
			w->Raise();
			}
		}

	XFree(childList);
}

/******************************************************************************
 HideAllWindows

	Hide all windows on this display.  Mainly useful when quitting.

 ******************************************************************************/

void
JXDisplay::HideAllWindows()
{
	const JSize count = itsWindowList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		((itsWindowList->GetElement(i)).window)->Hide();
		}
}

/******************************************************************************
 UndockAllWindows

 ******************************************************************************/

void
JXDisplay::UndockAllWindows()
{
	const JSize count = itsWindowList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		((itsWindowList->GetElement(i)).window)->Undock();
		}
}

/******************************************************************************
 CloseAllOtherWindows

	Calls Close() on all windows except the given one.

 ******************************************************************************/

void
JXDisplay::CloseAllOtherWindows
	(
	JXWindow* window
	)
{
	JOrderedSetIterator<WindowInfo> iter(*itsWindowList);

	WindowInfo info;
	while (iter.Next(&info))
		{
		if (info.window != window)
			{
			(info.window)->Close();
			}
		}
}

/******************************************************************************
 WindowExists (static)

	Returns kJTrue if the given window hasn't been deleted.  Since we
	cannot assume that anything exists, everything that we need must
	be passed in.

 ******************************************************************************/

JBoolean
JXDisplay::WindowExists
	(
	JXDisplay*		display,
	const Display*	xDisplay,
	const Window	xWindow
	)
{
	JXApplication* app;
	JXWindow* window;
	return JConvertToBoolean(
			JXGetApplication(&app) && app->DisplayExists(xDisplay) &&
			display->FindXWindow(xWindow, &window));
}

/******************************************************************************
 GetWDManager

 ******************************************************************************/

JXWDManager*
JXDisplay::GetWDManager()
	const
{
	assert( itsWDManager != NULL );
	return itsWDManager;
}

/******************************************************************************
 SetWDManager

 ******************************************************************************/

void
JXDisplay::SetWDManager
	(
	JXWDManager* mgr
	)
{
	assert( itsWDManager == NULL );
	itsWDManager = mgr;
}

/******************************************************************************
 GetFontManager

	Not inline to avoid including <JXFontManager.h> in header file.

 ******************************************************************************/

const JFontManager*
JXDisplay::GetFontManager()
	const
{
	return itsFontManager;
}

/******************************************************************************
 Get resolution

	Screen resolution in pixels/inch (25.4 mm = 1 inch)

 ******************************************************************************/

JSize
JXDisplay::GetHorizResolution()
	const
{
	const int screen = GetScreen();
	return JRound(DisplayWidth(itsXDisplay, screen) /
				  (DisplayWidthMM(itsXDisplay, screen)/25.4));
}

JSize
JXDisplay::GetVertResolution()
	const
{
	const int screen = GetScreen();
	return JRound(DisplayHeight(itsXDisplay, screen) /
				  (DisplayHeightMM(itsXDisplay, screen)/25.4));
}

/******************************************************************************
 GetBounds

 ******************************************************************************/

JRect
JXDisplay::GetBounds()
	const
{
	#ifdef _J_HAS_XINERAMA

	if (itsBounds == NULL && itsShrinkDisplayToScreenRefCount > 0)
		{
		int event_base, error_base;
		if (XineramaQueryExtension(itsXDisplay, &event_base, &error_base) &&
			XineramaIsActive(itsXDisplay))
			{
			int count;
			XineramaScreenInfo* info = XineramaQueryScreens(itsXDisplay, &count);

			itsBounds = new JArray<JRect>(count);
			assert( itsBounds != NULL );

			for (int i=0; i<count; i++)
				{
				JRect r(info[i].y_org,
						info[i].x_org,
						info[i].y_org + info[i].height,
						info[i].x_org + info[i].width);

				if (info[i].x_org == 0 && info[i].y_org == 0)
					{
					itsBounds->PrependElement(r);
					}
				else
					{
					itsBounds->AppendElement(r);
					}
				}

			XFree(info);
			}
		}

	#endif

	if (itsBounds == NULL)
		{
		int x,y;
		unsigned int width, height, borderWidth, depth;
		Window rootWindow;
		const Status ok =
			XGetGeometry(itsXDisplay, GetRootWindow(), &rootWindow, &x, &y,
						 &width, &height, &borderWidth, &depth);
		assert( ok );

		itsBounds = new JArray<JRect>(1);
		assert( itsBounds != NULL );
		itsBounds->AppendElement(JRect(y, x, y+height, x+width));
		}

	// The location of the mouse is a good guess as to the location of the
	// user's attention.  Treat the screen that contains this location as
	// the root "window"

	const JSize count = itsBounds->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JRect r = itsBounds->GetElement(i);
		if (r.Contains(itsLatestMouseLocation))
			{
			return r;
			}
		}

	return itsBounds->GetFirstElement();
}

/******************************************************************************
 Placing windows on active screen

 ******************************************************************************/

void
JXDisplay::ShrinkDisplayBoundsToActiveScreen()
{
	itsShrinkDisplayToScreenRefCount++;
	delete itsBounds;
	itsBounds = NULL;
}

void
JXDisplay::RestoreDisplayBounds()
{
	assert( itsShrinkDisplayToScreenRefCount > 0 );

	itsShrinkDisplayToScreenRefCount--;
	if (itsShrinkDisplayToScreenRefCount == 0)
		{
		delete itsBounds;
		itsBounds = NULL;
		}
}

/******************************************************************************
 GetCurrentButtonKeyState

	Call these if you need to get information more current than the latest
	event.  This is useful if you need the state of the Shift key because
	pressing Shift doesn't generate an event.

	buttonStates and/or modifiers can be NULL.

 ******************************************************************************/

void
JXDisplay::GetCurrentButtonKeyState
	(
	JXButtonStates* buttonStates,
	JXKeyModifiers* modifiers
	)
	const
{
	Window root, child;
	int root_x, root_y, win_x, win_y;
	unsigned int state;
	XQueryPointer(itsXDisplay, GetRootWindow(), &root, &child, &root_x, &root_y,
				  &win_x, &win_y, &state);

	if (buttonStates != NULL)
		{
		buttonStates->SetState(state);
		}

	if (modifiers != NULL)
		{
		modifiers->SetState(this, state);
		}
}

/******************************************************************************
 UpdateModifierMapping (private)

 ******************************************************************************/

// same order as JXModifierKey enum in JXKeyModifiers.h

static const KeySym kModifierKeySymList[] =
{
	XK_Num_Lock,    0,
	XK_Scroll_Lock, 0,
	XK_Meta_L,      XK_Meta_R,
	XK_Alt_L,       XK_Alt_R,
	XK_Super_L,     XK_Super_R,
	XK_Hyper_L,     XK_Hyper_R
};

const JSize kModifierKeySymCount = sizeof(kModifierKeySymList)/(2*sizeof(KeySym));

void
JXDisplay::UpdateModifierMapping()
{
JIndex i;

	if (itsModifierKeymap != NULL)
		{
		XFreeModifiermap(itsModifierKeymap);
		}
	itsModifierKeymap = XGetModifierMapping(itsXDisplay);

	itsJXKeyModifierMapping[0] = 0;		// might as well initialize

	for (i=1; i<=kXModifierCount; i++)
		{
		itsJXKeyModifierMapping[i] = i;
		}

	assert( kModifierKeySymCount == kJXKeyModifierMapCount-kXModifierCount );

	for (i=1; i<=kModifierKeySymCount; i++)
		{
		JIndex j;
		if (!KeysymToModifier(kModifierKeySymList[2*i-2], &j) &&
			(kModifierKeySymList[2*i-1] == 0 ||
			 !KeysymToModifier(kModifierKeySymList[2*i-1], &j)))
			{
			j = 0;
			}
		itsJXKeyModifierMapping [ kXModifierCount+i ] = j;

//		cout << "modifier " << kXModifierCount+i << " mapped to " << j << endl;
		}
//	cout << endl;

	// some Linux distributions (Mandrake) map only Alt modifier, not Meta

	if (itsJXKeyModifierMapping[kJXMetaKeyIndex] == 0)
		{
		itsJXKeyModifierMapping [ kJXMetaKeyIndex ] =
			itsJXKeyModifierMapping [ kJXAltKeyIndex ];
		}
}

/******************************************************************************
 KeysymToModifier

	Converts an X keysym into an X modifier index.

 ******************************************************************************/

JBoolean
JXDisplay::KeysymToModifier
	(
	const KeySym	keysym,
	JIndex*			modifierIndex
	)
	const
{
	return KeycodeToModifier(XKeysymToKeycode(itsXDisplay, keysym), modifierIndex);
}

/******************************************************************************
 KeycodeToModifier

	Converts an X keycode into an X modifier index.

 ******************************************************************************/

JBoolean
JXDisplay::KeycodeToModifier
	(
	const KeyCode	keycode,
	JIndex*			modifierIndex
	)
	const
{
	*modifierIndex = 0;
	if (keycode == 0)
		{
		return kJFalse;
		}

	const JSize maxKeyPerMod = itsModifierKeymap->max_keypermod;
	for (JIndex i=0; i<kXModifierCount; i++)
		{
		const KeyCode* modifier =
			itsModifierKeymap->modifiermap + i * maxKeyPerMod;

		for (JIndex j=0; j<maxKeyPerMod; j++)
			{
			if (modifier[j] == keycode)
				{
				*modifierIndex = i+1;
				return kJTrue;
				}
			}
		}

	return kJFalse;
}

/******************************************************************************
 CreateBuiltInCursor

 ******************************************************************************/

JCursorIndex
JXDisplay::CreateBuiltInCursor
	(
	const JCharacter*	name,
	const unsigned int	shape
	)
{
	JCursorIndex index;
	if (GetCursor(name, &index))
		{
		return index;
		}

	CursorInfo info;

	info.name = new JString(name);
	assert( info.name != NULL );

	info.xid = XCreateFontCursor(itsXDisplay, shape);

	itsCursorList->AppendElement(info);
	return itsCursorList->GetElementCount();
}

/******************************************************************************
 CreateCustomCursor

 ******************************************************************************/

JCursorIndex
JXDisplay::CreateCustomCursor
	(
	const JCharacter*	name,
	const JXCursor&		cursor
	)
{
	JCursorIndex index;
	if (GetCursor(name, &index))
		{
		return index;
		}

	CursorInfo info;

	info.name = new JString(name);
	assert( info.name != NULL );

	info.xid = CreateCustomXCursor(cursor);

	itsCursorList->AppendElement(info);
	return itsCursorList->GetElementCount();
}

/******************************************************************************
 CreateCustomXCursor (private)

 ******************************************************************************/

Cursor
JXDisplay::CreateCustomXCursor
	(
	const JXCursor& cursor
	)
	const
{
	XColor foreColor;
	foreColor.pixel = itsColormap->GetBlackColor();
	foreColor.red = foreColor.green = foreColor.blue = 0;
	foreColor.flags = DoRed | DoGreen | DoBlue;

	XColor backColor;
	backColor.pixel = itsColormap->GetWhiteColor();
	backColor.red = backColor.green = backColor.blue = 65535;
	backColor.flags = DoRed | DoGreen | DoBlue;

	Pixmap sourcePixmap =
		XCreateBitmapFromData(itsXDisplay, GetRootWindow(),
							  cursor.data, cursor.w, cursor.h);
	assert( sourcePixmap != 0 );

	Pixmap maskPixmap =
		XCreateBitmapFromData(itsXDisplay, GetRootWindow(),
							  cursor.mask, cursor.w, cursor.h);
	assert( maskPixmap != 0 );

	const Cursor cursorID =
		XCreatePixmapCursor(itsXDisplay, sourcePixmap, maskPixmap,
							&foreColor, &backColor, cursor.hotx, cursor.hoty);

	XFreePixmap(itsXDisplay, sourcePixmap);
	XFreePixmap(itsXDisplay, maskPixmap);

	return cursorID;
}

/******************************************************************************
 GetCursor

	Returns kJTrue if a cursor with the given name has been created.

 ******************************************************************************/

JBoolean
JXDisplay::GetCursor
	(
	const JCharacter*	name,
	JCursorIndex*		index
	)
	const
{
	const JSize count = itsCursorList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CursorInfo info = itsCursorList->GetElement(i);
		if (*(info.name) == name)
			{
			*index = i;
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 DisplayCursorInAllWindows

	Displays the specified cursor in all windows on this display.
	Mainly useful for displaying the watch cursor or the inactive cursor.

 ******************************************************************************/

void
JXDisplay::DisplayCursorInAllWindows
	(
	const JCursorIndex index
	)
{
	const JSize count = itsWindowList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		WindowInfo info = itsWindowList->GetElement(i);
		(info.window)->DisplayXCursor(index);
		}

	// Since this is usually called before a blocking process,
	// we need to make sure that the X server gets the messages.

	Flush();
}

/******************************************************************************
 ColormapChanged

	Update the colormap for each window.

 ******************************************************************************/

void
JXDisplay::ColormapChanged
	(
	JXColormap*	colormap
	)
{
	assert( colormap == itsColormap );

	const JSize windowCount = itsWindowList->GetElementCount();
	for (JIndex i=1; i<=windowCount; i++)
		{
		WindowInfo info = itsWindowList->GetElement(i);
		(info.window)->ColormapChanged(itsColormap);
		}
}

/******************************************************************************
 HandleEvent

	JXSelectionManager handles all the selection events.
	All other events go to the appropriate window.

 ******************************************************************************/

void
JXDisplay::HandleEvent
	(
	const XEvent&	xEvent,
	const Time		currentTime
	)
{
	// update event time (mainly for JXSelectionManager::BecomeOwner)

	Time time;
	if (JXGetEventTime(xEvent, &time))
		{
		itsLastEventTime = time;
		}

	if (xEvent.type == MotionNotify)
		{
		itsLastMotionNotifyTime = currentTime;
		}
//	else if (xEvent.type == Expose)
//		{
//		itsLastIdleTime = 0;
//		}

	// save button and key modifier states

	unsigned int state;
	if (JXGetButtonAndModifierStates(xEvent, this, &state))
		{
		itsLatestButtonStates.SetState(state);
		itsLatestKeyModifiers.SetState(this, state);
		}

	JPoint mousePt;
	if (JXGetMouseLocation(xEvent, this, &mousePt))
		{
		itsLatestMouseLocation = mousePt;
		}

	// handle event

	if (xEvent.type == MappingNotify)
		{
		if (xEvent.xmapping.request == MappingModifier)
			{
			UpdateModifierMapping();
			}
		else if (xEvent.xmapping.request == MappingKeyboard)
			{
			XRefreshKeyboardMapping(const_cast<XMappingEvent*>(&(xEvent.xmapping)));
			}
		}

	else if (xEvent.type == SelectionRequest)
		{
		itsSelectionManager->HandleSelectionRequest(xEvent.xselectionrequest);
		}
	else if (xEvent.type == SelectionClear)
		{
		itsSelectionManager->ClearData(xEvent.xselectionclear.selection,
									   xEvent.xselectionclear.time);
		}
	else if (xEvent.type == SelectionNotify)
		{
		// JXSelectionManager catches these events
		}

	else if (xEvent.type == ClientMessage &&
			 itsDNDManager->HandleClientMessage(xEvent.xclient))
		{
		// JXDNDManager handled it if it returns kJTrue
		}
	else if (xEvent.type == DestroyNotify &&
			 itsDNDManager->HandleDestroyNotify(xEvent.xdestroywindow))
		{
		// JXDNDManager handled it if it returns kJTrue
		}

	else if ((xEvent.type == ButtonPress || xEvent.type == ButtonRelease) &&
			 itsMouseGrabber != NULL)
		{
		XEvent fixedEvent         = xEvent;
		XButtonEvent& buttonEvent = fixedEvent.xbutton;
		if (buttonEvent.window != itsMouseGrabber->GetXWindow())
			{
			const JPoint ptG =
				itsMouseGrabber->RootToGlobal(buttonEvent.x_root, buttonEvent.y_root);
			buttonEvent.window    = itsMouseGrabber->GetXWindow();
			buttonEvent.subwindow = None;
			buttonEvent.x         = ptG.x;
			buttonEvent.y         = ptG.y;
			}
		itsMouseGrabber->HandleEvent(fixedEvent);
		}
	else if (xEvent.type == MotionNotify && itsMouseGrabber != NULL)
		{
		// calls XQueryPointer() for itsXWindow so we don't care which window
		// it is reported relative to

		itsMouseGrabber->HandleEvent(xEvent);
		}
	else if ((xEvent.type == KeyPress || xEvent.type == KeyRelease) &&
			 itsKeyboardGrabber != NULL)
		{
		// ignores mouse coordinates so we don't care which window
		// it is reported relative to

		itsKeyboardGrabber->HandleEvent(xEvent);
		}

	else
		{
		JXWindow* window;
		if (FindXWindow(xEvent.xany.window, &window))
			{
			window->HandleEvent(xEvent);
			}
		else
			{
			XEventMessage msg(xEvent);
			BroadcastWithFeedback(&msg);
			}
		}
}

/******************************************************************************
 Idle

	Redraw our windows and send a fake motion event.

	If the mouse is not pressed, we send idle events relatively slowly so
	that we don't hog CPU time.

	When the mouse is pressed, we send one at every opportunity because
	the user presumably doesn't care how much CPU time we use.

 ******************************************************************************/

void
JXDisplay::Idle
	(
	const Time currentTime
	)
{
	if (currentTime - itsLastIdleTime > kMaxSleepTime ||
		!itsLatestButtonStates.AllOff())
		{
		Update();
		itsLastIdleTime = currentTime;

		if (currentTime - itsLastMotionNotifyTime > kMaxSleepTime)
			{
			DispatchMouse();
			itsLastMotionNotifyTime = currentTime;
			}
		}
}

/******************************************************************************
 Update

	Give each window a chance to redraw itself.  Since this routine is
	an idle task, we use a single flag to make sure that it doesn't do
	more work than it has to.

 ******************************************************************************/

void
JXDisplay::Update()
{
	if (itsNeedsUpdateFlag)
		{
		itsNeedsUpdateFlag = kJFalse;	// clear first, in case redraw triggers update

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const WindowInfo info = itsWindowList->GetElement(i);
			(info.window)->Update();
			}
		}
}

/******************************************************************************
 DispatchMouse

 ******************************************************************************/

void
JXDisplay::DispatchMouse()
{
	if (itsMouseContainer != NULL)
		{
		itsMouseContainer->DispatchMouse();
		}
}

/******************************************************************************
 DispatchCursor

 ******************************************************************************/

void
JXDisplay::DispatchCursor()
{
	if (itsMouseContainer != NULL)
		{
		itsMouseContainer->DispatchCursor();
		}
}

/******************************************************************************
 FindMouseContainer

	Returns kJTrue if the mouse is in one of our windows.

	Regardless of the return value, if xWindow is not NULL, it is set to the
	id of the X window that the cursor is in.  ptG is set to the mouse
	coordinates relative to xWindow, and ptR contains the mouse coordinates
	relative to the root window.

 ******************************************************************************/

JBoolean
JXDisplay::FindMouseContainer
	(
	JXContainer**	obj,
	Window*			xWindow,
	JPoint*			ptG,
	JPoint*			ptR
	)
	const
{
	Window rootWindow, childWindow;
	int root_x, root_y, x,y;
	unsigned int state;
	if (XQueryPointer(itsXDisplay, GetRootWindow(), &rootWindow, &childWindow,
					  &root_x, &root_y, &x, &y, &state) &&
		childWindow != None)
		{
		return FindMouseContainer(rootWindow, root_x, root_y,
								  childWindow, obj, xWindow, ptG, ptR);
		}
	else
		{
		if (xWindow != NULL)
			{
			*xWindow = rootWindow;
			}
		if (ptG != NULL)
			{
			ptG->x = x;
			ptG->y = y;
			}
		if (ptR != NULL)
			{
			ptR->x = root_x;
			ptR->y = root_y;
			}
		*obj = NULL;
		return kJFalse;
		}
}

JBoolean
JXDisplay::FindMouseContainer
	(
	const JXContainer*	coordOwner,
	const JPoint&		pt,
	JXContainer**		obj,
	Window*				xWindow,
	JPoint*				resultPtG,
	JPoint*				resultPtR
	)
	const
{
	const Window startWindow = (coordOwner->GetWindow())->GetXWindow();
	const JPoint ptG         = coordOwner->LocalToGlobal(pt);

	const Window rootWindow = GetRootWindow();
	Window childWindow;
	int x,y;
	if (XTranslateCoordinates(itsXDisplay, startWindow, rootWindow,
							  ptG.x, ptG.y, &x, &y, &childWindow) &&
		childWindow != None)
		{
		return FindMouseContainer(rootWindow, x,y, childWindow,
								  obj, xWindow, resultPtG, resultPtR);
		}
	else
		{
		if (xWindow != NULL)
			{
			*xWindow = rootWindow;
			}
		if (resultPtG != NULL)
			{
			resultPtG->x = x;
			resultPtG->y = y;
			}
		if (resultPtR != NULL)
			{
			resultPtR->x = x;
			resultPtR->y = y;
			}
		*obj = NULL;
		return kJFalse;
		}
}

// private

JBoolean
JXDisplay::FindMouseContainer
	(
	const Window		rootWindow,
	const JCoordinate	xRoot,
	const JCoordinate	yRoot,
	const Window		origChildWindow,
	JXContainer**		obj,
	Window*				xWindow,
	JPoint*				ptG,
	JPoint*				ptR
	)
	const
{
	Window window1 = rootWindow;
	Window window2 = origChildWindow;
	Window childWindow;
	int x1 = xRoot, y1 = yRoot, x2,y2;
	while (XTranslateCoordinates(itsXDisplay, window1, window2,
								 x1, y1, &x2, &y2, &childWindow) &&
		   childWindow != None)
		{
		window1 = window2;
		window2 = childWindow;
		x1      = x2;
		y1      = y2;
		}

	if (xWindow != NULL)
		{
		*xWindow = window2;
		}
	if (ptG != NULL)
		{
		ptG->x = x2;
		ptG->y = y2;
		}
	if (ptR != NULL)
		{
		ptR->x = xRoot;
		ptR->y = yRoot;
		}

	JXWindow* window;
	if (FindXWindow(window2, &window))
		{
		return window->FindContainer(JPoint(x2,y2), obj);
		}
	else
		{
		*obj = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 SwitchDrag

	Switch the current mouse drag messages from one widget to another.

 ******************************************************************************/

JBoolean
JXDisplay::SwitchDrag
	(
	JXContainer*			fromObj,
	const JPoint&			fromObjPt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	JXContainer*			toObj
	)
{
	JXWindow* fromWindow = fromObj->GetWindow();
	const JPoint fromPtG = fromObj->LocalToGlobal(fromObjPt);

	fromWindow->EndDrag(fromObj, fromPtG, buttonStates, modifiers);

	JXWindow* toWindow = toObj->GetWindow();
	JPoint toPtG;
	if (toWindow != fromWindow)
		{
		const JPoint ptR = fromWindow->GlobalToRoot(fromPtG);
		toPtG = toWindow->RootToGlobal(ptR);
		}
	else
		{
		toPtG = fromPtG;
		}

	return toWindow->BeginDrag(toObj, toPtG, buttonStates, modifiers);
}

/******************************************************************************
 WindowCreated

	Insert the new window so that the list remains sorted by xWindow values.

 ******************************************************************************/

void
JXDisplay::WindowCreated
	(
	JXWindow*		window,
	const Window	xWindow
	)
{
	const WindowInfo newInfo(window, xWindow);
	JIndex i;
	if (itsWindowList->InsertSorted(newInfo, kJFalse, &i) && i == 1)
		{
		window->AcceptSaveYourself(kJTrue);
		if (itsWindowList->GetElementCount() > 1)
			{
			((itsWindowList->GetElement(2)).window)->AcceptSaveYourself(kJFalse);
			}
		}
}

/******************************************************************************
 WindowDeleted

 ******************************************************************************/

void
JXDisplay::WindowDeleted
	(
	JXWindow* window
	)
{
	if (itsMouseContainer == window)
		{
		itsMouseContainer = NULL;
		}
	if (itsMouseGrabber == window)
		{
		itsMouseGrabber = NULL;
		}
	if (itsKeyboardGrabber == window)
		{
		itsKeyboardGrabber = NULL;
		}

	const JSize count = itsWindowList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const WindowInfo info = itsWindowList->GetElement(i);
		if (info.window == window)
			{
			itsWindowList->RemoveElement(i);
			if (i == 1 && count > 1)
				{
				((itsWindowList->GetElement(1)).window)->AcceptSaveYourself(kJTrue);
				}
			break;
			}
		}
}

/******************************************************************************
 FindXWindow

	Returns kJTrue if the given xWindow is registered.
	Since the window list is sorted by xWindow, we use an O(log(N)) binary search.

	We can't call it FindWindow() because Windows #define's FindWindow to
	FindWindowA.

 ******************************************************************************/

JBoolean
JXDisplay::FindXWindow
	(
	const Window	xWindow,
	JXWindow**		window
	)
	const
{
	WindowInfo target(NULL, xWindow);
	JIndex i;
	if (itsWindowList->SearchSorted(target, JOrderedSetT::kAnyMatch, &i))
		{
		target  = itsWindowList->GetElement(i);
		*window = target.window;
		return kJTrue;
		}
	else
		{
		*window = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 CompareXWindows (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
JXDisplay::CompareXWindows
	(
	const WindowInfo& info1,
	const WindowInfo& info2
	)
{
	if (info1.xWindow < info2.xWindow)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (info1.xWindow == info2.xWindow)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 XErrorHandler (static)

	Errors reported by the X Server are translated into XError messages
	and BroadcastWithFeedback() by the appropriate JXDisplay to anybody who
	cares to listen.  If somebody catches the error and deals with it, they
	*must* call SetCaught() to notify us.  If nobody calls SetCaught(), we
	abort, causing a core dump.

	The messages are broadcast as an urgent task to avoid reentrancy problems.

	The one exception is BadWindow errors for windows that we do not
	own.  These errors must be ignored to avoid horrendous amounts of
	error checking in JXSelectionManager and JXDNDManager.

	*** X restricts the actions that can be taken while handling an error:
		You cannot contact the X Server.

 ******************************************************************************/

static JArray<XErrorEvent> theXErrorList;

int
JXDisplay::JXErrorHandler
	(
	Display*		xDisplay,
	XErrorEvent*	error
	)
{
	theXErrorList.AppendElement(*error);
	return Success;
}

void
JXDisplay::CheckForXErrors()
{
	if (!theXErrorList.IsEmpty())
		{
		// we need to get count every time since extra XErrors could be generated

		for (JIndex i=1; i<=theXErrorList.GetElementCount(); i++)
			{
			const XErrorEvent error = theXErrorList.GetElement(i);

			JXDisplay* display;
			const JBoolean found = (JXGetApplication())->FindDisplay(error.display, &display);
			assert( found );

			XError msg(error);
			display->BroadcastWithFeedback(&msg);
			if (!msg.WasCaught())
				{
				JXWindow* w = NULL;
				if (msg.GetType() == BadWindow && msg.GetXID() != None &&
					!display->FindXWindow(msg.GetXID(), &w))
					{
					// not our window -- probably residual from selection or DND
					continue;
					}
				else if (msg.GetType() == BadMatch &&
						 error.request_code == X_SetInputFocus)
					{
					// Never die on anything as silly as an XSetInputFocus() error,
					// but still useful to know if it happens.
//					cerr << "Illegal call to XSetInputFocus()" << endl;
					continue;
					}

				cerr << "An unexpected XError occurred!" << endl;

				char str[80];
				XGetErrorText(error.display, error.error_code, str, 80);
				cerr << "Error code: " << str << endl;

				JString reqCodeStr(error.request_code, 0);
				XGetErrorDatabaseText(error.display, "XRequest", reqCodeStr, "unknown", str, 80);
				cerr << "Offending request: " << str << endl;

				assert( 0 /* unexpected XError */ );
				}
			}

		theXErrorList.RemoveAll();
		}
}

/******************************************************************************
 JXDebugAfterFunction (static)

	This generates a core dump immediately when an X error ocurrs, instead
	of at some unspecified time in the future.

 ******************************************************************************/

int
JXDebugAfterFunction
	(
	Display* xDisplay
	)
{
	XSync(xDisplay, False);
	JXDisplay::CheckForXErrors();
	return True;
}

/******************************************************************************
 WMBehavior::Load

 ******************************************************************************/

JBoolean
JXDisplay::WMBehavior::Load
	(
	JXDisplay* display
	)
{
	JBoolean success = kJFalse;

	const Atom atom = display->itsStandardXAtoms[ kJXWMBehaviorV0XAtomIndex ];

	Atom actualType;
	int actualFormat;
	unsigned long itemCount, remainingBytes;
	unsigned char* data = NULL;
	XGetWindowProperty(*display, display->GetRootWindow(), atom,
					   0, LONG_MAX, False, atom,
					   &actualType, &actualFormat,
					   &itemCount, &remainingBytes, &data);

	if (actualType == atom && actualFormat == 8 && itemCount > 0)
		{
		const std::string s(reinterpret_cast<char*>(data), itemCount);
		std::istringstream input(s);
		success = Read(input, 0);
		}

	XFree(data);
	return success;
}

/******************************************************************************
 WMBehavior::Save

 ******************************************************************************/

void
JXDisplay::WMBehavior::Save
	(
	JXDisplay* display
	)
	const
{
	// version 0

	std::ostringstream dataV0;
	WriteV0(dataV0);
	const std::string s = dataV0.str();

	const Atom atom = display->itsStandardXAtoms[ kJXWMBehaviorV0XAtomIndex ];
	XChangeProperty(*display, display->GetRootWindow(),
					atom, atom, 8, PropModeReplace,
					(unsigned char*) s.c_str(), s.length());
}

/******************************************************************************
 WMBehavior::Read (private)

 ******************************************************************************/

JBoolean
JXDisplay::WMBehavior::Read
	(
	istream&			input,
	const JFileVersion	vers
	)
{
	if (vers > 0)
		{
		return kJFalse;
		}

	input >> desktopMapsWindowsFlag;
	input >> frameCompensateFlag;
	input >> reshowOffset;

	return kJTrue;
}

/******************************************************************************
 WMBehavior::WriteV0 (private)

 ******************************************************************************/

void
JXDisplay::WMBehavior::WriteV0
	(
	ostream& output
	)
	const
{
	output << desktopMapsWindowsFlag;
	output << ' ' << frameCompensateFlag;
	output << ' ' << reshowOffset;
}

#define JTemplateType JXDisplay::WindowInfo
#include <JArray.tmpls>
#undef JTemplateType

#define JTemplateType JXDisplay::CursorInfo
#include <JArray.tmpls>
#undef JTemplateType

#define JTemplateType XErrorEvent
#include <JArray.tmpls>
#undef JTemplateType

#define JTemplateType JXDisplay
#include <JPtrArray.tmpls>
#undef JTemplateType
