/******************************************************************************
 JPhaseAngle.h

	Interface for JPhaseAngle class.

	Copyright � 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPhaseAngle
#define _H_JPhaseAngle

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JPhaseAngle : public JUnaryFunction
{
public:

	JPhaseAngle();
	JPhaseAngle(JFunction* arg);
	JPhaseAngle(const JPhaseAngle& source);

	virtual ~JPhaseAngle();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JPhaseAngle& operator=(const JPhaseAngle& source);
};

#endif
