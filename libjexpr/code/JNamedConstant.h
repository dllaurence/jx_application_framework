/******************************************************************************
 JNamedConstant.h

	Interface for JNamedConstant class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JNamedConstant
#define _H_JNamedConstant

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFunction.h>

class JNamedConstant : public JFunction
{
public:

	JNamedConstant(const JNamedConstIndex nameIndex);
	JNamedConstant(const JNamedConstant& source);

	virtual ~JNamedConstant();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual void		Print(ostream& output) const;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;
	virtual JFunction*	Copy() const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

private:

	JNamedConstIndex	itsNameIndex;

private:

	// not allowed

	const JNamedConstant& operator=(const JNamedConstant& source);
};

#endif
