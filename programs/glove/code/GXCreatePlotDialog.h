/******************************************************************************
 GXCreatePlotDialog.h

	Interface for the GXCreatePlotDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GXCreatePlotDialog
#define _H_GXCreatePlotDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXTextMenu;
class JXTextCheckbox;
class GRaggedFloatTableData;
class GXDataDocument;
class JXInputField;
class JString;

class GXCreatePlotDialog : public JXDialogDirector
{
public:

	GXCreatePlotDialog(GXDataDocument* supervisor, GRaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);

	virtual ~GXCreatePlotDialog();
	
	void GetColumns(JIndex* startX, JIndex* startXErr, 
					JIndex* startY, JIndex* startYErr);
					
	JBoolean GetPlotIndex(JIndex* index);
	
	const JString& GetLabel();
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual JBoolean	OKToDeactivate();
	
private:

	JIndex itsStartX;
	JIndex itsStartXErr;
	JIndex itsStartY;
	JIndex itsStartYErr;
	JIndex itsPlotIndex;
	GXDataDocument*	itsTableDir;

// begin JXLayout

    JXTextMenu*   itsXMenu;
    JXTextMenu*   itsYMenu;
    JXTextMenu*   itsXErrMenu;
    JXTextMenu*   itsYErrMenu;
    JXTextMenu*   itsPlotsMenu;
    JXInputField* itsLabelInput;

// end JXLayout

private:

	void	BuildWindow(GRaggedFloatTableData* data, 
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);

	// not allowed

	GXCreatePlotDialog(const GXCreatePlotDialog& source);
	const GXCreatePlotDialog& operator=(const GXCreatePlotDialog& source);
};

#endif
