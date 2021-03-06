/******************************************************************************
 jVCSUtil.cpp

	Routines to help with Version Control Systems.

	Copyright � 2005 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jVCSUtil.h>
#include <JRegex.h>
#include <JSimpleProcess.h>
#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jXMLUtil.h>
#include <libxml/parser.h>
#include <strstream>
#include <jGlobals.h>
#include <jAssert.h>

static const JCharacter* kGitDirName         = ".git";
static const JCharacter* kSubversionDirName  = ".svn";
static const JCharacter* kCVSDirName         = "CVS";
static const JCharacter* kSCCSDirName        = "SCCS";

static const JCharacter* kDirName[] =
{
	kGitDirName,
	kSubversionDirName,
	kCVSDirName,
	kSCCSDirName
};

const JSize kDirCount = sizeof(kDirName) / sizeof(const JCharacter*);

// error types

const JCharacter* kJUnsupportedVCS = "JUnsupportedVCS";

/******************************************************************************
 JIsVCSDirectory

 ******************************************************************************/

JBoolean
JIsVCSDirectory
	(
	const JCharacter* name
	)
{
	return JI2B(strcmp(name, kGitDirName)        == 0 ||
				strcmp(name, kSubversionDirName) == 0 ||
				strcmp(name, kCVSDirName)        == 0 ||
				strcmp(name, kSCCSDirName)       == 0);
}

/******************************************************************************
 JGetVCSDirectoryNames

 ******************************************************************************/

JSize
JGetVCSDirectoryNames
	(
	const JCharacter*** dirNames
	)
{
	*dirNames = kDirName;
	return kDirCount;
}

/******************************************************************************
 JGetVCSType

 ******************************************************************************/

JVCSType
JGetVCSType
	(
	const JCharacter* path
	)
{
	JString p = path, n;
	if (JFileExists(path) ||
		!JDirectoryExists(path))	// broken link
		{
		JSplitPathAndName(path, &p, &n);
		}

	JString vcsDir = JCombinePathAndName(p, kSubversionDirName);
	if (JDirectoryExists(vcsDir))
		{
		return kJSVNType;
		}

	vcsDir = JCombinePathAndName(p, kCVSDirName);
	if (JDirectoryExists(vcsDir))
		{
		return kJCVSType;
		}

	vcsDir = JCombinePathAndName(p, kSCCSDirName);
	if (JDirectoryExists(vcsDir))
		{
		return kJSCCSType;
		}

	// check git last, since it needs to search directory tree up to root

	do
		{
		vcsDir = JCombinePathAndName(p, kGitDirName);
		if (JDirectoryExists(vcsDir))
			{
			return kJGitType;
			}

		JSplitPathAndName(p, &p, &n);
		}
		while (!JIsRootDirectory(p));

	return kJUnknownVCSType;
}

/******************************************************************************
 JIsManagedByVCS

 ******************************************************************************/

JBoolean
JIsManagedByVCS
	(
	const JCharacter*	fullName,
	JVCSType*			returnType
	)
{
	JVCSType type = JGetVCSType(fullName);
	if (returnType != NULL)
		{
		*returnType = type;
		}

	if (type == kJSVNType)
		{
		JString path, name, entriesFileName, data, pattern;
		JSplitPathAndName(fullName, &path, &name);
		entriesFileName = JCombinePathAndName(path, ".svn");
		entriesFileName = JCombinePathAndName(entriesFileName, "entries");
		JReadFile(entriesFileName, &data);

		if (data.BeginsWith("<?xml"))
			{
			pattern = "<entry[^>]+name=\"" + JRegex::BackslashForLiteral(name) + "\"(.|\n)*?>";
			JRegex r(pattern);
			return r.Match(data);
			}
		else
			{
			std::istrstream input(data, data.GetLength());

			const JString version = JReadLine(input);
			if (version == "8" || version == "9" || version == "10")
				{
				pattern = "\n\f\n" + name + "\n";

				JBoolean found;
				JIgnoreUntil(input, pattern, &found);
				return found;
				}
			}
		}
	else if (type == kJGitType)
		{
		return kJTrue;	// TODO: ask git (until then, better safe than sorry)
		}

	return kJFalse;
}

/******************************************************************************
 JEditVCS

 ******************************************************************************/

void
JEditVCS
	(
	const JCharacter* fullName
	)
{
	if (JFileExists(fullName) && !JFileWritable(fullName))
		{
		JString path, name;
		JSplitPathAndName(fullName, &path, &name);

		JString vcsDir = JCombinePathAndName(path, kCVSDirName);
		if (JDirectoryExists(vcsDir))
			{
			const JString cmd = "cd " + path + "; cvs edit " + name;
			system(cmd);
			}
		}
}

/******************************************************************************
 JRenameVCS

 ******************************************************************************/

JError
JRenameVCS
	(
	const JCharacter* oldFullName,
	const JCharacter* newFullName
	)
{
	if (!JNameUsed(oldFullName))
		{
		return JDirEntryDoesNotExist(oldFullName);
		}

	JString oldPath, newPath, name;
	JSplitPathAndName(newFullName, &newPath, &name);
	JSplitPathAndName(oldFullName, &oldPath, &name);	// must be second

	const JString origPath = JGetCurrentDirectory();
	if (JChangeDirectory(oldPath) != kJNoError)
		{
		return JAccessDenied(oldPath);
		}

	JVCSType type1    = JGetVCSType(oldPath);
	JVCSType type2    = JGetVCSType(newPath);
	JError err        = JNoError();
	JBoolean tryPlain = kJFalse;
	JString cmd;
	JProcess* p = NULL;
	if (type1 != type2)
		{
		tryPlain = kJTrue;
		}
	else if (type1 == kJSVNType || type1 == kJGitType)
		{
		if (type1 == kJSVNType)
			{
			cmd  = "svn mv --force ";
			cmd += JPrepArgForExec(oldFullName);
			cmd += " ";
			cmd += JPrepArgForExec(newFullName);
			}
		else if (type1 == kJGitType)
			{
			cmd  = "git mv -f ";
			cmd += JPrepArgForExec(name);
			cmd += " ";
			cmd += JPrepArgForExec(newFullName);
			}

		err = JProcess::Create(&p, cmd);
		if (err.OK())
			{
			p->WaitUntilFinished();
			}

		if (p != NULL && !p->SuccessfulFinish())
			{
			err      = JAccessDenied(oldFullName, newFullName);
			tryPlain = kJTrue;
			}
		}
	else if (type1 == kJUnknownVCSType)
		{
		tryPlain = kJTrue;
		}
	else
		{
		err = JUnsupportedVCS(oldFullName);
		}

	if (tryPlain && JProgramAvailable("mv"))
		{
		cmd  = "mv ";
		cmd += JPrepArgForExec(oldFullName);
		cmd += " ";
		cmd += JPrepArgForExec(newFullName);

		JSimpleProcess* p1;
		err = JSimpleProcess::Create(&p1, cmd);
		p   = p1;
		if (err.OK())
			{
			p->WaitUntilFinished();
			if (!p->SuccessfulFinish())
				{
				err = JAccessDenied(oldFullName, newFullName);
				}
			}
		}
	else if (tryPlain)
		{
		err = JRenameDirEntry(oldFullName, newFullName);
		}

	delete p;
	JChangeDirectory(origPath);
	return err;
}

/******************************************************************************
 JRemoveVCS

 ******************************************************************************/

JError
JRemoveVCS
	(
	const JCharacter*	fullName,
	const JBoolean		sync
	)
{
	if (!JNameUsed(fullName))
		{
		return JDirEntryDoesNotExist(fullName);
		}

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JString origPath = JGetCurrentDirectory();
	if (JChangeDirectory(path) != kJNoError)
		{
		return JAccessDenied(path);
		}

	JVCSType type     = JGetVCSType(path);
	JError err        = JNoError();
	JBoolean tryPlain = kJFalse;
	JString cmd;
	JProcess* p = NULL;
	if (type == kJSVNType || type == kJGitType)
		{
		const JCharacter *binary;
		if (type == kJCVSType)
			{
			binary = "cvs remove -f ";
			}
		else if (type == kJSVNType)
			{
			binary = "svn rm --force ";
			}
		else if (type == kJGitType)
			{
			binary = "git rm -rf ";
			}

		cmd  = binary;
		cmd += JPrepArgForExec(name);
		err  = JProcess::Create(&p, cmd);
		if (err.OK())
			{
			p->WaitUntilFinished();
			}

		if (p != NULL && !p->SuccessfulFinish())
			{
			err      = JAccessDenied(fullName);
			tryPlain = kJTrue;
			}
		}
	else if (type == kJUnknownVCSType)
		{
		tryPlain = kJTrue;
		}
	else
		{
		err = JUnsupportedVCS(fullName);
		}

	if (tryPlain && JKillDirectory(fullName, sync))
		{
		err = JNoError();
		}
	else if (tryPlain)
		{
		err = JAccessDenied(fullName);
		}

	delete p;
	JChangeDirectory(origPath);
	return err;
}

/******************************************************************************
 JGetVCSRepositoryPath

 ******************************************************************************/

static const JRegex svn4RepositoryPattern1 = "<entry[^>]+name=\"\"(.|\n)*?>";
static const JRegex svn4RepositoryPattern2 = "<entry[^>]+url=\"([^\"]+)\"";

JBoolean
JGetVCSRepositoryPath
	(
	const JCharacter*	origPath,
	JString*			repoPath
	)
{
	JString path = origPath, name;
	if (JFileExists(origPath) ||
		!JDirectoryExists(origPath))	// broken link
		{
		JSplitPathAndName(origPath, &path, &name);
		}

	const JVCSType type = JGetVCSType(path);
	JBoolean found      = kJFalse;
	if (type == kJCVSType)
		{
		const JString cvsPath = JCombinePathAndName(path, kCVSDirName);

		JString fullName = JCombinePathAndName(cvsPath, "Root");
		JReadFile(fullName, repoPath);

		fullName = JCombinePathAndName(cvsPath, "Repository");
		JString repo;
		JReadFile(fullName, &repo);

		if (!repoPath->IsEmpty() && !repo.IsEmpty())
			{
			*repoPath = JCombinePathAndName(*repoPath, repo);
			found = kJTrue;
			}
		}
	else if (type == kJSVNType)
		{
		JString entriesFileName, data;
		entriesFileName = JCombinePathAndName(path, kSubversionDirName);
		entriesFileName = JCombinePathAndName(entriesFileName, "entries");
		JReadFile(entriesFileName, &data);

		if (data.BeginsWith("<?xml"))
			{
			JIndexRange range;
			JArray<JIndexRange> matchList;
			if (svn4RepositoryPattern1.Match(data, &range) &&
				svn4RepositoryPattern2.MatchWithin(data, range, &matchList))
				{
				*repoPath = data.GetSubstring(matchList.GetElement(2));
				found = kJTrue;
				}
			}
		else
			{
			std::istrstream input(data, data.GetLength());

			const JString version = JReadLine(input);
			if (version == "8" || version == "9" || version == "10")
				{
				JIgnoreLine(input);		// ???
				JIgnoreLine(input);		// dir
				JIgnoreLine(input);		// latest update version

				*repoPath = JReadLine(input);
				found     = JI2B(input.good());
				}
			}
		}

	if (found)
		{
		if (!name.IsEmpty())
			{
			*repoPath = JCombinePathAndName(*repoPath, name);
			}
		return kJTrue;
		}
	else
		{
		repoPath->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 JGetCurrentSVNRevision

 ******************************************************************************/

static const JRegex svn4RevisionPattern = "<entry[^>]+committed-rev=\"([^\"]+)\"";

JBoolean
JGetCurrentSVNRevision
	(
	const JCharacter*	fullName,
	JString*			rev
	)
{
	JString path, name, entriesFileName, data, pattern;
	JSplitPathAndName(fullName, &path, &name);
	entriesFileName = JCombinePathAndName(path, ".svn");
	entriesFileName = JCombinePathAndName(entriesFileName, "entries");
	JReadFile(entriesFileName, &data);

	if (data.BeginsWith("<?xml"))
		{
		pattern = "<entry[^>]+name=\"" + JRegex::BackslashForLiteral(name) + "\"(.|\n)*?>";
		JRegex r(pattern);
		JIndexRange range;
		JArray<JIndexRange> matchList;
		if (r.Match(data, &range) &&
			svn4RevisionPattern.MatchWithin(data, range, &matchList))
			{
			*rev = data.GetSubstring(matchList.GetElement(2));
			return kJTrue;
			}
		}
	else
		{
		std::istrstream input(data, data.GetLength());

		const JString version = JReadLine(input);
		if (version == "8" || version == "9" || version == "10")
			{
			pattern = "\n\f\n" + name + "\n";

			JBoolean found;
			JIgnoreUntil(input, pattern, &found);
			if (found)
				{
				const JString data2 = JReadUntil(input, '\f');
				std::istrstream input2(data2, data2.GetLength());

				JIgnoreLine(input2);		// file
				JIgnoreLine(input2);		// ???
				JIgnoreLine(input2);		// ???
				JIgnoreLine(input2);		// ???
				JIgnoreLine(input2);		// ???
				JIgnoreLine(input2);		// timestamp
				JIgnoreLine(input2);		// hash
				JIgnoreLine(input2);		// timestamp

				*rev = JReadLine(input2);
				if (input2.good())
					{
					return kJTrue;
					}
				}
			}
		}

	rev->Clear();
	return kJFalse;
}

/******************************************************************************
 JGetSVNEntryType

	*** This function makes a synchronous call to the central repository!
	*** It is only useful for command line interaction.

	If the process succeeds, *type is the entry's "kind":  file or dir

	If the process fails, the error output is returned in *error.

 ******************************************************************************/

JBoolean
JGetSVNEntryType
	(
	const JCharacter*	url,
	JString*			type,
	JString*			error
	)
{
	type->Clear();
	error->Clear();

	JString cmd = "svn info --xml " + JPrepArgForExec(url);
	JProcess* p;
	int fromFD, errFD;
	JError err = JProcess::Create(&p, cmd, kJIgnoreConnection, NULL,
								  kJCreatePipe, &fromFD, kJCreatePipe, &errFD);
	if (!err.OK())
		{
		err.ReportIfError();
		return kJFalse;
		}

	p->WaitUntilFinished();
	if (p->SuccessfulFinish())
		{
		xmlDoc* doc = xmlReadFd(fromFD, NULL, NULL, XML_PARSE_NOBLANKS | XML_PARSE_NOCDATA);
		close(fromFD);
		if (doc != NULL)
			{
			xmlNode* root = xmlDocGetRootElement(doc);

			if (root != NULL &&
				strcmp((char*) root->name, "info") == 0 &&
				strcmp((char*) root->children->name, "entry") == 0)
				{
				*type = JGetXMLNodeAttr(root->children, "kind");
				return kJTrue;
				}
			}
		}

	JReadAll(errFD, error, kJTrue);
	return kJFalse;
}

/******************************************************************************
 JUpdateCVSIgnore

 ******************************************************************************/

void
JUpdateCVSIgnore
	(
	const JCharacter* ignoreFullName
	)
{
	JString path, name;
	JSplitPathAndName(ignoreFullName, &path, &name);
	const JString cvsFile = JCombinePathAndName(path, ".cvsignore");

	if (!JFileExists(cvsFile))
		{
		// We cannot just check for CVS, because they might be planning to
		// add it to CVS later.

		const JVCSType type = JGetVCSType(path);
		if (type != kJUnknownVCSType && type != kJCVSType)
			{
			return;
			}
		}

	JString cvsData;
	JReadFile(cvsFile, &cvsData);

	name += "\n";
	if (!cvsData.Contains(name))
		{
		JEditVCS(cvsFile);

		// append new data

		if (!cvsData.IsEmpty() && !cvsData.EndsWith("\n"))
			{
			cvsData += "\n";
			}
		cvsData += name;

		// write result

		ofstream output(cvsFile);
		cvsData.Print(output);
		}
}

/******************************************************************************
 JUnsupportedVCS

 ******************************************************************************/

JUnsupportedVCS::JUnsupportedVCS
	(
	const JCharacter* fullName
	)
	:
	JError(kJUnsupportedVCS, "")
{
	const JCharacter* map[] =
		{
		"file", fullName
		};
	SetMessage(map, sizeof(map));
}
