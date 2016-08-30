/*
	susers.inc ;
		Easy and simple user system!

	Created by Stinged
		August 28, 2016

	INI Version!
*/

#if !defined isnull
	#define isnull(%1) \
		((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

#if !defined strcpy
	#define strcpy(%0,%1,%2) \
		strcat((%0[0] = '\0', %0), %1, %2)
#endif

#define MAX_BUFFER_SIZE 1024

forward OnPlayerLogin(playerid);
forward OnPlayerRegister(playerid);
forward OnPlayerDataLoad(playerid, entry[], value[]);
forward OnPlayerLogout(playerid);

enum field_types
{
	field_int,
	field_string,
	field_float
};

enum E_USER_DATA
{
	E_USER_ID,
	E_USER_PASS[65],
	E_USER_SALT[17],

	bool: E_USER_LOGGED_IN
};
static
	SUSER@USER_DATA[MAX_PLAYERS][E_USER_DATA],
	SUSER@USER_BUFFER[MAX_PLAYERS][MAX_BUFFER_SIZE],

	SUSER@TABLE_BUFFER[2048] = "CREATE TABLE IF NOT EXISTS s_users (id INTEGER PRIMARY KEY AUTOINCREMENT,name varchar(24) UNIQUE NOT NULL,pass char(64) NOT NULL,salt char(16) NOT NULL,",
	DB: SUSER@DB_HANDLE;

#if defined FILTERSCRIPT

public OnFilterScriptInit()
{
	SUSERS_ScriptInit();
	#if defined SUSER_OnFilterScriptInit
		SUSER_OnFilterScriptInit();
	#endif
	return 1;
}
#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit SUSER_OnFilterScriptInit
#if defined SUSER_OnFilterScriptInit
	forward SUSER_OnFilterScriptInit();
#endif

public OnFilterScriptExit()
{
	db_close(SUSER@DB_HANDLE);
	#if defined SUSER_OnFilterScriptExit
		SUSER_OnFilterScriptExit();
	#endif
	return 1;
}
#if defined _ALS_OnFilterScriptExit
	#undef OnFilterScriptExit
#else
	#define _ALS_OnFilterScriptExit
#endif
#define OnFilterScriptExit SUSER_OnFilterScriptExit
#if defined SUSER_OnFilterScriptExit
	forward SUSER_OnFilterScriptExit();
#endif

#else

public OnGameModeInit()
{
	SUSERS_ScriptInit();
	#if defined SUSER_OnGameModeInit
		SUSER_OnGameModeInit();
	#endif
	return 1;
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit SUSER_OnGameModeInit
#if defined SUSER_OnGameModeInit
	forward SUSER_OnGameModeInit();
#endif

public OnGameModeExit()
{
	db_close(SUSER@DB_HANDLE);
	#if defined SUSER_OnGameModeExit
		SUSER_OnGameModeExit();
	#endif
	return 1;
}
#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif
#define OnGameModeExit SUSER_OnGameModeExit
#if defined SUSER_OnGameModeExit
	forward SUSER_OnGameModeExit();
#endif

#endif

public OnPlayerConnect(playerid)
{
	new
		tmp[E_USER_DATA];
	SUSER@USER_DATA[playerid] = tmp;

	strcpy(SUSER@USER_BUFFER[playerid], "UPDATE s_users SET ", MAX_BUFFER_SIZE);

	new
		query[71],
		DBResult: result;
	GetPlayerName(playerid, query, 25);
	format(query, sizeof (query), "SELECT id,pass,salt FROM s_users WHERE name='%q'", query);

	result = db_query(SUSER@DB_HANDLE, query);
	if (db_num_rows(result))
	{
		SUSER@USER_DATA[playerid][E_USER_ID] = db_get_field_int(result, 0);
		db_get_field(result, 1, SUSER@USER_DATA[playerid][E_USER_PASS], 65);
		db_get_field(result, 2, SUSER@USER_DATA[playerid][E_USER_SALT], 17);
	}
	db_free_result(result);

	#if defined SUSER_OnPlayerConnect
		SUSER_OnPlayerConnect(playerid);
	#endif
	return 1;
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect SUSER_OnPlayerConnect
#if defined SUSER_OnPlayerConnect
	forward SUSER_OnPlayerConnect(playerid);
#endif

public OnPlayerDisconnect(playerid, reason)
{
	if (SUSER@USER_DATA[playerid][E_USER_LOGGED_IN] == true)
	{
		CallLocalFunction("OnPlayerLogout", "d", playerid);
		User_WriteBuffer(playerid);
	}
	#if defined SUSER_OnPlayerDisconnect
		SUSER_OnPlayerDisconnect(playerid, reason);
	#endif
	return 1;
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect SUSER_OnPlayerDisconnect
#if defined SUSER_OnPlayerDisconnect
	forward SUSER_OnPlayerDisconnect(playerid, reason);
#endif

static
	SUSERS_ScriptInit()
{
	SUSER@DB_HANDLE = db_open("susers.db");
	if (SUSER@DB_HANDLE != DB:0)
		print("[S_USERS] Connection to database was successful");
	else
		print("[S_USERS] ERROR: Could not connect to database");
	return 1;
}

#if defined sscanf
	
#define User_LoadInt(%0,%1) \
	if (!strcmp(entry, %0, true)) \
		return sscanf(value, "d", %1)

#define User_LoadFloat(%0,%1) \
	if (!strcmp(entry, %0, true)) \
		return sscanf(value, "f", %1)

#define User_LoadString(%0,%1,%2) \
	if (!strcmp(entry, %0, true)) \
		return sscanf(value, "s["#%2"]", %1)

#define User_LoadHex(%0,%1) \
	if (!strcmp(entry, %0, true)) \
		return sscanf(value, "d", %1)

#else

stock SUSERS_HexToInt(const string[])
{
    if(!string[0])
    	return 0;

    new
        res = 0,
        cur = 1,
        i = strlen(string);
    for( ; (--i) != -1; cur *= 16)
    {
        switch(string[i])
        {
            case '0'..'9': res += (cur * (string[i] - '0'));
            case 'a'..'z': res += (cur * (string[i] - 'a' + 10));
            case 'A'..'Z': res += (cur * (string[i] - 'A' + 10));
            default: return 0;
        }
    }
    return res;
}

#define User_LoadInt(%0,%1) \
	if (!strcmp(entry, %0, true)) \
		%1 = strval(value)

#define User_LoadFloat(%0,%1) \
	if (!strcmp(entry, %0, true)) \
		return _:(%1 = floatstr(value))

#define User_LoadString(%0,%1,%2) \
	if (!strcmp(entry, %0, true)) \
		return strcpy(%1, value, %2)

#define User_LoadHex(%0,%1) \
	if (!strcmp(entry, %0, true)) \
		return %1 = SUSERS_HexToInt(value[2])

#endif

stock User_CreateField(name[], field_types: type, maxlength = 10, def = 0)
{
	if ((strlen(SUSER@TABLE_BUFFER) + strlen(name) + 59 > 2047) || maxlength > 99999)
		return 0;

	new field_type[30];
	switch (_:type)
	{
		case field_int: format(field_type, sizeof (field_type), "int(%d) NOT NULL DEFAULT %d", maxlength, def);
		case field_string: format(field_type, sizeof (field_type), "varchar(%d)", maxlength);
		case field_float: format(field_type, sizeof (field_type), "float NOT NULL DEFAULT %d", def);
	}
	format(SUSER@TABLE_BUFFER, sizeof (SUSER@TABLE_BUFFER), "%s`%q` %s,", SUSER@TABLE_BUFFER, name, field_type);
	return 1;
}

stock User_CreateTable()
{
	new len = strlen(SUSER@TABLE_BUFFER) -1;
	if (SUSER@TABLE_BUFFER[len] == ',')
		SUSER@TABLE_BUFFER[len] = '\0';

	strcat(SUSER@TABLE_BUFFER, ")");
	db_query(SUSER@DB_HANDLE, SUSER@TABLE_BUFFER);
	return 1;
}

/*-----------------------------------------------------------------*\
	Function:
		User_IsRegistered

	Params:
		playerid
			Player who is being checked.

	Returns:
		-1
			Invalid playerid.
		0
			Player isn't registered.
		1
			Player is registered.

\*-----------------------------------------------------------------*/

stock User_IsRegistered(playerid)
{
	if (!(0 <= playerid < MAX_PLAYERS))
		return -1;
	return (SUSER@USER_DATA[playerid][E_USER_ID])?(1):(0);
}

/*-----------------------------------------------------------------*\
	Function:
		User_Register

	Params:
		playerid
			Player who is registering.
		password
			The password to register with.

	Returns:
		-1
			Name is already taken.
		0
			Invalid password.
			(must be between 3 and 32 characters)
		1
			Successfully registered.

\*-----------------------------------------------------------------*/

stock User_Register(playerid, password[])
{
	if (User_IsRegistered(playerid))
		return -1;

	if (!(3 <= strlen(password) <= 32))
		return 0;

	new
		query[159],
		user_name[25],
		DBResult: result;
	GetPlayerName(playerid, user_name, 25);

	RandomSalt(SUSER@USER_DATA[playerid][E_USER_SALT], 16);
	SHA256_PassHash(password, SUSER@USER_DATA[playerid][E_USER_SALT], SUSER@USER_DATA[playerid][E_USER_PASS], 65);
	
	format(query, sizeof (query), "INSERT into s_users (name,pass,salt) VALUES ('%q','%s','%s')", user_name, SUSER@USER_DATA[playerid][E_USER_PASS], SUSER@USER_DATA[playerid][E_USER_SALT]); 
	db_query(SUSER@DB_HANDLE, query);

	format(query, sizeof (query), "SELECT id FROM s_users WHERE name='%q'", user_name);
	result = db_query(SUSER@DB_HANDLE, query);
	SUSER@USER_DATA[playerid][E_USER_ID] = db_get_field_int(result, 0);
	db_free_result(result);

	SUSER@USER_DATA[playerid][E_USER_LOGGED_IN] = true;
	CallRemoteFunction("OnPlayerRegister", "d", playerid);
	return 1;
}

/*-----------------------------------------------------------------*\
	Function:
		User_Login

	Params:
		playerid
			Player who is logging in.
		password
			The password to login with.

	Returns:
		-1
			User is not registered.
		0
			Incorrect password.
		1
			Successfully logged in.

\*-----------------------------------------------------------------*/

stock User_Login(playerid, password[])
{
	if (!User_IsRegistered(playerid))
		return -1;

	if (isnull(password))
		return 0;

	new
		query[65];
	SHA256_PassHash(password, SUSER@USER_DATA[playerid][E_USER_SALT], query, 65);

	if (!strcmp(SUSER@USER_DATA[playerid][E_USER_PASS], query, false))
	{
		new
			DBResult: result;
		format(query, sizeof (query), "SELECT * FROM s_users WHERE id=%d", SUSER@USER_DATA[playerid][E_USER_ID]);
		result = db_query(SUSER@DB_HANDLE, query);

		if (db_num_rows(result))
		{
			new
				data[130];
			for (new i = 4, j = db_num_fields(result); i < j; i++)
			{
				db_field_name(result, i, query, 33);
				db_get_field(result, i, data, 130);
				CallLocalFunction("OnPlayerDataLoad", "dss", playerid, query, data);
			}
		}
		db_free_result(result);

		SUSER@USER_DATA[playerid][E_USER_LOGGED_IN] = true;
		CallLocalFunction("OnPlayerLogin", "d", playerid);
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------------*\
	Function:
		User_ForceLogin

	Params:
		playerid
			Player who is logging in.

	Returns:
		0
			User is not registered.
		1
			Successfully logged in.

\*-----------------------------------------------------------------*/

stock User_ForceLogin(playerid)
{
	if (!User_IsRegistered(playerid))
		return 0;

	new
		DBResult: result,
		query[42];
	format(query, sizeof (query), "SELECT * FROM s_users WHERE id=%d", SUSER@USER_DATA[playerid][E_USER_ID]);
	result = db_query(SUSER@DB_HANDLE, query);

	if (db_num_rows(result))
	{
		new
			data[130];
		for (new i = 4, j = db_num_fields(result); i < j; i++)
		{
			db_field_name(result, i, query, 33);
			db_get_field(result, i, data, 130);
			CallLocalFunction("OnPlayerDataLoad", "dss", playerid, query, data);
		}
	}
	db_free_result(result);

	SUSER@USER_DATA[playerid][E_USER_LOGGED_IN] = true;
	CallLocalFunction("OnPlayerLogin", "d", playerid);
	return 1;
}

/*-----------------------------------------------------------------*\
	Function:
		User_IsLoggedIn

	Params:
		playerid
			Player who is being checked.

	Returns:
		false
			Player is not logged in.
		true
			Player is logged in.

\*-----------------------------------------------------------------*/

stock bool: User_IsLoggedIn(playerid)
{
	return SUSER@USER_DATA[playerid][E_USER_LOGGED_IN];
}

/*-----------------------------------------------------------------*\
	Function:
		User_SaveInt

	Params:
		playerid
			Player whose data is being saved.
		entry
			Name of the entry to save the value under.
		data
			The integer to save.

\*-----------------------------------------------------------------*/

stock User_SaveInt(playerid, const entry[], const data)
{
	new
		tmp[11];
	valstr(tmp, data);
	User_AddToBuffer(playerid, entry, tmp);
}

/*-----------------------------------------------------------------*\
	Function:
		User_SaveFloat

	Params:
		playerid
			Player whose data is being saved.
		entry
			Name of the entry to save the value under.
		data
			The float to save.

\*-----------------------------------------------------------------*/

stock User_SaveFloat(playerid, const entry[], Float: data, accuracy = 6)
{
	new
		tmp[11];
	format(tmp, sizeof (tmp), "%.*f", accuracy, data);
	User_AddToBuffer(playerid, entry, tmp);
}

/*-----------------------------------------------------------------*\
	Function:
		User_SaveString

	Params:
		playerid
			Player whose data is being saved.
		entry
			Name of the entry to save the value under.
		data
			The string to save.

\*-----------------------------------------------------------------*/

stock User_SaveString(playerid, const entry[], const data[])
{
	User_AddToBuffer(playerid, entry, data);
}

/*-----------------------------------------------------------------*\
	Function:
		User_SaveHex

	Params:
		playerid
			Player whose data is being saved.
		entry
			Name of the entry to save the value under.
		data
			The hex to save.

\*-----------------------------------------------------------------*/

stock User_SaveHex(playerid, const entry[], const data)
{
	User_SaveInt(playerid, entry, data);
}

/*-----------------------------------------------------------------*\
	Function:
		User_AddToBuffer

	Params:
		playerid
			Player whose data is being saved.
		entry
			Name of the entry to save the value under.
		data
			The string to add to buffer.

\*-----------------------------------------------------------------*/

static
	stock User_AddToBuffer(playerid, const entry[], const data[])
{
	new
		len = strlen(SUSER@USER_BUFFER[playerid]),
		entry_len = strlen(entry),
		data_len = strlen(data);
	if (len + 6 + strlen(entry) + strlen(data) > MAX_BUFFER_SIZE-21)
	{
		len--;
		User_WriteBuffer(playerid, len);
		User_AddToBuffer(playerid, entry, data);
		return 1;
	}

	if (isnull(entry) || isnull(data))
		return 0;

	if (entry_len > 32 || data_len > 129)
		return 0;

	format(SUSER@USER_BUFFER[playerid], MAX_BUFFER_SIZE, "%s`%q`='%q',", SUSER@USER_BUFFER[playerid], entry, data);
	return 1;
}

/*-----------------------------------------------------------------*\
	Function:
		User_WriteBuffer

	Params:
		playerid
			Player whose data is being saved.
		len
			The length of the player's buffer.
			(optional: -1 to call strlen inside this function)

\*-----------------------------------------------------------------*/

static
	User_WriteBuffer(playerid, len = -1)
{
	if (len == -1)
		len = strlen(SUSER@USER_BUFFER[playerid]) - 1;
	if (SUSER@USER_BUFFER[playerid][len] == ',')
		SUSER@USER_BUFFER[playerid][len] = '\0';
	format(SUSER@USER_BUFFER[playerid], MAX_BUFFER_SIZE, "%s WHERE ID=%d", SUSER@USER_BUFFER[playerid], SUSER@USER_DATA[playerid][E_USER_ID]);
	db_query(SUSER@DB_HANDLE, SUSER@USER_BUFFER[playerid]);

	strcpy(SUSER@USER_BUFFER[playerid], "UPDATE s_users SET ", MAX_BUFFER_SIZE);
	return 1;
}

/*-----------------------------------------------------------------*\
	Function:
		RandomSalt

	Params:
		dest
			The array to save the salt in.
		len
			The length of the salt.
			(optional: default 10)

\*-----------------------------------------------------------------*/

static
	stock RandomSalt(dest[], len = 10)
{
	while (len--) dest[len] = random(2) ? (random(6) + 'A') : (random(9) + '0');
}
