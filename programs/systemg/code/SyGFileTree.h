/******************************************************************************
 SyGFileTree.h

	Copyright � 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGFileTree
#define _H_SyGFileTree

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFSFileTree.h>
#include "SyGColUtils.h"	// need defn of GFMColType

class JDirInfo;
class SyGFileTreeNode;

class SyGFileTree : public JFSFileTree
{
public:

	SyGFileTree(SyGFileTreeNode* root);

	virtual ~SyGFileTree();

	const JString&	GetDirectory() const;
	JBoolean		Update(const JBoolean force = kJFalse,
						   JFSFileTreeNodeBase** updateNode = NULL);

	JBoolean	HiddenVisible() const;
	void		ShowHidden(const JBoolean show);

	void	SetWildcardFilter(const JCharacter* filter);
	void	ClearWildcardFilter();

	void	SetNodeCompareFunction(const GFMColType type);

	SyGFileTreeNode*		GetSyGRoot();
	const SyGFileTreeNode*	GetSyGRoot() const;

	JDirInfo*		GetRootDirInfo();
	const JDirInfo*	GetRootDirInfo() const;

private:

	// not allowed

	SyGFileTree(const SyGFileTree& source);
	const SyGFileTree& operator=(const SyGFileTree& source);
};

#endif
