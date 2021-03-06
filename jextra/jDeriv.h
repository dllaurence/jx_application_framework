/******************************************************************************
 jDeriv.h

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jDeriv
#define _H_jDeriv

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFloat.h>

JFloat JCenterDiff2(const JFloat x, const JFloat dx, JFloat (fn)(const JFloat));
JFloat JCenterDiff4(const JFloat x, const JFloat dx, JFloat (fn)(const JFloat));
JFloat JCenterDiff6(const JFloat x, const JFloat dx, JFloat (fn)(const JFloat));

#endif
