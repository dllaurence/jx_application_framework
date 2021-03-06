/******************************************************************************
 JFunctionComparison.h

	Interface for JFunctionComparison class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFunctionComparison
#define _H_JFunctionComparison

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBinaryDecision.h>

class JFunction;

class JFunctionComparison : public JBinaryDecision<JFunction>
{
public:

	JFunctionComparison(const JCharacter* name, const JDecisionType type);
	JFunctionComparison(JFunction* arg1, JFunction* arg2,
						const JCharacter* name, const JDecisionType type);
	JFunctionComparison(const JFunctionComparison& source);

	virtual ~JFunctionComparison();

	virtual void	BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

	// provides safe downcasting

	virtual JFunctionComparison*		CastToJFunctionComparison();
	virtual const JFunctionComparison*	CastToJFunctionComparison() const;

private:

	// not allowed

	const JFunctionComparison& operator=(const JFunctionComparison& source);
};

#endif
