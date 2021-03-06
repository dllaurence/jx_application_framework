/******************************************************************************
 GloveCursorTableDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright � 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "GloveCursorTableDir.h"
#include "GloveCursorTable.h"
#include "GlovePlotter.h"
#include "PlotDir.h"
#include <JXTextButton.h>
#include <JXScrollbarSet.h>
#include <JXWindow.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GloveCursorTableDir::GloveCursorTableDir
	(
	PlotDir* supervisor,
	GlovePlotter* plot
	)
	:
	JXWindowDirector(supervisor)
{
	itsPlotDir = supervisor;
	
	JXWindow* window = new JXWindow(this, 250,300, "Cursor Values");
    assert( window != NULL );
    window->SetMinSize(250,300);
    
	JXScrollbarSet* scrollbarSet = 
		new JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 
			0,0,250,260);
	
	itsTable = 
		new GloveCursorTable(itsPlotDir->GetSessionDir(), plot, 
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic, 
			0,0,250,260);
	assert (itsTable != NULL);
	
	itsCloseButton = 
		new JXTextButton("Close", window,
			JXWidget::kHElastic, JXWidget::kVElastic, 
			10, 270, 70, 20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts("#W");
	ListenTo(itsCloseButton);
	
	itsSessionButton = 
		new JXTextButton("Session", window,
			JXWidget::kHElastic, JXWidget::kVElastic, 
			100, 270, 70, 20);
	assert( itsSessionButton != NULL );
	ListenTo(itsSessionButton);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GloveCursorTableDir::~GloveCursorTableDir()
{
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GloveCursorTableDir::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}
		
	if (sender == itsSessionButton && message.Is(JXButton::kPushed))
		{
		SendToSession();
		}	
		
	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 SendToSession

 ******************************************************************************/

void
GloveCursorTableDir::SendToSession()
{
	itsTable->SendToSession();
}
