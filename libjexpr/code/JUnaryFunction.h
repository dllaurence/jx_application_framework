/******************************************************************************
 JUnaryFunction.h

	Interface for JUnaryFunction class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JUnaryFunction
#define _H_JUnaryFunction

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFunctionWithArgs.h>

class JUnaryFunction : public JFunctionWithArgs
{
public:

	JUnaryFunction(const JFnNameIndex nameIndex, const JFunctionType type);
	JUnaryFunction(JFunction* arg, const JFnNameIndex nameIndex, const JFunctionType type);
	JUnaryFunction(const JUnaryFunction& source);

	virtual ~JUnaryFunction();

	virtual JBoolean	SameAs(const JFunction& theFunction) const;

	virtual JSize				GetArgCount() const;
	virtual const JFunction*	GetArg(const JIndex index) const;
	virtual JFunction*			GetArg(const JIndex index);
	virtual void				SetArg(const JIndex index, JFunction* arg);

	const JFunction*	GetArg() const;
	JFunction*			GetArg();
	void				SetArg(JFunction* arg);

	// provides safe downcasting

	virtual JUnaryFunction*			CastToJUnaryFunction();
	virtual const JUnaryFunction*	CastToJUnaryFunction() const;

private:

	JFunction*	itsArg;

private:

	// not allowed

	const JUnaryFunction& operator=(const JUnaryFunction& source);
};

/******************************************************************************
 GetArg

 ******************************************************************************/

inline const JFunction*
JUnaryFunction::GetArg()
	const
{
	return itsArg;
}

inline JFunction*
JUnaryFunction::GetArg()
{
	return itsArg;
}

/******************************************************************************
 SetArg

 ******************************************************************************/

inline void
JUnaryFunction::SetArg
	(
	JFunction* arg
	)
{
	delete itsArg;
	itsArg = arg;
}

#endif
