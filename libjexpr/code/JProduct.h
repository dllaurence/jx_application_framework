/******************************************************************************
 JProduct.h

	Interface for JProduct class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JProduct
#define _H_JProduct

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JNaryOperator.h>

class JProduct : public JNaryOperator
{
public:

	JProduct();
	JProduct(const JProduct& source);

	virtual ~JProduct();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual void		Print(ostream& output) const;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;

private:

	// not allowed

	const JProduct& operator=(const JProduct& source);
};

#endif
