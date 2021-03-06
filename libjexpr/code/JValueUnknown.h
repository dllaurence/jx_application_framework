/******************************************************************************
 JValueUnknown.h

	Interface for JValueUnknown class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JValueUnknown
#define _H_JValueUnknown

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JDecisionWithVar.h>

class JValueUnknown : public JDecisionWithVar
{
public:

	JValueUnknown(const JVariableList* theVariableList, const JIndex variableIndex,
				  JFunction* arrayIndex);
	JValueUnknown(const JValueUnknown& source);

	virtual ~JValueUnknown();

	virtual JBoolean	Evaluate() const;
	virtual void		Print(ostream& output) const;
	virtual JDecision*	Copy() const;

private:

	// not allowed

	const JValueUnknown& operator=(const JValueUnknown& source);
};

#endif
