/******************************************************************************
 JWebBrowser.h

	Copyright � 2000-2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JWebBrowser
#define _H_JWebBrowser

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JString.h>

class JWebBrowser
{
public:

	JWebBrowser();

	virtual ~JWebBrowser();

	void	ShowURL(const JCharacter* url);
	void	ShowFileContent(const JCharacter* fileName);
	void	ShowFileLocation(const JCharacter* fileName);
	void	ShowFileLocations(const JPtrArray<JString>& fileList);
	void	ComposeMail(const JCharacter* address);

	const JString&	GetShowURLCmd() const;
	void			SetShowURLCmd(const JCharacter* cmd);

	const JString&	GetShowFileContentCmd() const;
	void			SetShowFileContentCmd(const JCharacter* cmd);

	const JString&	GetShowFileLocationCmd() const;
	void			SetShowFileLocationCmd(const JCharacter* cmd);

	const JString&	GetComposeMailCmd() const;
	void			SetComposeMailCmd(const JCharacter* cmd);

	void	ReadConfig(istream& input);
	void	WriteConfig(ostream& output, const JFileVersion vers) const;

	static JFileVersion	GetCurrentConfigVersion();

	// upgrading from previous versions -- public because used by other classes

	static void	ConvertVarNames(JString* s, const JCharacter* varNameList);

protected:

	virtual void	SaveCommands();

private:

	JString	itsShowURLCmd;
	JString	itsShowFileContentCmd;
	JString	itsShowFileLocationCmd;
	JString	itsComposeMailCmd;

private:

	void	Exec(const JCharacter* cmd,
				 const JCharacter* varName, const JCharacter* value) const;

	// not allowed

	JWebBrowser(const JWebBrowser& source);
	const JWebBrowser& operator=(const JWebBrowser& source);
};


/******************************************************************************
 Get outsourced commands

 ******************************************************************************/

inline const JString&
JWebBrowser::GetShowURLCmd()
	const
{
	return itsShowURLCmd;
}

inline const JString&
JWebBrowser::GetShowFileContentCmd()
	const
{
	return itsShowFileContentCmd;
}

inline const JString&
JWebBrowser::GetShowFileLocationCmd()
	const
{
	return itsShowFileLocationCmd;
}

inline const JString&
JWebBrowser::GetComposeMailCmd()
	const
{
	return itsComposeMailCmd;
}

/******************************************************************************
 Set outsourced commands

 ******************************************************************************/

inline void
JWebBrowser::SetShowURLCmd
	(
	const JCharacter* cmd
	)
{
	itsShowURLCmd = cmd;
	SaveCommands();
}

inline void
JWebBrowser::SetShowFileContentCmd
	(
	const JCharacter* cmd
	)
{
	itsShowFileContentCmd = cmd;
	SaveCommands();
}

inline void
JWebBrowser::SetShowFileLocationCmd
	(
	const JCharacter* cmd
	)
{
	itsShowFileLocationCmd = cmd;
	SaveCommands();
}

inline void
JWebBrowser::SetComposeMailCmd
	(
	const JCharacter* cmd
	)
{
	itsComposeMailCmd = cmd;
	SaveCommands();
}

#endif
