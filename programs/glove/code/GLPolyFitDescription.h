/******************************************************************************
 GLPolyFitDescription.h

	Interface for the GLPolyFitDescription class

	Copyright � 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GLPolyFitDescription
#define _H_GLPolyFitDescription

// Superclass Header
#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <GLFitDescription.h>



class GLPolyFitDescription : public GLFitDescription
{
public:

	GLPolyFitDescription(const JCharacter* name, const JArray<JIndex>& powers);
	GLPolyFitDescription(istream& is);
	
	virtual ~GLPolyFitDescription();

	void	GetPowers(JArray<JIndex>* powers) const;

	virtual void	WriteSetup(ostream& os); // must call base class first!
	
private:

	JArray<JIndex>*	itsPowers;

private:

	void	GLPolyFitDescriptionX();

	// not allowed

	GLPolyFitDescription(const GLPolyFitDescription& source);
	const GLPolyFitDescription& operator=(const GLPolyFitDescription& source);

};
#endif
