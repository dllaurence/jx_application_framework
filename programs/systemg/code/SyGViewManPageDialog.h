/******************************************************************************
 SyGViewManPageDialog.h

	Interface for the SyGViewManPageDialog class

	Copyright � 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGViewManPageDialog
#define _H_SyGViewManPageDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JPrefObject.h>
#include <JString.h>

class JXTextButton;
class JXInputField;
class JXCharInput;
class JXTextCheckbox;
class JXStringHistoryMenu;

class SyGViewManPageDialog : public JXWindowDirector, public JPrefObject
{
public:

	SyGViewManPageDialog(JXDirector* supervisor);

	virtual ~SyGViewManPageDialog();

	virtual void	Activate();

	void	ViewManPage(const JCharacter* item, const JCharacter index = ' ',
						const JBoolean apropos = kJFalse);
	void	ViewManPages(const JPtrArray<JString>& list);

	static const JString&	GetViewManPageCommand();
	static void				SetViewManPageCommand(const JCharacter* cmd);

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	static JString	itsViewCmd;		// static so SyGPrefsMgr can set it

// begin JXLayout

    JXTextButton*        itsCloseButton;
    JXTextButton*        itsViewButton;
    JXInputField*        itsFnName;
    JXTextCheckbox*      itsAproposCheckbox;
    JXTextButton*        itsHelpButton;
    JXStringHistoryMenu* itsFnHistoryMenu;
    JXCharInput*         itsManIndex;
    JXTextCheckbox*      itsStayOpenCB;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateDisplay();
	void	ViewManPage();
	void	SetFunction(const JCharacter* historyStr);
	void	AddToHistory(const JCharacter* pageName, const JCharacter pageIndex,
						 const JBoolean apropos);

	// not allowed

	SyGViewManPageDialog(const SyGViewManPageDialog& source);
	const SyGViewManPageDialog& operator=(const SyGViewManPageDialog& source);
};


/******************************************************************************
 View man page command (static)

 ******************************************************************************/

inline const JString&
SyGViewManPageDialog::GetViewManPageCommand()
{
	return itsViewCmd;
}

inline void
SyGViewManPageDialog::SetViewManPageCommand
	(
	const JCharacter* cmd
	)
{
	itsViewCmd = cmd;
	itsViewCmd.TrimWhitespace();
}

#endif
