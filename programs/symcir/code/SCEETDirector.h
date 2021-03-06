/******************************************************************************
 SCEETDirector.h

	Interface for the SCEETDirector class

	Copyright � 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCEETDirector
#define _H_SCEETDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SCDirectorBase.h"

class SCExprEditor;
class SCComponentMenu;
class JXTextButton;
class JXHorizPartition;
class JXVertPartition;

class SCEETDirector : public SCDirectorBase
{
public:

	SCEETDirector(SCCircuitDocument* supervisor);
	SCEETDirector(istream& input, const JFileVersion vers,
				  SCCircuitDocument* supervisor);

	virtual ~SCEETDirector();

	virtual void	StreamOut(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXHorizPartition*	itsHPartition;
	JXHorizPartition*	itsZPartition;

	SCExprEditor*	itsOutputFn;
	SCExprEditor*	itsH0;
	SCExprEditor*	itsHinf;
	SCExprEditor*	itsZd;
	SCExprEditor*	itsZn;
	SCExprEditor*	itsScratchFn;

	JIndex	itsBuildH0Index;
	JIndex	itsBuildHinfIndex;

// begin JXLayout

    JXVertPartition* itsMainPartition;
    SCComponentMenu* itsInputSource;
    SCComponentMenu* itsExtraElement;
    JXTextButton*    itsEvalButton;

// end JXLayout

private:

	void	BuildWindow(SCCircuitDocument* doc);
	void	EvaluateEETParameters();

	void	UpdateExtraMenu();
	void	HandleExtraMenu(const JIndex item);

	// not allowed

	SCEETDirector(const SCEETDirector& source);
	const SCEETDirector& operator=(const SCEETDirector& source);
};

#endif
