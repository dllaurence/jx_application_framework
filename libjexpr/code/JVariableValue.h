/******************************************************************************
 JVariableValue.h

	Interface for JVariableValue class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVariableValue
#define _H_JVariableValue

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFunctionWithVar.h>

class JVariableValue : public JFunctionWithVar
{
public:

	JVariableValue(const JVariableList* theVariableList, const JIndex variableIndex,
				   JFunction* arrayIndex);
	JVariableValue(const JVariableValue& source);

	virtual ~JVariableValue();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual void		Print(ostream& output) const;
	virtual JFunction*	Copy() const;

	virtual void	SetVariableIndex(const JIndex variableIndex);

private:

	// not allowed

	const JVariableValue& operator=(const JVariableValue& source);
};

#endif
