#include "EvemuPCH.h"

static int		EvePersistVarsCount = 30;
static const char * EvePersistVars[] = 
{
	"userid",
	"languageID",
	"regionid",
	"constellationid",
	"role",
	"charid",
	"allianceid",
	"corpid",
	"gangid",
	"gangrole",
	"gangbooster",
	"wingid",
	"squadid",
	"shipid",
	"stationid",
	"solarsystemid",
	"solarsystemid2",
	"address",
	"userType",
	"maxSessionTime",
	"hqID",
	"baseID",
	"rolesAtAll",
	"rolesAtHQ",
	"rolesAtBase",
	"rolesAtOther",
	"genderID",
	"bloodlineID",
	"raceID",
	"corpAccountKey"
};

static int		EveNonPersistVarsCount = 6;
static const char * EveNonpersistvars[] = {
	"sid",
	"c2ooid",
	"connectedObjects",
	"clientID",
	"locationid",
	"corprole"
};

/* eve session is a container for often used client info... */

void EveSession::init(uint32 sid, uint32 role)
{
	for (int i = 0; i < EvePersistVarsCount; i++)
	{
		mSessionDict[EvePersistVars[i]] = GetDefaultValueOfAttribute(EvePersistVars[i]);
	}
	mSessionDict["locationid"] = GetDefaultValueOfAttribute("locationid");
	mSessionDict["corprole"] = GetDefaultValueOfAttribute("corprole");

	/* fill the rest of the session info */

	/* small note: its unclear to me if are going to use all of this... */
	mSessionDict["version"] = 1;
	mSessionDict["sid"] = sid;
	mSessionDict["role"] = role;
	mSessionDict["c2ooid"] = 1;
	mSessionDict["connectedObjects"] = 1;
	mSessionDict["calltimes"] = 1;
	mSessionDict["notificationID"] = 0;
	mSessionDict["machoObjectsByID"] = 1;
	mSessionDict["machoObjectConnectionsByObjectID"] = 1;
	mSessionDict["sessionVariables"] = 1;
	mSessionDict["lastRemoteCall"] = getMSTime();
	mSessionDict["nextSessionChange"] = PyStreamNone;
	mSessionDict["sessionChangeReason"] = "Initial State";
	mSessionDict["callback"] = PyStreamNone;
	//mSessionDict["semaphore"] = 1;//uthread.SquidgySemaphore(('sessions', sid))
	//mSessionDict["sessionhist"] = 1;
	mSessionDict["hasproblems"] = 0;
	mSessionDict["mutating"] = 0;
	mSessionDict["changing"] = PyStreamNone;

	ChangeAttribute("role", role);
	//self.LogSessionHistory('Session created')
}

PySessionAttribute EveSession::GetDefaultValueOfAttribute(const char *attribute)
{
	PySessionAttribute ret;

	if (!strncmp(attribute, "role", 4))
	{
		ret = ROLE_LOGIN;
		return ret;
	}
	
	if (!strncmp(attribute, "corprole", 8) || !strncmp(attribute, "rolesAtAll", 10) || !strncmp(attribute, "rolesAtHQ", 9) || !strncmp(attribute, "rolesAtBase", 11) || !strncmp(attribute, "rolesAtOther", 12))
	{
		ret = ROLE_LOGIN;
		return ret;
	}

	ret = PyStreamNone;
	return ret;
}
