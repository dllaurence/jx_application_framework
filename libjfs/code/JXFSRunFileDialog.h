/******************************************************************************
 JXFSRunFileDialog.h

	Copyright � 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFSRunFileDialog
#define _H_JXFSRunFileDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include "JFSBinding.h"		// need defn of CommandType

class JString;
class JXInputField;
class JXTextButton;
class JXTextCheckbox;
class JXFSCommandHistoryMenu;

class JXFSRunFileDialog : public JXDialogDirector
{
public:

	JXFSRunFileDialog(const JCharacter* fileName, const JBoolean allowSaveCmd);

	virtual ~JXFSRunFileDialog();

	const JString&	GetCommand(JFSBinding::CommandType* type,
							   JBoolean* singleFile, JBoolean* saveBinding) const;

	static void	HandleHistoryMenu(const Message& message, JXFSCommandHistoryMenu* menu,
								  JXInputField* cmdInput, JXTextCheckbox* shellCB,
								  JXTextCheckbox* windowCB, JXTextCheckbox* singleFileCB);
	static void	HandleChooseCmdButton(JXInputField* cmdInput);

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message);
	virtual JBoolean	OKToDeactivate();

private:

// begin JXLayout

    JXInputField*           itsCmdInput;
    JXTextButton*           itsHelpButton;
    JXTextButton*           itsOKButton;
    JXFSCommandHistoryMenu* itsCmdHistoryMenu;
    JXTextButton*           itsChooseCmdButton;
    JXTextCheckbox*         itsSaveBindingCB;
    JXTextCheckbox*         itsUseShellCB;
    JXTextCheckbox*         itsUseWindowCB;
    JXTextCheckbox*         itsSingleFileCB;

// end JXLayout

private:

	void		BuildWindow(const JCharacter* fileName, const JBoolean allowSaveCmd);
	void		UpdateDisplay();

	JBoolean	ReadSetup();
	void		WriteSetup();

	void		ReadSetup(istream& input);
	void		WriteSetup(ostream& output) const;

	// not allowed

	JXFSRunFileDialog(const JXFSRunFileDialog& source);
	const JXFSRunFileDialog& operator=(const JXFSRunFileDialog& source);
};

#endif
