/******************************************************************************
 GCLRecurrence.h

	Interface for the GCLRecurrence class

	Copyright � 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GCLRecurrence
#define _H_GCLRecurrence

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "GCLUtil.h"

class GCLRecurrence
{
public:

	GCLRecurrence();
	GCLRecurrence(istream& is);
	GCLRecurrence(const GCLRecurrence& source);
	virtual ~GCLRecurrence();

	void	WriteSetup(ostream& os);

	void	Set(const GCLRecurrence& source);

	JBoolean	IsSkippingDay(const CLDay day);
	void		ShouldSkipDay(const CLDay day);

	CLRecurrenceType	type;
	CLDay				endDay;
	CLRecurrenceFrame	frame;
	JIndex				skipInterval;
	JBoolean			days[7];
	JBoolean			useOrd;
	JIndex				date;
	JIndex				ord;
	JIndex				day;
	CLMonth				month;

	const GCLRecurrence& operator=(const GCLRecurrence& source);

private:

	JArray<CLDay>*		itsSkipDays;

private:


};

#endif
