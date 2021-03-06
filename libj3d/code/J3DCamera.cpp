/******************************************************************************
 J3DCamera.cpp

	Pure virtual base class for rendering J3DUniverse.  Derived classes
	must implement the following functions:

		Render
			Redraw the scene.

		Refresh
			Call Render() as soon as is convenient. (e.g. next idle point)

		Redraw
			Set up and call Render() immediately.

		UpdateViewport
			Update the viewport of the camera, especially the
			aspect ratio of width to height.

		ModelToScreen
			Convert the given point from model coordinates to screen coordinates.

		ScreenToModel
			Convert the given point from screen coordinates to model coordinates.

	BASE CLASS = NONE

	Copyright � 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <J3DCamera.h>
#include <J3DUniverse.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DCamera::J3DCamera
	(
	J3DUniverse* universe
	)
	:
	itsUniverse(universe),

	itsPosition   (3, 1.0, 0.0, 0.0),
	itsAttentionPt(3, 0.0, 0.0, 0.0),
	itsUpVector   (3, 0.0, 0.0, 1.0)
{
	itsUniverse->AddCamera(this);
}

J3DCamera::J3DCamera
	(
	J3DUniverse*	universe,
	const JVector&	position,
	const JVector&	attentionPt,
	const JVector&	upVector
	)
	:
	itsUniverse(universe),

	itsPosition(position),
	itsAttentionPt(attentionPt),
	itsUpVector(upVector)
{
	itsUniverse->AddCamera(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DCamera::~J3DCamera()
{
	itsUniverse->RemoveCamera(this);
}

#define JTemplateType J3DCamera
#include <JPtrArray.tmpls>
#undef JTemplateType
