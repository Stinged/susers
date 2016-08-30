#include <a_samp>
#undef MAX_PLAYERS
#define MAX_PLAYERS 32

//#include <sscanf2>
/*
	Uncomment to include sscanf2
	to take advantage of its speed
	
	note:
		sscanf2 was created by Y_Less and is being maintaining by maddinat0r
		http://forum.sa-mp.com/showthread.php?t=602923
	
*/

//#define SUSERS_SAVING_INI
#define SUSERS_SAVING_SQLITE
#include <susers>

#define DIALOG_REGISTER         0
#define DIALOG_LOGIN            1

enum e_Player
{
	money,
	Float: pX,
	Float: pY,
	Float: pZ
};
new Player[MAX_PLAYERS][e_Player];

main(){}
public OnGameModeInit()
{
	// SQLite only!
	User_CreateField("Money", field_int, 9);
	User_CreateField("pX", field_float); // maxlength is optional if it's a float
	User_CreateField("pY", field_float);
	User_CreateField("pZ", field_float);
	User_CreateTable(); // Has to be after the fields
	// Delete those lines if you switch to INI
	
	AddPlayerClass(0, 0.0, 0.0, 2.0, 0.0, 0, 0, 0, 0, 0, 0);
	return 1;
}

public OnPlayerConnect(playerid)
{
	if (User_IsRegistered(playerid))
	{
	    ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login", "Please enter your password to login", "Login", "Quit");
	}
	else
	{
		ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_INPUT, "Register", "Please enter a password to register", "Register", "Quit");
	}
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	switch (dialogid)
	{
	    case DIALOG_REGISTER:
	    {
	        if (User_Register(playerid, inputtext) == 0)
	            ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_INPUT, "Register", "Please enter a password to register\n{FF0000}Your password must contain 3-32 characters", "Register", "Quit");
	    }
	    case DIALOG_LOGIN:
	    {
			if (User_Login(playerid, inputtext) == 0)
			    ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, "Login", "Please enter your password to login\n{FF0000}Incorrect password, please try again", "Login", "Quit");
	    }
		default: return 0;
	}
	return 1;
}

public OnPlayerRegister(playerid)
{
    SendClientMessage(playerid, 0x00FF00FF, "You have been successfully registered.");
	return 1;
}

public OnPlayerLogin(playerid)
{
	SendClientMessage(playerid, 0x00FF00FF, "You have successfully logged in.");
	return 1;
}

public OnPlayerDataLoad(playerid, entry[], value[])
{
	User_LoadInt("Money", Player[playerid][money]);
	User_LoadFloat("pX", Player[playerid][pX]);
	User_LoadFloat("pY", Player[playerid][pY]);
	User_LoadFloat("pZ", Player[playerid][pZ]);
	return 1;
}

public OnPlayerLogout(playerid)
{
	GetPlayerPos(playerid, Player[playerid][pX], Player[playerid][pY], Player[playerid][pZ]);

	User_SaveInt(playerid, "Money", Player[playerid][money]);
	User_SaveFloat(playerid, "pX", Player[playerid][pX]);
	User_SaveFloat(playerid, "pY", Player[playerid][pY]);
	User_SaveFloat(playerid, "pZ", Player[playerid][pZ]);
	return 1;
}
