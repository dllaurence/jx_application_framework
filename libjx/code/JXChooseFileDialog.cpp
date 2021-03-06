/******************************************************************************
 JXChooseFileDialog.cpp

	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXCSFDialogBase

	Copyright � 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXChooseFileDialog.h>
#include <JXDirTable.h>
#include <JXCurrentPathMenu.h>
#include <JDirInfo.h>

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXPathInput.h>
#include <JXPathHistoryMenu.h>
#include <JXScrollbarSet.h>
#include <jXGlobals.h>

#include <JTableSelection.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JXChooseFileDialog*
JXChooseFileDialog::Create
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		allowSelectMultiple,
	const JCharacter*	origName,
	const JCharacter*	message
	)
{
	JXChooseFileDialog* dlog =
		new JXChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple);
	assert( dlog != NULL );
	dlog->BuildWindow(origName, message);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXChooseFileDialog::JXChooseFileDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		allowSelectMultiple
	)
	:
	JXCSFDialogBase(supervisor, dirInfo, fileFilter),
	itsSelectMultipleFlag(allowSelectMultiple)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXChooseFileDialog::~JXChooseFileDialog()
{
}

/******************************************************************************
 GetFullName

 ******************************************************************************/

JBoolean
JXChooseFileDialog::GetFullName
	(
	JString* fullName
	)
	const
{
	const JDirEntry* entry;
	if ((GetFileBrowser())->GetFirstSelection(&entry))
		{
		*fullName = entry->GetFullName();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetFullNames

 ******************************************************************************/

JBoolean
JXChooseFileDialog::GetFullNames
	(
	JPtrArray<JString>* fullNameList
	)
	const
{
	fullNameList->CleanOut();

	JPtrArray<JDirEntry> entryList(JPtrArrayT::kDeleteAll);
	if ((GetFileBrowser())->GetSelection(&entryList))
		{
		const JSize count = entryList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JString* s = new JString((entryList.NthElement(i))->GetFullName());
			assert( s != NULL );
			fullNameList->Append(s);
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXChooseFileDialog::BuildWindow
	(
	const JCharacter* origName,
	const JCharacter* message
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 310,340, "");
    assert( window != NULL );

    JXTextButton* openButton =
        new JXTextButton(JGetString("openButton::JXChooseFileDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
    assert( openButton != NULL );
    openButton->SetShortcuts(JGetString("openButton::JXChooseFileDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXChooseFileDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
    assert( cancelButton != NULL );

    JXTextButton* homeButton =
        new JXTextButton(JGetString("homeButton::JXChooseFileDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
    assert( homeButton != NULL );

    JXStaticText* pathLabel =
        new JXStaticText(JGetString("pathLabel::JXChooseFileDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
    assert( pathLabel != NULL );
    pathLabel->SetToLabel();

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
    assert( scrollbarSet != NULL );

    JXStaticText* filterLabel =
        new JXStaticText(JGetString("filterLabel::JXChooseFileDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
    assert( filterLabel != NULL );
    filterLabel->SetToLabel();

    JXTextCheckbox* showHiddenCB =
        new JXTextCheckbox(JGetString("showHiddenCB::JXChooseFileDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
    assert( showHiddenCB != NULL );

    JXPathInput* pathInput =
        new JXPathInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
    assert( pathInput != NULL );

    JXInputField* filterInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
    assert( filterInput != NULL );

    JXPathHistoryMenu* pathHistory =
        new JXPathHistoryMenu(1, "", window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
    assert( pathHistory != NULL );

    JXStringHistoryMenu* filterHistory =
        new JXStringHistoryMenu(1, "", window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
    assert( filterHistory != NULL );

    JXTextButton* upButton =
        new JXTextButton(JGetString("upButton::JXChooseFileDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
    assert( upButton != NULL );

    JXTextButton* selectAllButton =
        new JXTextButton(JGetString("selectAllButton::JXChooseFileDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
    assert( selectAllButton != NULL );

    JXCurrentPathMenu* currPathMenu =
        new JXCurrentPathMenu("/", window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
    assert( currPathMenu != NULL );

    JXTextButton* desktopButton =
        new JXTextButton(JGetString("desktopButton::JXChooseFileDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
    assert( desktopButton != NULL );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, cancelButton, upButton, homeButton, desktopButton,
			   selectAllButton, showHiddenCB, currPathMenu,
			   origName, message);
}

/******************************************************************************
 SetObjects (protected)

 ******************************************************************************/

void
JXChooseFileDialog::SetObjects
	(
	JXScrollbarSet*			scrollbarSet,
	JXStaticText*			pathLabel,
	JXPathInput*			pathInput,
	JXPathHistoryMenu*		pathHistory,
	JXStaticText*			filterLabel,
	JXInputField*			filterInput,
	JXStringHistoryMenu*	filterHistory,
	JXTextButton*			openButton,
	JXTextButton*			cancelButton,
	JXTextButton*			upButton,
	JXTextButton*			homeButton,
	JXTextButton*			desktopButton,
	JXTextButton*			selectAllButton,
	JXTextCheckbox*			showHiddenCB,
	JXCurrentPathMenu*		currPathMenu,
	const JCharacter*		origName,
	const JCharacter*		message
	)
{
	if (itsSelectMultipleFlag)
		{
		(scrollbarSet->GetWindow())->SetTitle("Choose files");
		}
	else
		{
		(scrollbarSet->GetWindow())->SetTitle("Choose file");
		}

	itsOpenButton      = openButton;
	itsSelectAllButton = selectAllButton;

	SetButtons(openButton, cancelButton);
	JXCSFDialogBase::SetObjects(
		scrollbarSet, pathLabel, pathInput, pathHistory,
		filterLabel, filterInput, filterHistory,
		openButton, upButton, homeButton, desktopButton, NULL, showHiddenCB,
		currPathMenu, message);

	JXDirTable* fileBrowser = GetFileBrowser();
	fileBrowser->AllowSelectFiles(kJTrue, itsSelectMultipleFlag);
	ListenTo(fileBrowser);
	ListenTo(&(fileBrowser->GetTableSelection()));

	if (itsSelectMultipleFlag)
		{
		ListenTo(itsSelectAllButton);
		}
	else
		{
		itsSelectAllButton->Hide();
		}

    cancelButton->SetShortcuts("^[");

	// select initial file

	JIndex index;
	if (!JStringEmpty(origName) &&
		fileBrowser->ClosestMatch(origName, &index))
		{
		const JDirEntry& entry = (GetDirInfo())->GetEntry(index);
		if (entry.GetName() == origName)
			{
			fileBrowser->UpdateScrollbars();
			fileBrowser->SelectSingleEntry(index);
			}
		}
}

/******************************************************************************
 AdjustSizings (virtual protected)

	Adjust the sizing options for the widgets in the window after the
	message has been installed at the top.

 ******************************************************************************/

void
JXChooseFileDialog::AdjustSizings()
{
	JXCSFDialogBase::AdjustSizings();
	itsSelectAllButton->SetSizing(JXWidget::kFixedRight, JXWidget::kFixedTop);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXChooseFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXDirTable* fileBrowser = GetFileBrowser();

	if (sender == fileBrowser && message.Is(JXDirTable::kFileDblClicked))
		{
		const JXDirTable::FileDblClicked* info =
			dynamic_cast(const JXDirTable::FileDblClicked*, &message);
		assert( info != NULL );
		if (info->IsActive())
			{
			EndDialog(kJTrue);
			}
		}

	else if (sender == itsSelectAllButton && message.Is(JXButton::kPushed))
		{
		fileBrowser->SelectAll();
		}

	else if (sender == fileBrowser &&
			 (message.Is(JXWidget::kGotFocus) ||
			  message.Is(JXWidget::kLostFocus)))
		{
		UpdateDisplay();
		}

	else
		{
		if (sender == &(fileBrowser->GetTableSelection()))
			{
			UpdateDisplay();
			}

		JXCSFDialogBase::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateDisplay (virtual protected)

	Disable the Open button if nothing is selected.

 ******************************************************************************/

void
JXChooseFileDialog::UpdateDisplay()
{
	JXCSFDialogBase::UpdateDisplay();

	JXDirTable* dirTable = GetFileBrowser();
	if (dirTable->HasFocus() && !dirTable->HasSelection())
		{
		itsOpenButton->Deactivate();
		}
	else
		{
		itsOpenButton->Activate();
		}
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JXChooseFileDialog::OKToDeactivate()
{
	if (!JXCSFDialogBase::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	JXPathInput* pathInput = GetPathInput();
	if (pathInput->HasFocus())
		{
		GoToItsPath();
		return kJFalse;
		}

	JXInputField* filterInput = GetFilterInput();
	if (filterInput->HasFocus())
		{
		AdjustFilter();
		return kJFalse;
		}

	JXDirTable* fileBrowser = GetFileBrowser();
	if (fileBrowser->GoToSelectedDirectory())
		{
		return kJFalse;
		}

	JPtrArray<JDirEntry> entryList(JPtrArrayT::kDeleteAll);
	if (fileBrowser->GetSelection(&entryList))
		{
		const JSize count = entryList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			JDirEntry* entry = entryList.NthElement(i);
			entry->ForceUpdate();	// check that link hasn't been broken behind our back
			if (!entry->IsFile())
				{
				(GetDirInfo())->ForceUpdate();
				return kJFalse;
				}
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
