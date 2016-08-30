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

#define MAX_BUFFER_SIZE	(32)

forward OnPlayerLogin(playerid);
forward OnPlayerRegister(playerid);
forward OnPlayerDataLoad(playerid, entry[], value[]);
forward OnPlayerLogout(playerid);

enum E_USER_BUFFER
{
	E_BUFFER_NAME[33],
	E_BUFFER_TEXT[130]
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
	SUSER@USER_BUFFER[MAX_PLAYERS][MAX_BUFFER_SIZE][E_USER_BUFFER],
	SUSER@USER_BUFFER_ID[MAX_PLAYERS],
	SUSER@USER_COUNT;

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

#endif

public OnPlayerConnect(playerid)
{
	new
		tmp[E_USER_DATA];
	SUSER@USER_DATA[playerid] = tmp;
	
	new
		File: file,
		user_name[25],
		string[27],
		count,
		len;
	GetPlayerName(playerid, user_name, 25);
	file = fopen("susers/$USERS", io_read);
	while ((len = fread(file, string)))
	{
		if (isnull(string))
			continue;

		count++;
		string[len-2] = '\0';
		if (!strcmp(user_name, string, true))
		{
			SUSER@USER_DATA[playerid][E_USER_ID] = count;
			break;
		}
	}
	fclose(file);

	if (SUSER@USER_DATA[playerid][E_USER_ID] != 0)
	{
		new
			str[76];
		format(str, sizeof (str), "susers/%010d.ini", SUSER@USER_DATA[playerid][E_USER_ID]);

		file = fopen(str, io_read);
		while ((len = fread(file, str)))
		{
			if (isnull(str))
				continue;

			str[len-2] = '\0';
			if (!strcmp(str, "Password=", true, 9))
			{
				strcpy(SUSER@USER_DATA[playerid][E_USER_PASS], str[9], 65);
			}
			else if (!strcmp(str, "Salt=", true, 5))
			{
				strcpy(SUSER@USER_DATA[playerid][E_USER_SALT], str[5], 17);
				break;
			}
		}
		fclose(file);
	}
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
	new
		File: file;
	if (!fexist("susers/$USERS"))
	{
		file = fopen("susers/$USERS", io_write);
		if (!file)
		{
			print("[S_USERS] ERROR: \"susers\" folder does not exist!\7\7\7\7");
		}
		else fclose(file); print("[S_USER] 0 users have been found.");
	}
	else
	{
		new
			string[27];
		file = fopen("susers/$USERS", io_read);
		while (fread(file, string))
		{
			if (isnull(string))
				continue;

			SUSER@USER_COUNT++;
		}
		fclose(file);

		if (SUSER@USER_COUNT == 1)
			print("[S_USER] 1 user has been found.");
		else
			printf("[S_USER] %d users have been found.", SUSER@USER_COUNT);
	}
	return 1;
}

#if defined sscanf
	
#define SUSERS_SplitString(%0,%1,%2) \
	sscanf(%0, "p<=>s["#%2"]s[256]", %1, %0)

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
		return sscanf(value, "h", %1)

#else

static
	stock SUSERS_SplitString(source[], entry[], size)
{
	new pos = strfind(source, "=");
	if (pos == -1)
		return 1;

	strmid(entry, source, 0, pos, size);
	strdel(source, 0, pos+1);
	return 0;
}

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
		return %1 = strval(value)

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
		File: file,
		str[25];
	GetPlayerName(playerid, str, 25);

	file = fopen("susers/$USERS", io_append);
	fwrite(file, str);
	fwrite(file, "\r\n");
	fclose(file);

	SUSER@USER_COUNT++;
	SUSER@USER_DATA[playerid][E_USER_ID] = SUSER@USER_COUNT;
	format(str, sizeof (str), "susers/%010d.ini", SUSER@USER_DATA[playerid][E_USER_ID]);

	file = fopen(str, io_write);

	RandomSalt(SUSER@USER_DATA[playerid][E_USER_SALT], 16);
	SHA256_PassHash(password, SUSER@USER_DATA[playerid][E_USER_SALT], SUSER@USER_DATA[playerid][E_USER_PASS], 65);

	fwrite(file, "Password=");
	fwrite(file, SUSER@USER_DATA[playerid][E_USER_PASS]);

	fwrite(file, "\r\nSalt=");
	fwrite(file, SUSER@USER_DATA[playerid][E_USER_SALT]);
	fwrite(file, "\r\n");

	fclose(file);

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
		buf[161];
	SHA256_PassHash(password, SUSER@USER_DATA[playerid][E_USER_SALT], buf, 65);

	if (!strcmp(SUSER@USER_DATA[playerid][E_USER_PASS], buf, false))
	{
		format(buf, sizeof (buf), "susers/%010d.ini", SUSER@USER_DATA[playerid][E_USER_ID]);
		new 
			File: file = fopen(buf, io_read),
			entry[32],
			len;
		while ((len = fread(file, buf)))
		{
			if (isnull(buf))
				continue;

			buf[len-2] = '\0';
			SUSERS_SplitString(buf, entry, 33);
			CallLocalFunction("OnPlayerDataLoad", "dss", playerid, entry, buf);
		}
		fclose(file);

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
		File: file = fopen(buf, io_read),
		entry[32],
		buf[161],
		len;
	format(buf, sizeof (buf), "susers/%010d.ini", SUSER@USER_DATA[playerid][E_USER_ID]);
	while ((len = fread(file, buf)))
	{
		if (isnull(buf))
			continue;

		buf[len-2] = '\0';
		SUSERS_SplitString(buf, entry, 32);
		CallLocalFunction("OnPlayerDataLoad", "dss", playerid, entry, buf);
	}
	fclose(file);

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
	new
		tmp[35];
	format(tmp, sizeof (tmp), "0x%x", data);
	User_AddToBuffer(playerid, entry, tmp);
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
	stock User_AddToBuffer(playerid, const name[], const text[])
{
	new
		id = SUSER@USER_BUFFER_ID[playerid];

	if (id == 32)
	{
		User_WriteBuffer(playerid);
		User_AddToBuffer(playerid, name, text);
		return 1;
	}

	if (isnull(name) || isnull(text))
		return 0;

	if (strlen(name) > 32 || strlen(text) > 129)
		return 0;

	for (new i = 0; i < id; i++)
	{
		if (!strcmp(SUSER@USER_BUFFER[playerid][i][E_BUFFER_NAME], name, true))
		{
			strcpy(SUSER@USER_BUFFER[playerid][i][E_BUFFER_TEXT], text, 133);
			return 1;
		}
	}

	strcat(SUSER@USER_BUFFER[playerid][id][E_BUFFER_NAME], name, 33);
	strcat(SUSER@USER_BUFFER[playerid][id][E_BUFFER_TEXT], text, 130);

	SUSER@USER_BUFFER_ID[playerid]++;
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
	User_WriteBuffer(playerid)
{
	new
		str[22],
		File: file;
	format(str, sizeof (str), "susers/%010d.ini", SUSER@USER_DATA[playerid][E_USER_ID]);

	file = fopen(str, io_write);

	fwrite(file, "Password=");
	fwrite(file, SUSER@USER_DATA[playerid][E_USER_PASS]);

	fwrite(file, "\r\nSalt=");
	fwrite(file, SUSER@USER_DATA[playerid][E_USER_SALT]);
	fwrite(file, "\r\n");

	for (new i = 0; i < SUSER@USER_BUFFER_ID[playerid]; i++)
	{
		fwrite(file, SUSER@USER_BUFFER[playerid][i][E_BUFFER_NAME]);
		fwrite(file, "=");
		fwrite(file, SUSER@USER_BUFFER[playerid][i][E_BUFFER_TEXT]);
		fwrite(file, "\r\n");

		SUSER@USER_BUFFER[playerid][i][E_BUFFER_NAME][0] = '\0';
		SUSER@USER_BUFFER[playerid][i][E_BUFFER_TEXT][0] = '\0';
	}
	fclose(file);

	SUSER@USER_BUFFER_ID[playerid] = 0;
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

#if defined sscanf
	#undef SUSERS_SplitString
#endif
