/******************************************************************************
 SyGFindFileDialog.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright � 1996-2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include "SyGFindFileDialog.h"
#include "SyGFindFileTask.h"
#include "SyGViewManPageDialog.h"
#include "SyGPrefsMgr.h"
#include "SyGGlobals.h"

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXPathInput.h>
#include <JXChooseSaveFile.h>

#include <jProcessUtil.h>
#include <jAssert.h>

enum
{
	kFindFileAction = 1,
	kFindExprAction
};

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGFindFileDialog::SyGFindFileDialog
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(SyGGetPrefsMgr(), kSFindFilePrefID)
{
	BuildWindow();
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFindFileDialog::~SyGFindFileDialog()
{
	// prefs written by SyGDeleteGlobals()
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
SyGFindFileDialog::Activate()
{
	JXWindowDirector::Activate();

	if (IsActive() && itsFileInput->IsActive())
		{
		itsFileInput->Focus();
		itsFileInput->SelectAll();
		}
	else if (IsActive() && itsExprInput->IsActive())
		{
		itsExprInput->Focus();
		itsExprInput->SelectAll();
		}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SyGFindFileDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 360,220, "");
    assert( window != NULL );
    SetWindow(window);

    itsActionRG =
        new JXRadioGroup(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 10,50, 340,120);
    assert( itsActionRG != NULL );

    itsCloseButton =
        new JXTextButton(JGetString("itsCloseButton::SyGFindFileDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,190, 60,20);
    assert( itsCloseButton != NULL );
    itsCloseButton->SetShortcuts(JGetString("itsCloseButton::SyGFindFileDialog::shortcuts::JXLayout"));

    itsSearchButton =
        new JXTextButton(JGetString("itsSearchButton::SyGFindFileDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 279,189, 62,22);
    assert( itsSearchButton != NULL );
    itsSearchButton->SetShortcuts(JGetString("itsSearchButton::SyGFindFileDialog::shortcuts::JXLayout"));

    itsFileInput =
        new JXInputField(itsActionRG,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 35,30, 295,20);
    assert( itsFileInput != NULL );

    JXTextRadioButton* obj1_JXLayout =
        new JXTextRadioButton(kFindFileAction, JGetString("obj1_JXLayout::SyGFindFileDialog::JXLayout"), itsActionRG,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 320,20);
    assert( obj1_JXLayout != NULL );

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::SyGFindFileDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 200,190, 60,20);
    assert( itsHelpButton != NULL );
    itsHelpButton->SetShortcuts(JGetString("itsHelpButton::SyGFindFileDialog::shortcuts::JXLayout"));

    itsStayOpenCB =
        new JXTextCheckbox(JGetString("itsStayOpenCB::SyGFindFileDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,190, 90,20);
    assert( itsStayOpenCB != NULL );

    itsExprInput =
        new JXInputField(itsActionRG,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 35,90, 295,20);
    assert( itsExprInput != NULL );

    JXTextRadioButton* obj2_JXLayout =
        new JXTextRadioButton(kFindExprAction, JGetString("obj2_JXLayout::SyGFindFileDialog::JXLayout"), itsActionRG,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 10,70, 320,20);
    assert( obj2_JXLayout != NULL );

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::SyGFindFileDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 50,20);
    assert( obj3_JXLayout != NULL );

    itsPathInput =
        new JXPathInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 70,20, 210,20);
    assert( itsPathInput != NULL );

    itsChoosePathButton =
        new JXTextButton(JGetString("itsChoosePathButton::SyGFindFileDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 280,20, 60,20);
    assert( itsChoosePathButton != NULL );

// end JXLayout

	window->SetTitle("Find files");
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();
	window->ShouldFocusWhenShow(kJTrue);

	itsActionRG->SetBorderWidth(0);

	ListenTo(itsChoosePathButton);
	ListenTo(itsActionRG);
	ListenTo(itsSearchButton);
	ListenTo(itsCloseButton);
	ListenTo(itsHelpButton);

	itsPathInput->ShouldAllowInvalidPath();

	itsFileInput->ShouldBroadcastAllTextChanged(kJTrue);
	itsFileInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsFileInput);

	itsExprInput->ShouldBroadcastAllTextChanged(kJTrue);
	itsExprInput->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	ListenTo(itsExprInput);

	itsStayOpenCB->SetState(kJTrue);

	UpdateDisplay();
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
SyGFindFileDialog::UpdateDisplay()
{
	const JIndex action = itsActionRG->GetSelectedItem();
	if (action == kFindFileAction)
		{
		itsExprInput->Deactivate();
		itsFileInput->Activate();
		itsFileInput->Focus();
		itsFileInput->SelectAll();
		}
	else if (action == kFindExprAction)
		{
		itsFileInput->Deactivate();
		itsExprInput->Activate();
		itsExprInput->Focus();
		itsExprInput->SelectAll();
		}

	UpdateButtons();
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
SyGFindFileDialog::UpdateButtons()
{
	JXInputField* field = NULL;

	const JIndex action = itsActionRG->GetSelectedItem();
	if (action == kFindFileAction)
		{
		field = itsFileInput;
		}
	else if (action == kFindExprAction)
		{
		field = itsExprInput;
		}

	assert( field != NULL );
	if (field->IsEmpty())
		{
		itsSearchButton->Deactivate();
		}
	else
		{
		itsSearchButton->Activate();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGFindFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSearchButton && message.Is(JXButton::kPushed))
		{
		if (Search() && !itsStayOpenCB->IsChecked())
			{
			Deactivate();
			}
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		(GetWindow())->KillFocus();
		Deactivate();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(SyGGetManPageDialog())->ViewManPage("find");
		}

	else if (sender == itsChoosePathButton && message.Is(JXButton::kPushed))
		{
		itsPathInput->ChoosePath("");
		}

	else if (sender == itsActionRG && message.Is(JXRadioGroup::kSelectionChanged))
		{
		UpdateDisplay();
		}

	else if ((sender == itsFileInput || sender == itsExprInput) &&
			 (message.Is(JTextEditor::kTextSet) ||
			  message.Is(JTextEditor::kTextChanged)))
		{
		UpdateButtons();
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 Search (private)

 ******************************************************************************/

JBoolean
SyGFindFileDialog::Search()
{
	itsPathInput->ShouldAllowInvalidPath(kJFalse);
	if (!itsPathInput->InputValid())
		{
		itsPathInput->ShouldAllowInvalidPath(kJTrue);
		itsPathInput->Focus();
		return kJFalse;
		}
	itsPathInput->ShouldAllowInvalidPath(kJTrue);

	JString path;
	const JBoolean ok = itsPathInput->GetPath(&path);
	assert( ok );

	const JIndex action = itsActionRG->GetSelectedItem();
	if (action == kFindFileAction && itsFileInput->InputValid())
		{
		SearchFileName(path, itsFileInput->GetText());
		}
	else if (action == kFindExprAction && itsExprInput->InputValid())
		{
		SearchExpr(path, itsExprInput->GetText());
		}

	return kJTrue;
}

/******************************************************************************
 Search

 ******************************************************************************/

void
SyGFindFileDialog::Search
	(
	const JCharacter* path
	)
{
	itsPathInput->SetText(path);
	Activate();
}

/******************************************************************************
 SearchFileName

 ******************************************************************************/

void
SyGFindFileDialog::SearchFileName
	(
	const JCharacter* path,
	const JCharacter* pattern
	)
{
	JString expr = "-iname ";
	expr        += JPrepArgForExec(pattern);

	SearchExpr(path, expr);
}

/******************************************************************************
 SearchExpr

 ******************************************************************************/

void
SyGFindFileDialog::SearchExpr
	(
	const JCharacter* path,
	const JCharacter* expr
	)
{
	SyGTreeDir* dir;
	if ((SyGGetApplication())->OpenDirectory(path, &dir, kJTrue, kJTrue, kJFalse, kJTrue))
		{
		SyGFindFileTask* task;
		SyGFindFileTask::Create(dir, path, expr, &task);
		}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
SyGFindFileDialog::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	JBoolean stayOpen;
	input >> stayOpen;
	itsStayOpenCB->SetState(stayOpen);
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
SyGFindFileDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	(GetWindow())->WriteGeometry(output);

	output << ' ' << itsStayOpenCB->IsChecked();
}