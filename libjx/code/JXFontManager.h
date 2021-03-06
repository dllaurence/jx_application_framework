/******************************************************************************
 JXFontManager.h

	Interface for the JXFontManager class.

	Copyright � 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFontManager
#define _H_JXFontManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFontManager.h>
#include <JArray.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

class JString;
class JRegex;
class JXDisplay;

typedef JOrderedSetT::CompareResult
	(*JSortXFontNamesFn)(JString * const &, JString * const &);

class JXFontManager : public JFontManager
{
public:

	enum FontType
	{
		kStdType,
		kTrueType
	};

	struct XFont
	{
		FontType		type;
		XFontStruct*	xfstd;
		XftFont*		xftrue;

		XFont()
			:
			type(kStdType), xfstd(NULL), xftrue(NULL)
		{ };

		void Free(JXDisplay* display);
	};

public:

	JXFontManager(JXDisplay* display);

	virtual ~JXFontManager();

	virtual void		GetFontNames(JPtrArray<JString>* fontNames) const;
	virtual void		GetMonospaceFontNames(JPtrArray<JString>* fontNames) const;
	virtual JBoolean	GetFontSizes(const JCharacter* name, JSize* minSize,
									 JSize* maxSize, JArray<JSize>* sizeList) const;

	virtual JFontID				GetFontID(const JCharacter* name, const JSize size,
										  const JFontStyle& style) const;
	virtual const JCharacter*	GetFontName(const JFontID id) const;
	virtual JBoolean			IsExact(const JFontID id) const;

	virtual JSize	GetLineHeight(const JFontID fontID, const JSize size,
								  const JFontStyle& style,
								  JCoordinate* ascent, JCoordinate* descent) const;

	virtual JSize	GetCharWidth(const JFontID fontID, const JSize size,
								 const JFontStyle& style, const JCharacter c) const;

	virtual JSize	GetStringWidth(const JFontID fontID, const JSize size,
								   const JFontStyle& style,
								   const JCharacter* str, const JSize charCount) const;

	// for X Windows only

	void		GetXFontNames(const JRegex& regex,
							  JPtrArray<JString>* fontNames,
							  JSortXFontNamesFn compare = NULL) const;
	JBoolean	GetFontID(const JCharacter* xFontStr, JFontID* fontID) const;
	XFont		GetXFontInfo(const JFontID id) const;

private:

	struct FontInfo
	{
		JString*	name;
		JSize		size;
		JFontStyle	style;
		XFont		xfont;
		JBoolean	exact;		// kJTrue => exact match to requested specs
		JCoordinate	ascent;		// cache for TrueType; 0 until first computed
		JCoordinate	descent;	// cache for TrueType; 0 until first computed
		JCoordinate	monoWidth;	// 0 if not monospace

		FontInfo()
			:
			name(NULL), size(0), exact(kJFalse),
			 ascent(0), descent(0), monoWidth(0)
		{ };
	};

private:

	JXDisplay*			itsDisplay;
	JArray<FontInfo>*	itsFontList;

	mutable JPtrArray<JString>*	itsAllFontNames;	// can be NULL
	mutable JPtrArray<JString>*	itsMonoFontNames;	// can be NULL

private:

	JString		BuildStdFontName(const JCharacter* xName, const JSize size,
								 const JFontStyle& style, const JCharacter* italicStr,
								 const JCharacter* iso) const;
	JBoolean	BuildTrueTypeFontName(const JCharacter* xName,
									  const JSize size, const JFontStyle& style,
									  JString* xFontStr) const;

	JBoolean	GetNewFont(const JCharacter* name, const JSize size,
						   const JFontStyle& style, XFont* xfont) const;
	void		ApproximateFont(const JCharacter* name, const JSize size,
								const JFontStyle& style, XFont* xfont) const;

	JString		ConvertToXFontName(const JCharacter* name) const;
	void		ConvertToPSFontName(JString* name) const;

	JCoordinate	IsMonospace(const XFont& xfont) const;
	JBoolean	IsPostscript(const JString& name) const;
	JBoolean	IsUseless(const JString& name) const;

	// not allowed

	JXFontManager(const JXFontManager& source);
	const JXFontManager& operator=(const JXFontManager& source);
};

#endif
