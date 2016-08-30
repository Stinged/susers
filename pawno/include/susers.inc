/*
	susers.inc ;
		Easy and simple user system!

	Created by Stinged
		on August 30, 2016
*/

#if defined SUSERS_SAVING_INI
	#include <susers/ini>
#elseif defined SUSERS_SAVING_MYSQL
	#include <susers/mysql>
#elseif defined SUSERS_SAVING_SQLITE
	#include <susers/sqlite>
#else
	#error Define a saving type first!
#endif

#if defined SUSERS_SAVING_INI && defined SUSERS_SAVING_MYSQL
	#error You cannot use two saving types
#elseif defined SUSERS_SAVING_INI && defined SUSERS_SAVING_SQLITE
	#error You cannot use two saving types
#elseif defined SUSERS_SAVING_MYSQL && defined SUSERS_SAVING_SQLITE
	#error You cannot use two saving types
#endif

#if defined SUSERS_SAVING_MYSQL && !defined mysql_included
	#error Include the mysql include first!
#endif

/*

native User_Register(playerid, password[]);
native User_IsRegistered(playerid);
native
native User_Login(playerid, password[]);
native User_ForceLogin(playerid);
native User_IsLoggedIn(playerid);
native
native User_LoadInt(entry[], &dest);
native User_LoadFloat(entry[], &dest);
native User_LoadString(entry[], dest[]);
native User_LoadHex(entry[], &dest);
native
native User_SaveInt(playerid, entry[], data);
native User_SaveFloat(playerid, entry[], Float: data, accuracy = 6);
native User_SaveString(playerid, entry[], data[]);
native User_SaveHex(playerid, entry[], data);
native
native **_SQLITE_ONLY_**(not_a_function);
native
native User_CreateField(name[], field_types: type, maxlength);
native User_CreateTable();

*/
