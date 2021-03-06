/******************************************************************************
 JXColorWheel.h

	Interface for the JXColorWheel class

	Copyright � 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXColorWheel
#define _H_JXColorWheel

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>
#include <jColor.h>

class JXSlider;
class JXImage;

class JXColorWheel : public JXWidget
{
public:

	JXColorWheel(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXColorWheel();

	JRGB	GetRGB() const;
	void	SetColor(const JRGB& rgb);

	JHSB	GetHSB() const;
	void	SetColor(const JHSB& hsb);

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, 
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	JHSB		itsColor;
	JXSlider*	itsBrightnessSlider;
	JXImage*	itsImage;
	JSize		itsLastDrawBrightness;

private:

	void	SetColor(const JPoint& pt);

	// not allowed

	JXColorWheel(const JXColorWheel& source);
	const JXColorWheel& operator=(const JXColorWheel& source);

public:

	// JBroadcaster messages

	static const JCharacter* kColorChanged;

	class ColorChanged : public JBroadcaster::Message
		{
		public:

			ColorChanged()
				:
				JBroadcaster::Message(kColorChanged)
				{ };
		};
};


/******************************************************************************
 RGB

 ******************************************************************************/

inline JRGB
JXColorWheel::GetRGB()
	const
{
	return JRGB(itsColor);
}

inline void
JXColorWheel::SetColor
	(
	const JRGB& rgb
	)
{
	JHSB hsb(rgb);
	SetColor(hsb);
}

/******************************************************************************
 HSB

 ******************************************************************************/

inline JHSB
JXColorWheel::GetHSB()
	const
{
	return itsColor;
}

#endif
