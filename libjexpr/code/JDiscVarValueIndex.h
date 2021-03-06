/******************************************************************************
 JDiscVarValueIndex.h

	Interface for JDiscVarValueIndex class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JDiscVarValueIndex
#define _H_JDiscVarValueIndex

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFunctionWithVar.h>

class JDiscVarValueIndex : public JFunctionWithVar
{
public:

	JDiscVarValueIndex(const JVariableList* theVariableList,
					   const JIndex variableIndex, JFunction* arrayIndex);
	JDiscVarValueIndex(const JDiscVarValueIndex& source);

	virtual ~JDiscVarValueIndex();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual void		Print(ostream& output) const;
	virtual JFunction*	Copy() const;

	virtual void	SetVariableIndex(const JIndex variableIndex);

private:

	// not allowed

	const JDiscVarValueIndex& operator=(const JDiscVarValueIndex& source);
};

#endif
