/*********************************************************************************
 GloveCursorMonitor.h

	Interface for the GloveCursorMonitor class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ********************************************************************************/
 
#ifndef _H_GloveCursorMonitor
#define _H_GloveCursorMonitor

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "JXWidget.h"

class GlovePlotter;

class GloveCursorMonitor : public JXWidget
{
public:

	GloveCursorMonitor(GlovePlotter* plot, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	virtual ~GloveCursorMonitor();
	
protected:

	virtual void	Receive(JBroadcaster* sender,
							const JBroadcaster::Message& message);
	
	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

private:

	GlovePlotter*	itsPlot;
	JBoolean		itsShowX;
	JBoolean		itsShowY;
	JBoolean		itsDual;
	JFloat			itsX1;
	JFloat			itsX2;
	JFloat			itsY1;
	JFloat			itsY2;

private:

	// not allowed

	GloveCursorMonitor(const GloveCursorMonitor& source);
	const GloveCursorMonitor& operator=(const GloveCursorMonitor& source);
	
};
#endif
