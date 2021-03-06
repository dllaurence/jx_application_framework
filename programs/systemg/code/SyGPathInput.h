/******************************************************************************
 SyGPathInput.h

	Copyright � 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGPathInput
#define _H_SyGPathInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPathInput.h>
#include <JString.h>

class SyGPathInput : public JXPathInput
{
public:

	SyGPathInput(JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~SyGPathInput();

	void	SetDirList(JXWidget* dirList);

	virtual void HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	HandleFocusEvent();			// must call inherited

private:

	JXWidget*	itsDirList;
	JString		itsInitialText;

	// not allowed

	SyGPathInput(const SyGPathInput& source);
	const SyGPathInput& operator=(const SyGPathInput& source);
};

#endif
