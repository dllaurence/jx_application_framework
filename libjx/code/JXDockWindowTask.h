/******************************************************************************
 JXDockWindowTask.h

	Copyright � 2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDockWindowTask
#define _H_JXDockWindowTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "JXIdleTask.h"
#include <JBroadcaster.h>
#include <JPoint.h>

class JXWindow;

class JXDockWindowTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXDockWindowTask(JXWindow* window, const Window parent, const JPoint& topLeft);

	virtual ~JXDockWindowTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);
	JBoolean		IsDone() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXWindow*	itsWindow;
	Window		itsParent;
	JPoint		itsPoint;
	JIndex		itsState;

private:

	// not allowed

	JXDockWindowTask(const JXDockWindowTask& source);
	const JXDockWindowTask& operator=(const JXDockWindowTask& source);
};

#endif
