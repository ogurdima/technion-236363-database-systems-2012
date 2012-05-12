#include <stdio.h>
#include "wet.h"

#define WET__DBG

#ifdef WET__DBG
#define DEBUG(...) printf(__VA_ARGS__);
#endif
#ifndef WET__DBG
#define DEBUG(...) ;
#endif


void getPoints(const int personId)
{
	DEBUG("Called: getPoints\n");
}
void newMember(const int personId, const int companyId)
{
	DEBUG("Called: newMember\n");
}
void newInvitedMember(const int personId, const int companyId, const int invitedBy)
{
	DEBUG("Called: newInvitedMember\n");
}
void forbidMembership(const int personId, const int companyId)
{
	DEBUG("Called: forbidMembership\n");
}
void getTlcPerson(const int companyId)
{
	DEBUG("Called: getTlcPerson\n");
}
void getFriendliestPeople(const unsigned peopleNum)
{
	DEBUG("Called: getFriendliestPeople\n");
}
void getMembersAddresses(const int companyId)
{
	DEBUG("Called: getMembersAddresses\n");
}
void pyramidBonus(const int companyId, const unsigned height)
{
	DEBUG("Called: pyramidBonus\n");
}

