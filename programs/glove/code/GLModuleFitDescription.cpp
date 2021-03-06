/******************************************************************************
 GLModuleFitDescription.cpp

	<Description>

	BASE CLASS = public GLFitDescription

	Copyright � 1999 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <glStdInc.h>
#include <GLModuleFitDescription.h>
#include "GLDLFitModule.h"
#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GLModuleFitDescription::GLModuleFitDescription
	(
	GLDLFitModule* fit
	)
	:
	GLFitDescription(GLFitDescription::kModule, fit->GetFunctionalForm(), fit->GetFitName()),
	itsModule(fit)
{
	const JSize count	= fit->GetParameterCount();
	SetParameterCount(count);
	for (JIndex i = 1; i <= count; i++)
		{
		GetVarList()->AddVariable(itsModule->GetParameterName(i), 0);
		}
	DoesRequireStartValues(kJTrue);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLModuleFitDescription::~GLModuleFitDescription()
{
}

