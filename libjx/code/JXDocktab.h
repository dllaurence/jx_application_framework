/******************************************************************************
 JXDocktab.h

	Copyright � 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDocktab
#define _H_JXDocktab

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>
#include <JXDNDManager.h>
#include <JPartition.h>		// need defn of kDragRegionSize

class JXTextMenu;

class JXDocktab : public JXWidget
{
public:

	enum
	{
		kWidth = 9
	};

public:

	JXDocktab(JXContainer* enclosure,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);

	virtual ~JXDocktab();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	class DockFinder : public JXDNDManager::TargetFinder
		{
		public:

			DockFinder(JXDisplay* display);

			virtual	~DockFinder();

			virtual JBoolean	FindTarget(const JXContainer* coordOwner,
										   const JPoint& pt,
										   Window* xWindow, Window* msgWindow,
										   JXContainer** target, Atom* vers);
		};

private:

	JXTextMenu*	itsActionMenu;	// NULL until first used
	DockFinder*	itsDockFinder;
	JColorIndex	itsFocusColor;

private:

	void	OpenActionMenu(const JPoint& pt, const JXButtonStates& buttonStates,
						   const JXKeyModifiers& modifiers);
	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	// not allowed

	JXDocktab(const JXDocktab& source);
	const JXDocktab& operator=(const JXDocktab& source);
};

#endif
