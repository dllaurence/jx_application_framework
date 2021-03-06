/******************************************************************************
 jVCSUtil.h

	Copyright � 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jVCSUtil
#define _H_jVCSUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JError.h>

class JString;

enum JVCSType
{
	kJUnknownVCSType = 0,
	kJSCCSType,
	kJCVSType,
	kJSVNType,
	kJGitType
};

class JUnsupportedVCS : public JError
{
public:

	JUnsupportedVCS(const JCharacter* fullName);
};

JBoolean	JIsVCSDirectory(const JCharacter* name);
JSize		JGetVCSDirectoryNames(const JCharacter*** dirNames);
JVCSType	JGetVCSType(const JCharacter* path);
JBoolean	JIsManagedByVCS(const JCharacter* fullName, JVCSType* returnType = NULL);

void		JEditVCS(const JCharacter* fullName);
JError		JRenameVCS(const JCharacter* oldFullName, const JCharacter* newFullName);
JError		JRemoveVCS(const JCharacter* fullName, const JBoolean sync = kJTrue);

JBoolean	JGetVCSRepositoryPath(const JCharacter* path, JString* repoPath);

// SVN specific

JBoolean	JGetCurrentSVNRevision(const JCharacter* fullName, JString* rev);
JBoolean	JGetSVNEntryType(const JCharacter* url, JString* type, JString* error);

// CVS specific

void		JUpdateCVSIgnore(const JCharacter* ignoreFullName);

#endif
