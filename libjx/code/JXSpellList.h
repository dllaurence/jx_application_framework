/******************************************************************************
 JXSpellList.h

	Copyright @ 1997 by Glenn W. Bach.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSpellList
#define _H_JXSpellList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXStringList.h>

class JXSpellList : public JXStringList
{

public:

	JXSpellList(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXSpellList();

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

private:

	// not allowed

	JXSpellList(const JXSpellList& source);
	const JXSpellList& operator=(const JXSpellList& source);

private:

	class SpellBase : public JBroadcaster::Message
		{
		public:

			SpellBase(const JCharacter* type, const JString& word)
				:
				JBroadcaster::Message(type),
				itsWord(word)
				{ };

			const JString&
			GetWord() const
				{
				return itsWord;
				};

		private:

			const JString& itsWord;
		};

public:

	// JBroadcaster messages

	static const JCharacter* kWordSelected;
	static const JCharacter* kReplaceWord;
	static const JCharacter* kReplaceWordAll;

	class WordSelected : public SpellBase
		{
		public:

			WordSelected(const JString& word)
				:
				SpellBase(kWordSelected, word)
				{ };
		};

	class ReplaceWord : public SpellBase
		{
		public:

			ReplaceWord(const JString& word)
				:
				SpellBase(kReplaceWord, word)
				{ };
		};

	class ReplaceWordAll : public SpellBase
		{
		public:

			ReplaceWordAll(const JString& word)
				:
				SpellBase(kReplaceWordAll, word)
				{ };
		};
};

#endif
