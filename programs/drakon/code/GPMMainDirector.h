/******************************************************************************
 GPMMainDirector.h

	Copyright � 2001 by New Planet Software. All rights reserved.

 *****************************************************************************/

#ifndef _H_GPMMainDirector
#define _H_GPMMainDirector

#include <JXWindowDirector.h>
#include <JPrefObject.h>

class JXToolBar;
class JXTabGroup;
class JXStaticText;
class JXTextMenu;
class JXTimerTask;

class GPMProcessList;
class GPMProcessTable;
class GPMProcessTreeList;

class GPMMainDirector : public JXWindowDirector, public JPrefObject
{
public:

public:

	GPMMainDirector(JXDirector* supervisor);
	virtual ~GPMMainDirector();

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsProcessMenu;
	JXTextMenu*	itsPrefsMenu;
	JXTextMenu*	itsHelpMenu;

	GPMProcessList*	itsList;
	JXTimerTask*	itsTimerTask;

	JXTabGroup*			itsTabGroup;
	GPMProcessTable*	itsProcessTable;
	GPMProcessTreeList*	itsProcessTree;

// begin JXLayout

    JXToolBar*    itsToolBar;
    JXStaticText* itsFullCmdDisplay;

// end JXLayout

private:

	void	BuildWindow();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateProcessMenu();
	void	HandleProcessMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UpdateHelpMenu();
	void	HandleHelpMenu(const JIndex index);

	void	ReadPrefs();
	void	WritePrefs();

	// not allowed

	GPMMainDirector(const GPMMainDirector& source);
	const GPMMainDirector& operator=(const GPMMainDirector& source);
};

#endif
