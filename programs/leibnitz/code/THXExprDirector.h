/******************************************************************************
 THXExprDirector.h

	Interface for the THXExprDirector class

	Copyright � 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THXExprDirector
#define _H_THXExprDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JString.h>

class JXTextButton;
class JXTextMenu;
class JXVertPartition;
class THXVarList;
class THXExprEditor;
class THXTapeText;
class THXKeyPad;

class THXExprDirector : public JXWindowDirector
{
public:

	THXExprDirector(JXDirector* supervisor, const THXVarList* varList);
	THXExprDirector(istream& input, const JFileVersion vers,
				   JXDirector* supervisor, const THXVarList* varList);

	virtual ~THXExprDirector();

	virtual void	Activate();

	void	UpdateDisplay();
	void	WriteState(ostream& output) const;

	static void	ReadPrefs(istream& input, const JFileVersion vers);
	static void	WritePrefs(ostream& output);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	THXExprEditor*	itsExprWidget;
	THXTapeText*	itsTapeWidget;
	JXTextMenu*		itsActionsMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	static JString	itsDefGeom;			// window size + partition geometry
	JString			itsTapeName;		// file in which user last saved tape

// begin JXLayout

    JXVertPartition* itsPartition;
    THXKeyPad*       itsKeyPad;

// end JXLayout

private:

	void	BuildWindow(const THXVarList* varList);

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	void	SaveTape();

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UseDefaultGeometry();
	void	SetDefaultGeometry();

	// not allowed

	THXExprDirector(const THXExprDirector& source);
	const THXExprDirector& operator=(const THXExprDirector& source);
};

#endif
