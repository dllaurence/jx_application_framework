/******************************************************************************
 JXVIKeyHandler.h

	Copyright � 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXVIKeyHandler
#define _H_JXVIKeyHandler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JVIKeyHandler.h>

class JXTEBase;

class JXVIKeyHandler : public JVIKeyHandler
{
public:

	JXVIKeyHandler(JXTEBase* te);

	virtual ~JXVIKeyHandler();

	virtual JBoolean	HandleKeyPress(const JCharacter key, const JBoolean selectText,
									   const JTextEditor::CaretMotion motion,
									   const JBoolean deleteToTabStop);

private:

	JXTEBase*	itsJXTE;

private:

	// not allowed

	JXVIKeyHandler(const JXVIKeyHandler& source);
	const JXVIKeyHandler& operator=(const JXVIKeyHandler& source);
};

#endif
