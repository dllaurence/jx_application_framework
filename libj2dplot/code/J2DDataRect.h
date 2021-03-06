/*********************************************************************************
 J2DDataRect.h

	Copyright � 1999 John Lindal. All rights reserved.

 ********************************************************************************/

#ifndef _H_J2DDataRect
#define _H_J2DDataRect

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

struct J2DDataRect
{
	JFloat xmin, xmax, ymin, ymax;

	J2DDataRect()
		:
		xmin(0.0), xmax(0.0), ymin(0.0), ymax(0.0)
	{ };

	J2DDataRect(const JFloat xn, const JFloat xx,
				const JFloat yn, const JFloat yx)
		:
		xmin(xn), xmax(xx), ymin(yn), ymax(yx)
	{ };

	JBoolean
	Contains(const JFloat x, const JFloat y) const
	{
		return JI2B(xmin <= x && x <= xmax &&
					ymin <= y && y <= ymax);
	};
};

#endif
