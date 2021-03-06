/******************************************************************************
 JNegation.cpp

								The Unary Minus Class

	This class returns the negative of a JFunction.

	BASE CLASS = JUnaryFunction

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JNegation.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <jParserData.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JNegation::JNegation()
	:
	JUnaryFunction(kJSubtractionNameIndex, kJNegationType)
{
}

JNegation::JNegation
	(
	JFunction* arg
	)
	:
	JUnaryFunction(arg, kJSubtractionNameIndex, kJNegationType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JNegation::~JNegation()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JNegation::JNegation
	(
	const JNegation& source
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
JNegation::Copy()
	const
{
	JNegation* newFunction = new JNegation(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JNegation::Evaluate
	(
	JFloat* result
	)
	const
{
	JFloat value;
	if ((GetArg())->Evaluate(&value))
		{
		*result = -value;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JNegation::Evaluate
	(
	JComplex* result
	)
	const
{
	JComplex value;
	if ((GetArg())->Evaluate(&value))
		{
		*result = -value;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Print

 ******************************************************************************/

void
JNegation::Print
	(
	ostream& output
	)
	const
{
	output << JPGetSubtractionString();

	const JFunction* arg = GetArg();
	if (ParenthesizeArgForPrint(*this, *arg))
		{
		output << '(';
		arg->Print(output);
		output << ')';
		}
	else
		{
		arg->Print(output);
		}
}

/******************************************************************************
 PrepareToRender

 ******************************************************************************/

JIndex
JNegation::PrepareToRender
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// intialize our rectangle

	JPoint argUpperLeft = upperLeft;
	argUpperLeft.x += renderer.GetStringWidth(fontSize, JPGetSubtractionString());

	JFunction* arg = GetArg();
	const JFunctionType argType = arg->GetType();
	if (argType == kJDivisionType)
		{
		argUpperLeft.x += renderer.GetStringWidth(fontSize, " ");
		}

	JRect ourRect(upperLeft, argUpperLeft);
	ourRect.bottom += renderer.GetLineHeight(fontSize);

	// get rectangle for our argument

	const JIndex argIndex =
		arg->PrepareToRender(renderer, argUpperLeft, fontSize, rectList);
	JRect argRect = rectList->GetRect(argIndex);

	if (ParenthesizeArgForRender(*this, *arg))
		{
		const JSize parenWidth = renderer.GetParenthesisWidth(argRect.height());
		rectList->ShiftRect(argIndex, parenWidth, 0);
		argRect = rectList->GetRect(argIndex);
		ourRect.right = argRect.right + parenWidth;
		}

	ourRect = JCovering(ourRect, argRect);
	const JCoordinate ourMidline = rectList->GetMidline(argIndex);

	// save our rectangle

	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JNegation::Render
	(
	const JExprRenderer& renderer,
	const JExprRectList& rectList
	)
	const
{
	// find ourselves in the list

	JIndex ourIndex;
	const JBoolean found = rectList.FindFunction(this, &ourIndex);
	assert( found );

	const JRect ourRect = rectList.GetRect(ourIndex);
	const JCoordinate ourMidline = rectList.GetMidline(ourIndex);
	const JSize fontSize = rectList.GetFontSize(ourIndex);

	// draw ourselves

	renderer.DrawString(ourRect.left, ourMidline, fontSize, JPGetSubtractionString());

	const JFunction* arg = GetArg();
	arg->Render(renderer, rectList);

	if (ParenthesizeArgForRender(*this, *arg))
		{
		JIndex argIndex;
		const JBoolean found = rectList.FindFunction(arg, &argIndex);
		assert( found );
		renderer.DrawParentheses(rectList.GetRect(argIndex));
		}
}
