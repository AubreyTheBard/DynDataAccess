/*! \file DynDataAccess.cpp

    \brief DynRPG plugin to give access to data

    \author Bernard J. Badger, AKA Aubrey the Bard

    DynDataAccess is a DynRPG plugin which allows access (get and set) to RPG Maker 2003 data which
    is normally hidden from the developer.
*/

//#define DYNDATAACCESS_DEBUG              // Comment out to remove debugging

#include <DynRPG/DynRPG.h>
#ifdef DYNDATAACCESS_DEBUG
#include <sstream>          // Only needed for debug purposes
#include <iostream>         // Only needed for debug purposes
#endif // DYNDATAACCESS_DEBUG

// Use this as a template for debug popups
// DEBUG
#ifdef DYNDATAACCESS_DEBUG
//std::stringstream message;
//message << "message";
//MessageBox(
//            NULL, // We don't need a window handle
//            message.str().c_str(), // Text
//            "Debug", // Title
//            MB_OK // Flags
//);
#endif // DYNDATAACCESS_DEBUG
// /DEBUG

//! Respond to potential comment commands
/*!
    onComment() is called when the game runs across a comment line in its event scripting. Comment
    commands are how the user communicates with the plugin, so this function checks for comment
    commands and responds accordingly.

    \param text (const char*) The comment's content as simple text
    \param parsedData (const RPG::ParsedCommentData*) The already parsed data
    \param nextScriptLine (RPG::EventScriptLine*) The next event script line after the comment
    \param scriptData (RPG::EventScriptData*) Pointer to the RPG::EventScriptData object of the current event script
    \param eventId (int) The ID of the current event (negative for common events, zero for battle events)
    \param pageId (int) The ID of the current event page (zero for common and battle events - sorry, no battle event page ID yet)
    \param lineId (int) The zero-based line number
    \param nextLineId (int*) Pointer to the next executed line number (-1 for default)
    \return (bool) false will prevent other plugins from receiving this notification, use true otherwise
*/
bool onComment( const char* text,
                const RPG::ParsedCommentData* 	parsedData,
                RPG::EventScriptLine* 	nextScriptLine,
                RPG::EventScriptData* 	scriptData,
                int 	eventId,
                int 	pageId,
                int 	lineId,
                int* 	nextLineId )
{
    static std::string cmd;         // The portion of the comment text representing a comment command, if any
    static int variableIndex;       // The index of the RM2K3 variable to store data in or read data from
    static int dataValue;           // The data value read from DynRPG or to write to DynRPG
    static int partyIndex;          // The party position (NOT ID number) of an actor or enemy
    // ^ADD MORE STATIC VARIABLES AS NEEDED HERE

    // Check for and respond to plugin comment commands
    cmd = parsedData->command;

    // INSTRUCTIONS FOR PLUGIN CONTRIBUTORS
    // To contribute new comment commands to this plugin, copy a section of code below and modify it
    // to access the data you're interested in having commands for. Please try to follow the
    // established style, and use the static variables declared above rather than local variables
    // when feasible. For those who are more well-versed in programming practices, yes, it would be
    // better form to have separate functions for each comment command and merely call them from
    // onComment rather than have the functional code here. However, since the code for each comment
    // command is likely to be fairly short, and some contributors may be confused by having to
    // update multiple areas of code, this simple format is preferred. There are somewhat more
    // detailed instructions in the readme.html file, including how to find the data you're looking
    // for in DynRPG, updating the list of commands, and sharing the results.

    // ENEMY DATA
    // Contributed by Aubrey the Bard
    // This section contains commands for accessing data about enemies, such as their current and
    // maximum HP and MP, Attack stat, etc.
    // NOTE: These comment commands take 1-based values for the enemy's party index in imitation
    // of the way RM2K3 presents information to the developer (enemy 1, enemy 2, etc.), but
    // DynRPG's RPG::monsters array is 0-based (RPG::monsters[0], RPG::monsters[1], etc.). Thus, the
    // party index is subtracted by 1 to translate from 1-based to 0-based.
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_database_id" ) )
    {   // Get the database ID for an enemy
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->databaseId;
        return false;
    }
    // NOTE: While it would be possible to create a set command for the enemy database ID, using
    // such a command would probably lead to undesirable behavior in RM2K3. The Monster class in
    // DynRPG provides a transform function that handles the task of changing an enemy to a
    // different type of enemy. Function calls of that sort are beyond the scope of this project,
    // but the DynBattlerChange plugin (https://rpgmaker.net/engines/rt2k3/utilities/97/) handles
    // transforming enemies.
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_current_hp" ) )
    {   // Get the current HP for an enemy
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->hp;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_enemy_current_hp" ) )
    {   // Set the current HP for an enemy
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the monster to be affected
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::monsters[partyIndex]->hp = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_current_mp" ) )
    {   // Get the current MP for an enemy
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->mp;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_enemy_current_mp" ) )
    {   // Set the current MP for an enemy
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to be affected
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::monsters[partyIndex]->mp = dataValue;
        return false;
    }
    // NOTE: The remaining enemy attributes cannot be changed -- at least, not for individual
    // enemies; they are held in the DBMonster objects, which store data about each type of
    // enemy. Thus, the remainder of individual enemy attributes only have get comment commands,
    // not set comment commands.
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_max_hp" ) )
    {   // Get the maximum HP for an enemy
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getMaxHp();
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_max_mp" ) )
    {   // Get the maximum MP for an enemy
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getMaxMp();
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_attack" ) )
    {   // Get the Attack for an enemy
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getAttack();
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_defense" ) )
    {   // Get the Defense for an enemy
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getDefense();
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_intelligence" ) )
    {   // Get the Intelligence for a enemy
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getIntelligence();
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_agility" ) )
    {   // Get the Agility for an enemy
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getAgility();
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_all_stats" ) )
    {   // Get all the stats for an enemy
        // Parameter 0: The index of the first RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variables
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->databaseId;
        variableIndex++;
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->hp;
        variableIndex++;
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->mp;
        variableIndex++;
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getMaxHp();
        variableIndex++;
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getMaxMp();
        variableIndex++;
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getAttack();
        variableIndex++;
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getDefense();
        variableIndex++;
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getIntelligence();
        variableIndex++;
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->getAgility();
        return false;
    }
    // END OF ENEMY DATA SECTION

    // END OF COMMENT COMMANDS

    // No comment commands for this plugin detected; pass notification on for other plugins
    return true;
}
