/******************************************************************************
 JBooleanNOT.h

	Interface for JBooleanNOT class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBooleanNOT
#define _H_JBooleanNOT

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JDecision.h>

class JBooleanNOT : public JDecision
{
public:

	JBooleanNOT();
	JBooleanNOT(JDecision* arg);
	JBooleanNOT(const JBooleanNOT& source);

	virtual ~JBooleanNOT();

	virtual JBoolean	Evaluate() const;
	virtual void		Print(ostream& output) const;
	virtual JDecision*	Copy() const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

	const JDecision*	GetArg() const;
	JDecision*			GetArg();
	void				SetArg(JDecision* arg);

private:

	JDecision*	itsArg;

private:

	// not allowed

	const JBooleanNOT& operator=(const JBooleanNOT& source);
};

/******************************************************************************
 GetArg

 ******************************************************************************/

inline const JDecision*
JBooleanNOT::GetArg()
	const
{
	return itsArg;
}

inline JDecision*
JBooleanNOT::GetArg()
{
	return itsArg;
}

/******************************************************************************
 SetArg

 ******************************************************************************/

inline void
JBooleanNOT::SetArg
	(
	JDecision* arg
	)
{
	delete itsArg;
	itsArg = arg;
}

#endif
