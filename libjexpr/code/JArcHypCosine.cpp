/******************************************************************************
 JArcHypCosine.cpp

	This class calculates the inverse hyperbolic cosine of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright � 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JArcHypCosine.h>
#include <jErrno.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JArcHypCosine::JArcHypCosine()
	:
	JUnaryFunction(kJArcHypCosineNameIndex, kJArcHypCosineType)
{
}

JArcHypCosine::JArcHypCosine
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJArcHypCosineNameIndex, kJArcHypCosineType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JArcHypCosine::~JArcHypCosine()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JArcHypCosine::JArcHypCosine
	(
	const JArcHypCosine& source
	)
	:
	JUnaryFunction(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JArcHypCosine::Copy()
	const
{
	JArcHypCosine* newFunction = new JArcHypCosine(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JArcHypCosine::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat argValue;
	if (!(GetArg())->Evaluate(&argValue))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = JACosh(argValue);
	return jerrno_is_clear();
}

JBoolean
JArcHypCosine::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex argValue;
	if (!(GetArg())->Evaluate(&argValue))
		{
		return kJFalse;
		}
	jclear_errno();
	*result = JACosh(argValue);
	return jerrno_is_clear();
}
