/******************************************************************************
 JParallel.cpp

							The Parallel Values Class

	This class returns the result of 1/(1/arg1+1/arg2+...).

	Since || is not a standard operator, we have to act like both
	NaryFunction and NaryOperator.  Print returns parallel(arg1,arg2,...)
	and Render draws arg1||arg2||...  The parser only understands
	parallel(arg1,arg2,...).

	BASE CLASS = JNaryOperator

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JParallel.h>
#include <JExprRenderer.h>
#include <JExprRectList.h>
#include <JRect.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JParallel::JParallel()
	:
	JNaryOperator(kJParallelNameIndex, kJParallelType)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JParallel::~JParallel()
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JParallel::JParallel
	(
	const JParallel& source
	)
	:
	JNaryOperator(source)
{
}

/******************************************************************************
 Copy

	This provides a way to copy an object without knowing its exact class.

 ******************************************************************************/

JFunction*
JParallel::Copy()
	const
{
	JParallel* newFunction = new JParallel(*this);
	assert( newFunction != NULL );
	return newFunction;
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
JParallel::Evaluate
	(
	JFloat* result
	)
	const
{
	const JSize argCount = GetArgCount();
	if (argCount == 0)
		{
		return kJFalse;
		}

	*result = 0.0;
	for (JIndex i=1; i<=argCount; i++)
		{
		JFloat argValue;
		if (!(GetArg(i))->Evaluate(&argValue) || argValue == 0.0)
			{
			return kJFalse;
			}
		*result += 1.0/argValue;
		}

	if (*result != 0.0)
		{
		*result = 1.0/(*result);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
JParallel::Evaluate
	(
	JComplex* result
	)
	const
{
	const JSize argCount = GetArgCount();
	if (argCount == 0)
		{
		return kJFalse;
		}

	*result = 0.0;
	for (JIndex i=1; i<=argCount; i++)
		{
		JComplex argValue;
		if (!(GetArg(i))->Evaluate(&argValue) || argValue == 0.0)
			{
			return kJFalse;
			}
		*result += 1.0/argValue;
		}

	if (real(*result) != 0.0 || imag(*result) != 0.0)
		{
		*result = 1.0/(*result);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PrepareToRender

 ******************************************************************************/

JIndex
JParallel::PrepareToRender
	(
	const JExprRenderer&	renderer,
	const JPoint&			upperLeft,
	const JSize				fontSize,
	JExprRectList*			rectList
	)
{
	// intialize our rectangle and midline

	JPoint argUpperLeft = upperLeft;

	JRect ourRect(upperLeft, argUpperLeft);
	ourRect.bottom += renderer.GetLineHeight(fontSize);
	JCoordinate ourMidline = (ourRect.top + ourRect.bottom)/2;
	const JCoordinate origMidline = ourMidline;

	// get rectangle for each argument

	const JSize opWidth =
		2*(renderer.GetStringWidth(fontSize, " ") + renderer.GetVertBarWidth());
	const JSize argCount = GetArgCount();
	{
	for (JIndex i=1; i<=argCount; i++)
		{
		JFunction* arg = GetArg(i);
		const JIndex argIndex =
			arg->PrepareToRender(renderer, argUpperLeft, fontSize, rectList);
		JRect argRect  = rectList->GetRect(argIndex);
		argUpperLeft.x = argRect.right + opWidth;

		if (ParenthesizeArgForRender(*this, *arg))
			{
			const JSize parenWidth = renderer.GetParenthesisWidth(argRect.height());
			rectList->ShiftRect(argIndex, parenWidth, 0);
			argRect = rectList->GetRect(argIndex);
			argUpperLeft.x += 2*parenWidth;
			ourRect.right   = argRect.right + parenWidth;
			}

		ourRect = JCovering(ourRect, argRect);
		const JCoordinate argMidline = rectList->GetMidline(argIndex);
		if (argMidline > ourMidline)
			{
			ourMidline = argMidline;
			}
		}
	}

	// adjust the argument rectangles so all the midlines are the same
	// (ourMidline is guaranteed to stay constant)

	if (argCount > 1 && ourMidline > origMidline)
		{
		for (JIndex i=1; i<=argCount; i++)
			{
			const JFunction* arg = GetArg(i);
			JIndex argIndex;
			const JBoolean found = rectList->FindFunction(arg, &argIndex);
			assert( found );
			rectList->SetMidline(argIndex, ourMidline);
			ourRect = JCovering(ourRect, rectList->GetRect(argIndex));
			}
		}

	// save our rectangle

	return rectList->AddRect(ourRect, ourMidline, fontSize, this);
}

/******************************************************************************
 Render

 ******************************************************************************/

void
JParallel::Render
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

	const JSize spaceWidth = renderer.GetStringWidth(fontSize, " ");
	const JSize barWidth   = renderer.GetVertBarWidth();
	const JSize lineHeight = renderer.GetLineHeight(fontSize);

	JSize maxBarLength;
	if (((JSize) ourRect.height()) > lineHeight)
		{
		maxBarLength = JLFloor(ourRect.height()/2.0);
		}
	else
		{
		maxBarLength = ourRect.height();
		}
	JSize barLength;
	if (ourMidline > ourRect.ycenter())
		{
		barLength = 2*(ourRect.bottom - ourMidline);
		}
	else
		{
		barLength = 2*(ourMidline - ourRect.top);
		}
	if (barLength > maxBarLength)
		{
		barLength = maxBarLength;
		}
	const JCoordinate barTop = ourMidline - barLength/2;

	const JSize argCount = GetArgCount();
	for (JIndex i=1; i<=argCount; i++)
		{
		const JFunction* arg = GetArg(i);
		arg->Render(renderer, rectList);

		JIndex argIndex;
		const JBoolean found = rectList.FindFunction(arg, &argIndex);
		assert( found );
		const JRect argRect = rectList.GetRect(argIndex);
		JCoordinate h = argRect.right + spaceWidth;

		if (ParenthesizeArgForRender(*this, *arg))
			{
			renderer.DrawParentheses(argRect);
			h += renderer.GetParenthesisWidth(argRect.height());
			}

		if (i < argCount)
			{
			renderer.DrawVertBar(h, barTop, barLength);
			renderer.DrawVertBar(h + barWidth, barTop, barLength);
			}
		}
}
