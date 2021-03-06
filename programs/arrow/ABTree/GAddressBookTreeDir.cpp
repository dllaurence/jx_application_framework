/******************************************************************************
 GAddressBookTreeDir.cc

	<Description>

	BASE CLASS = public GMManagedDirector

	Copyright � 1998 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

// includes

//Class Header
#include <GAddressBookTreeDir.h>
#include <JTree.h>
#include <JNamedTreeNode.h>
#include <GAddressBookTreeWidget.h>
#include <JNamedTreeList.h>
#include <JXNamedTreeListWidget.h>
#include <GMGlobals.h>
#include <GPrefsMgr.h>
#include <GAddressBookMgr.h>
#include "GHelpText.h"
#include <GMApp.h>
#include "GMDirectorManager.h"
#include "GMDirectorMenu.h"

#include "address_entry.xpm"
#include "jx_help_toc.xpm"
#include "jx_help_specific.xpm"

#include <JXToolBar.h>

#include <JChooseSaveFile.h>
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXMenuBar.h>
#include <JXScrollbarSet.h>
#include <JXApplication.h>
#include <JXTextButton.h>
#include <JXTimerTask.h>
#include <JXHelpManager.h>
#include <JXImage.h>

#include <JString.h>

#include <jAssert.h>

static const JCharacter* kWindowsMenuTitleStr = "Windows";

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"About"
	"%l|Table of Contents %i \"TOCHelp::Arrow\""
	"|Overview|This window %i \"ThisWindowHelp::Arrow\""
	"%l|Changes|Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

const JCoordinate kCloseButtonWidth	= 50;
const JCoordinate kCurrentPrefsVersion	= 0;

const Time kDirUpdateDelay = 1000;	// milliseconds

const JCharacter* kGMAddDirMenuShortcut	= "Meta-Shift-A";
const JCharacter* kGMAddDirMenuID		= "AddBookWindow::Arrow";

/******************************************************************************
 Constructor

 *****************************************************************************/

GAddressBookTreeDir::GAddressBookTreeDir
	(
	JXDirector* supervisor
	)
	:
   GMManagedDirector(supervisor),
   JPrefObject(GGetPrefsMgr(), kAddressTreePrefsID),
   itsPrefsOK(kJFalse)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
	itsTimerTask = new JXTimerTask(kDirUpdateDelay);
	assert(itsTimerTask != NULL);
	itsTimerTask->Start();
	ListenTo(itsTimerTask);
	itsWidget->Update();
	if (!itsPrefsOK)
		{
		GGetAddressBookMgr()->AddDefaultAddressBook(itsTree);
		}

	GGetDirMgr()->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GAddressBookTreeDir::~GAddressBookTreeDir()
{
	JPrefObject::WritePrefs();
	itsWidget->Save();
	delete itsTree;
	delete itsTimerTask;
	delete itsMenuIcon;

	GGetDirMgr()->DirectorDeleted(this);
}

/******************************************************************************
 BuildWindow


 ******************************************************************************/

void
GAddressBookTreeDir::BuildWindow()
{
	JCoordinate w 		= 300;
	JCoordinate minW	= 250;
	JCoordinate h 		= 500;
	JCoordinate minH	= 250;

	JXWindow* window = new JXWindow(this, w,h, "Addresses");
    assert( window != NULL );

	window->SetWMClass(GMGetWMClassInstance(), GMGetAddressBookWindowClass());
	window->SetCloseAction(JXWindow::kDeactivateDirector);
    window->PlaceAsDialogWindow();
	window->ShouldFocusWhenShow(kJTrue);
    //window->SetMinSize(200, 300);
	//window->LockCurrentMinSize();

	JXMenuBar* menuBar =
		new JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
			0,0, w - kCloseButtonWidth,kJXDefaultMenuBarHeight);
	assert( menuBar != NULL );

    itsCloseButton =
        new JXTextButton("Close", window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop,
                    w - kCloseButtonWidth,0,
                    kCloseButtonWidth,kJXDefaultMenuBarHeight);
    assert( itsCloseButton != NULL );
    itsCloseButton->SetShortcuts("#W"); //^[
    ListenTo(itsCloseButton);

	itsToolBar =
		new JXToolBar(GGetPrefsMgr(), kGAddressToolBarID,
			menuBar, minW, minH, window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, kJXDefaultMenuBarHeight, w, h - kJXDefaultMenuBarHeight);
	assert(itsToolBar != NULL);

	const JSize newHeight = itsToolBar->GetWidgetEnclosure()->GetBoundsHeight();

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic,JXWidget::kVElastic,
						   0, 0,
						   w, newHeight);
	assert( scrollbarSet != NULL );

    JNamedTreeNode* base = new JNamedTreeNode(NULL, "");
    assert(base != NULL);
    itsTree = new JTree(base);
    assert(itsTree != NULL);
    JNamedTreeList* list = new JNamedTreeList(itsTree);

    itsWidget =
		new GAddressBookTreeWidget(itsTree, list,  menuBar,
									scrollbarSet, scrollbarSet->GetScrollEnclosure(),
									JXWidget::kHElastic, JXWidget::kVElastic,
									0,0,w,newHeight);
    assert(itsWidget != NULL);

	GMDirectorMenu* menu =
		new GMDirectorMenu(kWindowsMenuTitleStr, menuBar,
			JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert(menu != NULL);
	menuBar->AppendMenu(menu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd, JXPM(jx_help_toc));
	itsHelpMenu->SetItemImage(kThisWindowCmd, JXPM(jx_help_specific));

	itsToolBar->LoadPrefs();

	if (itsToolBar->IsEmpty())
		{
		itsWidget->AddDefaultButtcons(itsToolBar);
		itsToolBar->AppendButton(itsHelpMenu, kTOCCmd);
		itsToolBar->AppendButton(itsHelpMenu, kThisWindowCmd);
		}

	itsMenuIcon = new JXImage(window->GetDisplay(), address_entry);
	assert(itsMenuIcon != NULL);
	itsMenuIcon->ConvertToRemoteStorage();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GAddressBookTreeDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}
	else if (sender == itsTimerTask && message.Is(JXTimerTask::kTimerWentOff))
		{
		itsWidget->Update();
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}
	else
		{
		GMManagedDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReadPrefs (protected)

 ******************************************************************************/

void
GAddressBookTreeDir::ReadPrefs
	(
	istream& input
	)
{
	itsPrefsOK = kJTrue;
	JCoordinate id;
	input >> id;
	assert(id <= kCurrentPrefsVersion);
	GetWindow()->ReadGeometry(input);
	itsWidget->ReadPrefs(input);
	JBoolean active;
	input >> active;
	JBoolean iconified;
	input >> iconified;
	if (iconified)
		{
		GetWindow()->Deiconify();
		}
	if (active && !iconified)
		{
		Activate();
		}
}

/******************************************************************************
 WritePrefs (protected)

 ******************************************************************************/

void
GAddressBookTreeDir::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion << ' ';
	GetWindow()->WriteGeometry(output);
	itsWidget->WritePrefs(output);
	output << IsActive() << ' ';
	output << GetWindow()->IsIconified() << ' ';
}

/******************************************************************************
 SaveOnDeath (public)

 ******************************************************************************/

void
GAddressBookTreeDir::SaveOnDeath()
{
	itsWidget->Save();
}

/******************************************************************************
 HandleHelpMenu


 ******************************************************************************/

void
GAddressBookTreeDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
		{
		GMGetApplication()->DisplayAbout();
		}
	else if (index == kTOCCmd)
		{
		(JXGetHelpManager())->ShowSection(kGTOCHelpName);
		}
	else if (index == kOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kGOverviewHelpName);
		}
	else if (index == kThisWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kGABHelpName);
		}
	else if (index == kChangesCmd)
		{
		(JXGetHelpManager())->ShowSection(kGChangeLogName);
		}
	else if (index == kCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kGCreditsName);
		}

}

/******************************************************************************
 EditToolBar (public)

 ******************************************************************************/

void
GAddressBookTreeDir::EditToolBar()
{
	itsToolBar->Edit();
}

/******************************************************************************
 GetShortcut (public)

 ******************************************************************************/

JBoolean
GAddressBookTreeDir::GetShortcut
	(
	JString* shortcut
	)
	const
{
	*shortcut = kGMAddDirMenuShortcut;
	return kJTrue;
}

/******************************************************************************
 GetID (public)

 ******************************************************************************/

JBoolean
GAddressBookTreeDir::GetID
	(
	JString* id
	)
	const
{
	*id = kGMAddDirMenuID;
	return kJTrue;
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

JBoolean
GAddressBookTreeDir::Deactivate()
{
	itsWidget->Save();
	return JXWindowDirector::Deactivate();
}
