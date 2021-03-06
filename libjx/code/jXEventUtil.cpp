/******************************************************************************
 jXEventUtil.cpp

	Useful functions for dealing with X events.

	Copyright � 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <jXEventUtil.h>
#include <JXDisplay.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 JXIsPrint

	Returns kJTrue if the keysym is between 1 and 255 and isprint() returns
	kJTrue.

 ******************************************************************************/

JBoolean
JXIsPrint
	(
	const int keysym
	)
{
	return JConvertToBoolean( 0 < keysym && keysym <= 255 && JIsPrint(keysym) );
}

/******************************************************************************
 JXGetEventTime

	Return the time stamp of the event.  Returns kJFalse if the given
	event doesn't contain a time stamp.

	Selection events contain a time field, but this is a timestamp generated
	by clients, not the current server time.

 ******************************************************************************/

JBoolean
JXGetEventTime
	(
	const XEvent&	xEvent,
	Time*			time
	)
{
	*time = 0;

	if (xEvent.type == KeyPress || xEvent.type == KeyRelease)
		{
		*time = xEvent.xkey.time;
		return kJTrue;
		}
	else if (xEvent.type == ButtonPress || xEvent.type == ButtonRelease)
		{
		*time = xEvent.xbutton.time;
		return kJTrue;
		}
	else if (xEvent.type == MotionNotify)
		{
		*time = xEvent.xmotion.time;
		return kJTrue;
		}
	else if (xEvent.type == EnterNotify || xEvent.type == LeaveNotify)
		{
		*time = xEvent.xcrossing.time;
		return kJTrue;
		}
	else if (xEvent.type == PropertyNotify)
		{
		*time = xEvent.xproperty.time;
		return kJTrue;
		}
	else
		{
		return kJFalse;		// event doesn't contain the information
		}
}

/******************************************************************************
 JXGetMouseLocation

	Return the location of the mouse in root coordinates.
	Returns kJFalse if the given event doesn't contain the information.

 ******************************************************************************/

JBoolean
JXGetMouseLocation
	(
	const XEvent&	xEvent,
	JXDisplay*		display,
	JPoint*			pt
	)
{
	pt->Set(0, 0);

	if (xEvent.type == KeyPress || xEvent.type == KeyRelease)
		{
		pt->Set(xEvent.xkey.x_root, xEvent.xkey.y_root);
		return kJTrue;
		}
	else if (xEvent.type == ButtonPress || xEvent.type == ButtonRelease)
		{
		pt->Set(xEvent.xbutton.x_root, xEvent.xbutton.y_root);
		return kJTrue;
		}
	else if (xEvent.type == MotionNotify)
		{
		pt->Set(xEvent.xmotion.x_root, xEvent.xmotion.y_root);
		return kJTrue;
		}
	else if (xEvent.type == EnterNotify || xEvent.type == LeaveNotify)
		{
		pt->Set(xEvent.xcrossing.x_root, xEvent.xcrossing.y_root);
		return kJTrue;
		}
	else
		{
		return kJFalse;		// event doesn't contain the information
		}
}

/******************************************************************************
 JXGetButtonAndModifierStates

	Return the button and key modifiers states of the event.
	Returns kJFalse if the given event doesn't contain the information.

 ******************************************************************************/

JBoolean
JXGetButtonAndModifierStates
	(
	const XEvent&	xEvent,
	JXDisplay*		display,
	unsigned int*	state
	)
{
	*state = 0;

	if (xEvent.type == KeyPress)
		{
		*state = xEvent.xkey.state;
		JIndex modifierIndex;
		if (display->KeycodeToModifier(xEvent.xkey.keycode, &modifierIndex))
			{
			*state = JXKeyModifiers::SetState(display, *state, modifierIndex, kJTrue);
			}
		return kJTrue;
		}
	else if (xEvent.type == KeyRelease)
		{
		*state = xEvent.xkey.state;
		JIndex modifierIndex;
		if (display->KeycodeToModifier(xEvent.xkey.keycode, &modifierIndex))
			{
			*state = JXKeyModifiers::SetState(display, *state, modifierIndex, kJFalse);
			}
		return kJTrue;
		}
	else if (xEvent.type == ButtonPress)
		{
		const JXMouseButton currButton = (JXMouseButton) xEvent.xbutton.button;
		*state = JXButtonStates::SetState(xEvent.xbutton.state,
										  currButton, kJTrue);
		return kJTrue;
		}
	else if (xEvent.type == ButtonRelease)
		{
		const JXMouseButton currButton = (JXMouseButton) xEvent.xbutton.button;
		*state = JXButtonStates::SetState(xEvent.xbutton.state,
										  currButton, kJFalse);
		return kJTrue;
		}
	else if (xEvent.type == MotionNotify)
		{
		*state = xEvent.xmotion.state;
		return kJTrue;
		}
	else if (xEvent.type == EnterNotify || xEvent.type == LeaveNotify)
		{
		*state = xEvent.xcrossing.state;
		return kJTrue;
		}
	else
		{
		return kJFalse;		// event doesn't contain the information
		}
}
