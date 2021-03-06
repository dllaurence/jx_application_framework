/******************************************************************************
 JTEKeyHandler.h

	Copyright � 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTEKeyHandler
#define _H_JTEKeyHandler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JTextEditor.h>

class JTEKeyHandler
{
	friend class JTextEditor;

public:

	JTEKeyHandler(JTextEditor* te);

	virtual ~JTEKeyHandler();

	virtual JBoolean	HandleKeyPress(const JCharacter key, const JBoolean selectText,
									   const JTextEditor::CaretMotion motion,
									   const JBoolean deleteToTabStop) = 0;

protected:

	JTextEditor*	GetTE() const;
	void			InsertKeyPress(const JCharacter key);
	void			BackwardDelete(const JBoolean deleteToTabStop, JString* text);
	void			ForwardDelete(const JBoolean deleteToTabStop, JString* text);
	void			MoveCaretVert(const JInteger deltaLines);

	// called by JTextEditor

	virtual void	Initialize();

private:

	JTextEditor*	itsTE;		// we don't own this

private:

	// not allowed

	JTEKeyHandler(const JTEKeyHandler& source);
	const JTEKeyHandler& operator=(const JTEKeyHandler& source);
};


/******************************************************************************
 GetTE (protected)

 ******************************************************************************/

inline JTextEditor*
JTEKeyHandler::GetTE()
	const
{
	return itsTE;
}

#endif
