/******************************************************************************
 DeleteBroadcasterTask.cpp

	Urgent tasks are performed as soon as possible and then deleted.

	BASE CLASS = JXUrgentTask

	Copyright � 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include <DeleteBroadcasterTask.h>

#include <JXApplication.h>

#include <JBroadcaster.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DeleteBroadcasterTask::DeleteBroadcasterTask
	(
	JBroadcaster* object
	)
	:
	JXUrgentTask()
{
	itsObject = object;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DeleteBroadcasterTask::~DeleteBroadcasterTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
DeleteBroadcasterTask::Perform()
{
	delete itsObject;
}
