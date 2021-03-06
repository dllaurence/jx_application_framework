/******************************************************************************
 SyGTrashButton.cpp

	Provides a drop target regardless of which virtual terminal the user is on.

	BASE CLASS = JXImageButton

	Copyright � 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include "SyGTrashButton.h"
#include "SyGMoveToTrashProcess.h"
#include "SyGFileTreeTable.h"
#include "SyGFileTree.h"
#include "SyGGlobals.h"
#include <JXDisplay.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <jXUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGTrashButton::SyGTrashButton
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXImageButton(enclosure, hSizing, vSizing, x,y, w,h)
{
	SetImage(SyGGetTrashSmallIcon(), kJFalse);
	ListenTo(this);
	ListenTo(SyGGetApplication());

	JString trashDir;
	if (!SyGGetTrashDirectory(&trashDir, kJFalse))
		{
		Deactivate();
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGTrashButton::~SyGTrashButton()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
SyGTrashButton::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	if (IsDNDTarget())
		{
		JXDrawDNDBorder(p, frame, GetBorderWidth());
		}
	else
		{
		JXImageButton::DrawBorder(p, frame);
		}
}

/******************************************************************************
 UpdateDisplay

 ******************************************************************************/

void
SyGTrashButton::UpdateDisplay()
{
	SetImage(SyGGetTrashSmallIcon(), kJFalse);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SyGTrashButton::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == SyGGetApplication() && message.Is(SyGApplication::kTrashNeedsUpdate))
		{
		UpdateDisplay();
		}

	else
		{
		if (sender == this && message.Is(JXButton::kPushed))
			{
			JString trashDir;
			if (SyGGetTrashDirectory(&trashDir))
				{
				SyGGetApplication()->OpenDirectory(trashDir);
				}
			}

		JXImageButton::Receive(sender, message);
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
SyGTrashButton::WillAcceptDrop
	(
	const JArray<Atom>& typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	JString trashDir;
	if (!SyGGetTrashDirectory(&trashDir, kJFalse))
		{
		return kJFalse;
		}

	const Atom urlXAtom = (GetSelectionManager())->GetURLXAtom();

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		if (typeList.GetElement(i) == urlXAtom)
			{
			*action = (GetDNDManager())->GetDNDActionPrivateXAtom();
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

 ******************************************************************************/

void
SyGTrashButton::HandleDNDEnter()
{
	SyGSetDNDTarget(NULL);
	SetImage(SyGGetTrashSmallIcon(kJTrue), kJFalse);
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

 ******************************************************************************/

void
SyGTrashButton::HandleDNDLeave()
{
	SyGSetDNDTarget(NULL);
	SetImage(SyGGetTrashSmallIcon(kJFalse), kJFalse);
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

 ******************************************************************************/

void
SyGTrashButton::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>& typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	MoveFilesToTrash(time, source);
	HandleDNDLeave();
}

/******************************************************************************
 MoveFilesToTrash (static)

 ******************************************************************************/

void
SyGTrashButton::MoveFilesToTrash
	(
	const Time		time,
	const JXWidget*	source
	)
{
	SyGFileTreeTable* srcTable = NULL;
	if (SyGGetDNDSource(source, &srcTable) &&
		SyGIsTrashDirectory((srcTable->GetFileTree())->GetDirectory()))
		{
		return;
		}

	JXDisplay* display             = (JXGetApplication())->GetCurrentDisplay();
	JXSelectionManager* selManager = display->GetSelectionManager();
	JXDNDManager* dndMgr           = display->GetDNDManager();
	const Atom dndSelectionName    = dndMgr->GetDNDSelectionName();

	unsigned char* data = NULL;
	JSize dataLength;
	Atom returnType;
	JXSelectionManager::DeleteMethod delMethod;
	if (selManager->GetData(dndSelectionName, time, selManager->GetURLXAtom(),
							&returnType, &data, &dataLength, &delMethod))
		{
		if (returnType == selManager->GetURLXAtom())
			{
			JPtrArray<JString>* fileNameList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
			assert( fileNameList != NULL );
			JPtrArray<JString> urlList(JPtrArrayT::kDeleteAll);
			JXUnpackFileNames((char*) data, dataLength, fileNameList, &urlList);

			SyGMoveToTrashProcess::Move(srcTable, fileNameList);
			JXReportUnreachableHosts(urlList);
			}

		selManager->DeleteData(&data, delMethod);
		}

	SyGSetDNDSource(NULL);
}
