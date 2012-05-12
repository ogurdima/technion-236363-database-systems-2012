#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wet.h"

#define MAIN__DBG

#ifdef MAIN__DBG
#define DEBUG(...) printf(__VA_ARGS__);
#endif
#ifndef MAIN__DBG
#define DEBUG(...) ;
#endif


void testCallAllFunctions();


int main(int argc, char* argv[]) {
	DEBUG("Called: main\n");
	
	testCallAllFunctions();
	
	return 0;
}



void testCallAllFunctions()
{
	getPoints(0);
	newMember(0,0);
	newInvitedMember(0,0,0);
	forbidMembership(0,0);
	getTlcPerson(0);
	getFriendliestPeople(0);
	getMembersAddresses(0);
	pyramidBonus(0);
}
