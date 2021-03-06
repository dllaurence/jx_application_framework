/******************************************************************************
 JXFSBindingTable.h

	Copyright � 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFSBindingTable
#define _H_JXFSBindingTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXEditTable.h>

class JRegex;
class JXTextButton;
class JXTextMenu;
class JXInputField;
class JXColHeaderWidget;
class JFSBindingList;

class JXFSBindingTable : public JXEditTable
{
public:

	JXFSBindingTable(JFSBindingList* list,
					 JXTextButton* addButton, JXTextButton* removeButton,
					 JXTextButton* duplicateButton,
					 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual	~JXFSBindingTable();

	void	SetColTitles(JXColHeaderWidget* widget) const;
	void	SyncWithBindingList();

	virtual void		HandleKeyPress(const int key, const JXKeyModifiers& modifiers);
	virtual JBoolean	IsEditable(const JPoint& cell) const;

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell,
								  const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
									const JCoordinate x, const JCoordinate y,
									const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JFSBindingList*	itsBindingList;		// not owned
	JXTextMenu*		itsTypeMenu;
	JXTextButton*	itsAddButton;
	JXTextButton*	itsRemoveButton;
	JXTextButton*	itsDuplicateButton;
	JXInputField*	itsTextInput;
	JRegex*			itsTestRegex;

private:

	void	AddPattern();
	void	RemovePattern();
	void	DuplicatePattern();

	void	UpdateTypeMenu();
	void	HandleTypeMenu(const JIndex index);

	void	UpdateButtons();
	void	UpdateColWidths();

	// not allowed

	JXFSBindingTable(const JXFSBindingTable& source);
	const JXFSBindingTable& operator=(const JXFSBindingTable& source);

public:

	// JBroadcaster messages

	static const JCharacter* kDataChanged;

	class DataChanged : public JBroadcaster::Message
		{
		public:

			DataChanged()
				:
				JBroadcaster::Message(kDataChanged)
			{ };
		};
};

#endif
