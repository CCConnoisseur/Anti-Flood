#include <a_samp>

/*
    - External
*/
#define _MAX_ALLOWED_CONNECTIONS_       1   // Define maximum allowed connections for easier use
#include <YSI_Server\y_flooding>            // Include anti-flooding by Y_Less (https://github.com/pawn-lang/YSI-Includes/blob/5.x/YSI_Server/y_flooding.md)

/*
    - Implementation
*/

/*
    This function gets called when the script is initialized
*/
#if !defined _INC_y_hooks
    public OnGameModeInit()
#else
    hook OnGameModeInit()
#endif
{
    /*
        - Actions
        e_FLOOD_ACTION_NOTHING                                              - Do nothing, basically disable the system
        e_FLOOD_ACTION_BLOCK                                                - Kick the latest player on this IP (default)
        e_FLOOD_ACTION_KICK                                                 - Kick all players on this IP
        e_FLOOD_ACTION_BAN                                                  - Ban the IP and have players time out
        e_FLOOD_ACTION_FBAN                                                 - Ban the IP and kick all the players instantly
        e_FLOOD_ACTION_GHOST                                                - Silently force all players on the IP to reconnect
        e_FLOOD_ACTION_OTHER                                                - Call a callback (OnFloodLimitExceeded(ip[], count))
    */
    /*
        - APIs
        SetMaxConnections(max, e_FLOOD_ACTION:action)
        -> Note:
                Explained above, and below
        Flooding_UnbanIP(ip32)
        -> Note:
                Timers, by default, are not good with strings, so we can't pass the IP to unban in dot notation. Fortunately, this is easilly solved
        OnPlayerConnect(playerid)
        -> Note:
                Checks for too many connections from the same IP address and acts accordingly
                Could be edited to only loop through players once but I'm not sure the extra code required would be faster anyway, definately not easier
    */

    //SetMaxConnections();                                                  // Default settings will be applied, allowing unlimited connections with the same IP address to connect
    //SetMaxConnections(_MAX_ALLOWED_CONNECTIONS_);                         // Allow only _MAX_ALLOWED_CONNECTIONS_ IP address, and kick last connected player(by default) if there's 2+
    //SetMaxConnections(_MAX_ALLOWED_CONNECTIONS_, e_FLOOD_ACTION_KICK);    // Allow only _MAX_ALLOWED_CONNECTIONS_ IP address, and kick all players instantly if there's 2+
    //SetMaxConnections(_MAX_ALLOWED_CONNECTIONS_, e_FLOOD_ACTION_BAN);     // Allow only _MAX_ALLOWED_CONNECTIONS_ IP address, ban all players, but forcefully timeout them instantly if there's 2+
    //SetMaxConnections(_MAX_ALLOWED_CONNECTIONS_, e_FLOOD_ACTION_FBAN);    // Allow only _MAX_ALLOWED_CONNECTIONS_ IP address, ban all players instantly if there's 2+
    //SetMaxConnections(_MAX_ALLOWED_CONNECTIONS_, e_FLOOD_ACTION_GHOST);   // Allow only _MAX_ALLOWED_CONNECTIONS_ IP address, forcefully re-connect them instantly if there's 2+
    SetMaxConnections(_MAX_ALLOWED_CONNECTIONS_, e_FLOOD_ACTION_OTHER);     // Allow only _MAX_ALLOWED_CONNECTIONS_ IP address, call a callblack OnFloodLimitExceeded and customize what will happen yourself

    /*
        Continue OnScriptInit chain(if there's any hooked OnScriptInit, making sure they get called as well)
    */
    return 1;
}
#if !defined _INC_y_hooks
		#if defined _OnGameModeInit
			#undef OnGameModeInit
		#else
			#define _ALS_OnGameModeInit
		#endif
		#define OnGameModeInit AntiFlood_OnGameModeInit
		#if defined AntiFlood_OnGameModeInit
			forward AntiFlood_OnGameModeInit(playerid);
		#endif
	#endif

/*
    This function is called if action e_FLOOD_ACTION_OTHER is passed in API SetMaxConnections
*/
#if !defined OnFloodLimitExceeded
    public OnFloodLimitExceeded(ip[16], count)
    {
        /*
            If the limit was exceeded
        */
        if(count == _MAX_ALLOWED_CONNECTIONS_+1)
        {
            /*
                Loop through the IP where the limit was exceeded
            */
            new
                lStr[128];
            
            format(lStr, sizeof(lStr), "Warning:{FFFFFF} There is %i of you with the same IP connected, if one more connects you will be banned!", count);
            
            foreach(new i: FloodingPlayer)
            {
                /*
                    Send them a warning that if one more connects, they will be banned
                */
                SendClientMessage(i, 0xFF0000AA, lStr);
            }
        }
        /*
            Else! If there's more than _MAX_ALLOWED_CONNECTIONS_+1
        */
        else
        {
            /*
                Loop through the IP where the limit was exceeded
            */
            foreach (new i : FloodingPlayer)
            {
                /*
                    Ban them as they were already warned
                */
                BanEx(i, "Exceeded the anti-flooding limit.");
            }
        }
    }
#endif