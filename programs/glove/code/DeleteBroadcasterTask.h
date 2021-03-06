/******************************************************************************
 DeleteBroadcasterTask.h

	Interface for the DeleteBroadcasterTask class

	Copyright � 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_DeleteBroadcasterTask
#define _H_DeleteBroadcasterTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class JBroadcaster;

class DeleteBroadcasterTask : public JXUrgentTask
{
public:

	DeleteBroadcasterTask(JBroadcaster* object);

	virtual ~DeleteBroadcasterTask();

	virtual void	Perform();

private:

	JBroadcaster* 		itsObject;

	// not allowed

	DeleteBroadcasterTask(const DeleteBroadcasterTask& source);
	const DeleteBroadcasterTask& operator=(const DeleteBroadcasterTask& source);
};

#endif
