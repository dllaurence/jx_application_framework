/******************************************************************************
 war.cpp

	WizWar server.

	Copyright � 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "WarSocket.h"
#include "warGlobals.h"
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <jErrno.h>
#include <jAssert.h>

// Constants

static const JCharacter* kVersionStr =

	"WizWar Server 0.0.1\n"
	"\n"
	"Copyright 1999 by John Lindal\n"
	"DO NOT DISTRIBUTE!";

// Notation

typedef ACE_Acceptor<WarSocket, ACE_SOCK_ACCEPTOR>	WarAcceptor;

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	WarCreateGlobals(argc, argv, kVersionStr);

	// create the server socket

	const JIndex port = (WarGetSetup())->GetServerPort();

	WarAcceptor acceptor;
	if (acceptor.open(ACE_INET_Addr(port)) == -1)
		{
		cerr << "error trying to open port " << port << ": " << jerrno() << endl;
		return 1;
		}

	// start the event loop

	ACE_Reactor::run_event_loop();

	// clean up

	WarDeleteGlobals();
	return 0;
}

/******************************************************************************
 Required templates

	These are defined because the linker asked for them.

 ******************************************************************************/

#undef new
#undef delete

// INET

#define JTemplateName ACE_Acceptor
#define JTemplateType WarSocket, ACE_SOCK_ACCEPTOR
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType
