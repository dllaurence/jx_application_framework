/******************************************************************************
 GXChooseFileImportDialog.h

	Interface for the GXChooseFileImportDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GXChooseFileImportDialog
#define _H_GXChooseFileImportDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JPrefObject.h>

class JXTextMenu;
class JXTextButton;
class JXStaticText;
class GXDataDocument;
class JString;

class GXChooseFileImportDialog : public JXDialogDirector, public JPrefObject
{
public:

	GXChooseFileImportDialog(GXDataDocument* supervisor, const JString& filename);

	virtual ~GXChooseFileImportDialog();
	
	JIndex 			GetFilterIndex();
	const JString& 	GetFileText();
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;
	
private:

	JIndex 			itsFilterIndex;
	JXStaticText*	itsFileText;
	GXDataDocument*	itsDir;

// begin JXLayout

    JXTextMenu*   itsFilterMenu;
    JXTextButton* itsReloadButton;

// end JXLayout

private:

	void	BuildWindow(const JString& filename);

	// not allowed

	GXChooseFileImportDialog(const GXChooseFileImportDialog& source);
	const GXChooseFileImportDialog& operator=(const GXChooseFileImportDialog& source);
};

#endif
