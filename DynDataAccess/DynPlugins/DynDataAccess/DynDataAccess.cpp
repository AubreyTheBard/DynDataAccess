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
    static int partyIndex;          // The party position (NOT ID number) of an actor (1-4) or enemy (1-8)
    static int skillIndex;          // The skill database ID
    static int attributeIndex;      // The attribute database ID
    static int itemIndex;           // The item database ID
    static int conditionIndex;      // The condition database ID
    static int terrainIndex;        // The terrain database ID
    std::string textString;         // For the manipulation of strings and accepting of text input
    const int MAX_ACTORS = 4;                           //!< Maximum number of actors in a battle
    const int MAX_MONSTERS = 8;                         //!< Maximum number of monsters in a battle
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

    // ACTOR DATA SECTION
    // This section contains commands for accessing data about actors, such as their current and
    // maximum HP and MP, Attack stat, etc.
    // NOTE: These comment commands take 1-based values for the actor's party index in imitation
    // of the way RM2K3 presents information to the developer (party member 1, party member 2,
    // etc.), but DynRPG's RPG::Actor::partyMember function is 0-based (RPG::Actor::partyMember(0),
    // RPG::Actor::partyMember(1), etc.). Thus, the party index is subtracted by 1 to translate from
    // 1-based to 0-based.

    // Contributed by DJC

    if( 0 == cmd.compare( "dyndataaccess_get_party_member_id" ) )
    {   // Sets desired game variable to database ID of actor in requested party member slot (1-4)
        // If requested slot empty, variable is set to 0
        // Parameter 0: The index of the RM2K3 variable to store data
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the actor
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable after checking for empty slot
        if(RPG::Actor::partyMember(partyIndex) != NULL)
            RPG::variables[variableIndex] = RPG::Actor::partyMember(partyIndex)->id;
        else
            RPG::variables[variableIndex] = 0;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_party_member_all_ids" ) )
    {   // Sets four sequential game variables to database IDs of the actors in the party
        // If any slots are empty, the corresponding variable is set to 0
        // Parameter 0: The index of the first of four sequential RM2K3 variables to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Cycle through all party slots, storing data in the appropriate RM2K3 variable after checking if empty
        for(int i=0; i<MAX_ACTORS; i++) {
            if(RPG::Actor::partyMember(i) != NULL)
                RPG::variables[variableIndex+i] = RPG::Actor::partyMember(i)->id;
            else
                RPG::variables[variableIndex+i] = 0; }
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_party_member_critical_rate" ) )
    {   // Set party member critical hit rate percentage (1 out of dataValue chance; Example: 1 out of 2 = 50%)
        // Volatile change, must be reapplied after every load
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->criticalHitProbability = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_party_member_guard_type" ) )
    {   // Set party member guard to regular (0) or mighty (1)
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member to be affected
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        if(dataValue == 0 || 1)
            RPG::Actor::partyMember(partyIndex)->mightyGuard = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_party_member_database_attribute_resistance" ) )
    {   // Get database default party member attribute resistance percentage
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The attribute database id
        attributeIndex = (int) parsedData->parameters[2].number;
        // Store the data in the appropriate RM2K3 variable
        if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->attributes[attributeIndex] == 0)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgA;
        else if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->attributes[attributeIndex] == 1)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgB;
        else if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->attributes[attributeIndex] == 2)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgC;
        else if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->attributes[attributeIndex] == 3)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgD;
        else if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->attributes[attributeIndex] == 4)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgE;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_party_member_database_attribute_resistance" ) )
    {   // Set party member database default attribute resistance
        // Volatile change, must be reapplied on load
        // RM2K3 allows only one increase/decrease from this level of resistance
        // If set to E resistance, cannot increase back to this level with skills
        // Parameter 0: The value to set data to (0-4 correspond to attribute database values of A-E)
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The attribute database id
        attributeIndex = (int) parsedData->parameters[2].number;
        // Alter the data to the desired value if dataValue is within acceptable range
        if(dataValue >= 0 && dataValue <= 4)
            RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->attributes[attributeIndex] = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_party_member_current_attribute_resistance" ) )
    {   // Get current attribute resistance of a party member
        // Base resistance is default adjusted by any equipment boosts
        // 0=one resist level down from base; 1=base; 2=one level up from base; 3=two levels up; etc...
        // Values beyond A-E scope indicate miss/immune
        // Negative values indicate miss/immune
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The attribute database id
        attributeIndex = (int) parsedData->parameters[2].number;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::Actor::partyMember(partyIndex)->attributes[attributeIndex];
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_party_member_current_attribute_resistance" ) )
    {   // Set party member current attribute resistance
        // Base resistance is default adjusted by any equipment boosts
        // 0=one resist level down from base; 1=base; 2=one level up from base; 3=two levels up; etc...
        // Change beyond A-E scope results in miss/immune
        // Negative values result in miss/immune and no response to skill based attribute changes
        // Skill based attribute changes cannot exceed A-E scope, even if initially set beyond
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The attribute database id
        attributeIndex = (int) parsedData->parameters[2].number;
        // Alter the data to the desired value
        RPG::Actor::partyMember(partyIndex)->attributes[attributeIndex] = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_party_member_condition_turns" ) )
    {   // Get the number of turns a party member has been afflicted with the requested condition
        // 0=not currently afflicted with requested condition
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The condition database id
        conditionIndex = (int) parsedData->parameters[2].number;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::Actor::partyMember(partyIndex)->conditions[conditionIndex];
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_party_member_condition_turns_total" ) )
    {   // Get the number of turns a party member has been afflicted with all conditions
        // Ignores any conditions with a priority lower than requested
        // Returned value of 0 = not currently afflicted with any condition
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: Priority level to ignore
        int priority = (int) parsedData->parameters[2].number;
        // Initialize data variable
        RPG::variables[variableIndex] = 0;
        // Loop through all conditions, increment data variable by each condition's turn amount
        for(int i=1; i<RPG::conditions.count()+1; i++) { // Condition array is one based
            if(RPG::conditions[i]->priority >= priority)
                    RPG::variables[variableIndex] += RPG::Actor::partyMember(partyIndex)->conditions[i]; }
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_party_member_condition_total" ) )
    {   // Get the number of conditions a party member is currently suffering
        // Ignores any conditions with a priority lower than requested
        // Returned value of 0 = not currently afflicted with any condition
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: Priority level to ignore
        int priority = (int) parsedData->parameters[2].number;
        // Initialize data variable
        RPG::variables[variableIndex] = 0;
        // Loop through all conditions, increment data variable by each condition's turn amount
        for(int i=1; i<RPG::conditions.count()+1; i++) { // Condition array is one based
                if(RPG::conditions[i]->priority >= priority && RPG::Actor::partyMember(partyIndex)->conditions[i] > 0)
                    RPG::variables[variableIndex]++; }
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_party_member_database_condition_resistance" ) )
    {   // Get database default party member condition resistance percentage
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The condition database id
        conditionIndex = (int) parsedData->parameters[2].number;
        // Store the data in the appropriate RM2K3 variable
        if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->conditions[conditionIndex] == 0)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susA;
        else if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->conditions[conditionIndex] == 1)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susB;
        else if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->conditions[conditionIndex] == 2)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susC;
        else if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->conditions[conditionIndex] == 3)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susD;
        else if(RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->conditions[conditionIndex] == 4)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susE;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_party_member_database_condition_resistance" ) )
    {   // Set party member database default condition resistance
        // Volatile change, must be reapplied on load
        // Equipment condition resistances still override like normal
        // Parameter 0: The value to set data to (0-4 correspond to condition database values of A-E)
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The condition database id
        conditionIndex = (int) parsedData->parameters[2].number;
        // Alter the data to the desired value if dataValue is legit number
        if(dataValue >= 0 && dataValue <= 4)
            RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->conditions[conditionIndex] = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_party_member_combo" ) )
    {   // Set party member combo command and repetitions
        // Only one command can be setup for a combo per party member
        // Some commands cannot be set to combo (Item, Defend, etc...)
        // Possible to exceed Rm2K3 combo limit of 8, but excessive combos may cause issues with turn overlap
        // Parameter 0: The battle command ID from the database to become a combo
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The number of repetitions for the combo
        int numHits = (int) parsedData->parameters[2].number;
        // Alter the data to the desired value if dataValue
        RPG::Actor::partyMember(partyIndex)->comboBattleCommand = dataValue;
        RPG::Actor::partyMember(partyIndex)->comboRepetitions = numHits;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_party_member_animation2" ) )
    {   // Get party member Animations2 ID
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::variables[variableIndex] = RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->battleGraphicId;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_party_member_animation2" ) )
    {   // Set party member Animation2 ID
        // Only works outside of battle
        // Parameter 0: The Animation2 database ID
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the party member
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::dbActors[RPG::Actor::partyMember(partyIndex)->id]->battleGraphicId = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_party_member_defeated_count" ) )
    {   // Gets the number of fallen party members in the party
        // Parameter 0: The RM2K3 variable to store data
        variableIndex = (int) parsedData->parameters[0].number;
        RPG::variables[variableIndex] = 0;
        for(int i=0; i<MAX_ACTORS; i++) {
            if(RPG::Actor::partyMember(i) != NULL && RPG::Actor::partyMember(i)->conditions[1] > 0)
                RPG::variables[variableIndex]++; }
        return false;
    }
    // END OF ACTOR DATA SECTION

    // BATTLE DATA SECTION

    // Contributed by DJC

    if( 0 == cmd.compare( "dyndataaccess_set_battle_bg" ) )
    {   // Set battle background
        // Parameter 0: The filename of the battle background, directory included relative to main game folder
        textString = parsedData->parameters[0].text;
        // Alter the data to the desired value
        RPG::battleData->backdropImage->loadFromFile(textString);
        return false;
    }
    //!do one for changing frames
    // END OF BATTLE DATA SECTION

    // BATTLE DATABASE TROOP DATA SECTION

    // Contributed by DJC

    if( 0 == cmd.compare( "dyndataaccess_get_troop_initial_size" ) )
    {   // Get the initial enemy troop size as defined in the database
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::dbMonsterGroups[RPG::battleData->monsterGroupId]->monsterList.count();
        return false;
    }
    // END OF DATABASE TROOP DATA SECTION

    // ITEM DATA SECTION

    // Contributed by DJC

    if( 0 == cmd.compare( "dyndataaccess_get_item_attribute" ) )
    {   // Get whether an item has requested attribute tagged
        // Parameter 0: The index of the RM2K3 variable to store data in (0=false, 1=true)
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: Database ID of the item
        itemIndex = (int) parsedData->parameters[1].number;
        // Parameter 2: Database ID of the attribute
        attributeIndex = (int) parsedData->parameters[2].number - 1;
        // Store the value in the designated variable
        RPG::variables[variableIndex] = (int) RPG::items[itemIndex]->attributes[attributeIndex];
        return false;
    }
    // END OF ITEM DATA SECTION

    // ENEMY DATA
    // This section contains commands for accessing data about enemies, such as their current and
    // maximum HP and MP, Attack stat, etc.
    // NOTE: These comment commands take 1-based values for the enemy's party index in imitation
    // of the way RM2K3 presents information to the developer (enemy 1, enemy 2, etc.), but
    // DynRPG's RPG::monsters array is 0-based (RPG::monsters[0], RPG::monsters[1], etc.). Thus, the
    // party index is subtracted by 1 to translate from 1-based to 0-based.

    // Contributed by Aubrey the Bard

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

    //Contributed by DJC

    if( 0 == cmd.compare( "dyndataaccess_get_enemy_database_stats" ) )
    {   // Gets the unaltered database attack, defense, intelligence, and agility of requested monster
        // Parameter 0: The index of the first of four sequential RM2K3 variables to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variables
        RPG::variables[variableIndex] = RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->attack;
        variableIndex++;
        RPG::variables[variableIndex] = RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->defense;
        variableIndex++;
        RPG::variables[variableIndex] = RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->intelligence;
        variableIndex++;
        RPG::variables[variableIndex] = RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->agility;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_enemy_attack" ) )
    {   // Change the enemy's current attack relative to the database default
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::monsters[partyIndex]->attackDiff = dataValue - RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->attack;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_enemy_defense" ) )
    {   // Change the enemy's current attack relative to the database default
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::monsters[partyIndex]->defenseDiff = dataValue - RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->defense;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_enemy_intelligence" ) )
    {   // Change the enemy's current attack relative to the database default
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::monsters[partyIndex]->intelligenceDiff = dataValue - RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->intelligence;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_enemy_agility" ) )
    {   // Change the enemy's current attack relative to the database default
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::monsters[partyIndex]->agilityDiff = dataValue - RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->agility;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_attribute_resistance" ) )
    {   // Get database default enemy attribute resistance percentage
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The attribute database id
        attributeIndex = (int) parsedData->parameters[2].number;
        // Store the data in the appropriate RM2K3 variable
        if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->attributes[attributeIndex] == 0)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgA;
        else if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->attributes[attributeIndex] == 1)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgB;
        else if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->attributes[attributeIndex] == 2)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgC;
        else if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->attributes[attributeIndex] == 3)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgD;
        else if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->attributes[attributeIndex] == 4)
            RPG::variables[variableIndex] = RPG::attributes[attributeIndex]->dmgE;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_condition_resistance" ) )
    {   // Get database default enemy condition resistance percentage
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The condition database id
        conditionIndex = (int) parsedData->parameters[2].number;
        // Store the data in the appropriate RM2K3 variable
        if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->conditions[conditionIndex] == 0)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susA;
        else if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->conditions[conditionIndex] == 1)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susB;
        else if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->conditions[conditionIndex] == 2)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susC;
        else if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->conditions[conditionIndex] == 3)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susD;
        else if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->conditions[conditionIndex] == 4)
            RPG::variables[variableIndex] = RPG::conditions[conditionIndex]->susE;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_force_enemy_condition" ) )
    {   // Forces an enemy to suffer a condition
        // Failure if resistance less than or equal to specified level
        // Does not function correctly for condition 0 (KO); inflicts condition, but enemy does not KO; requires additional scripting
        // Parameter 0: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[0].number - 1;
        // Parameter 1: The condition database id
        conditionIndex = (int) parsedData->parameters[1].number;
        // Parameter 2: The failure threshold (0-4 for A-E, 5 for certain hit)
        int failureLevel = (int) parsedData->parameters[2].number;
        // Force the condition if enemy database resistance above failure threshold
        if(RPG::dbMonsters[RPG::monsters[partyIndex]->databaseId]->conditions[conditionIndex] < failureLevel) {
            RPG::monsters[partyIndex]->conditions[conditionIndex] = 1; // Inflict condition
            if(conditionIndex == 1) RPG::monsters[partyIndex]->hp = 0; } // Reduce HP to zero if Fallen condition
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_atb" ) )
    {   // Get the current ATB for an enemy (30000 full)
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->atbValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_enemy_atb" ) )
    {   // Set the current ATB for an enemy (300000 full, needs extra power of ten)
        // Parameter 0: The value to set data to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to be affected
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Alter the data to the desired value
        RPG::monsters[partyIndex]->atbValue = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_enemy_text_popup" ) )
    {   // Display pop-up text for an enemy
        // Parameter 0: Text to display, limited characters
        textString = parsedData->parameters[0].text;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Show the text
        RPG::monsters[partyIndex]->damagePopup(textString);
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_enemy_number_popup" ) )
    {   // Display pop-up number for an enemy
        // Parameter 0: Number to display
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy to get data from
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: Number color (0-19)
        int dataColor = (int) parsedData->parameters[2].number;
        // Show the number
        RPG::monsters[partyIndex]->damagePopup(dataValue, dataColor);
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_enemy_flash" ) )
    {   // Flash target enemy a specific RGB intensity for a number of frames
        // Parameter 0: The party index of the enemy
        partyIndex = (int) parsedData->parameters[0].number - 1;
        // Parameter 1: Red value
        int redLevel = (int) parsedData->parameters[1].number;
        // Parameter 2: Green value
        int greenLevel = (int) parsedData->parameters[2].number;
        // Parameter 3: Blue value
        int blueLevel = (int) parsedData->parameters[3].number;
        // Parameter 4: Intensity value
        int intensityLevel = (int) parsedData->parameters[4].number;
        // Parameter 5: Number of frames
        int flashFrames = (int) parsedData->parameters[5].number;
        // Flash the monster
        RPG::monsters[partyIndex]->flash(redLevel, greenLevel, blueLevel, intensityLevel, flashFrames);
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_enemy_sprite" ) )
    {   // Set the enemy graphic
        // Parameter 0: Filename of enemy graphic, including directory relative to game folder
        textString = parsedData->parameters[0].text;
        // Parameter 1: The party index of the enemy
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Change the battler image
        RPG::monsters[partyIndex]->image->loadFromFile(textString);
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_defeated_count" ) )
    {   // Gets the number of defeated foes in the current battle.
        // Parameter 0: The index of the RM2K3 variable to store data
        variableIndex = (int) parsedData->parameters[0].number;
        RPG::variables[variableIndex] = 0;
        for(int i=0; i<MAX_MONSTERS; i++) {
            if(RPG::monsters[i] != NULL && RPG::monsters[i]->hp < 1)
                RPG::variables[variableIndex]++; }
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_undefeated_count" ) )
    {   // Gets the number of undefeated foes in the current battle.
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Initialize game variable
        RPG::variables[variableIndex] = 0;
        // Cycle through possible monsters, increment troop size if monster exists and defeated
        for( int i=0; i<MAX_MONSTERS; i++) {
            if(RPG::monsters[i] != NULL && RPG::monsters[i]->hp > 0)
                RPG::variables[variableIndex]++; }
        return false;
    }

    //Contributed by xshobux
    if( 0 == cmd.compare( "dyndataaccess_get_enemy_condition_turns" ) )
    {   // Get the number of turns an enemy has been afflicted with the requested condition
        // 0=not currently afflicted with requested condition
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: The party index of the enemy
        partyIndex = (int) parsedData->parameters[1].number - 1;
        // Parameter 2: The condition database id
        conditionIndex = (int) parsedData->parameters[2].number;
        // Store the data in the appropriate RM2K3 variable
        RPG::variables[variableIndex] = RPG::monsters[partyIndex]->conditions[conditionIndex];
        return false;
    }
    // END OF ENEMY DATA SECTION

    // MAP DATA SECTION

    // Contributed by DJC

    if( 0 == cmd.compare( "dyndataaccess_get_encounter_rate_current" ) )
    {   // Get current map encounter rate
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Store the data in the specified variable
        RPG::variables[variableIndex] = RPG::map->encounterRateNew;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_encounter_rate_current" ) )
    {   // Set current map encounter rate
        // Parameter 0: The data value to change the map encounter rate to
        dataValue = (int) parsedData->parameters[0].number;
        // Alter the data to the desired value
        RPG::map->encounterRateNew = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_get_database_encounter_rate" ) )
    {   // Get database default map encounter rate
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Store the data in the specified variable
        RPG::variables[variableIndex] = RPG::mapTree->properties[RPG::mapTree->getTreeIndex(RPG::map->properties->id)]->encounterRate;
        return false;
    }
    // END OF MAP DATA SECTION

    // SKILL DATA SECTION

    // Contributed by DJC

    if( 0 == cmd.compare( "dyndataaccess_get_skill_cost" ) )
    {   // Get the cost of a skill
        // Parameter 0: The index of the RM2K3 variable to store data in
        variableIndex = (int) parsedData->parameters[0].number;
        // Parameter 1: Database ID of the skill
        skillIndex = (int) parsedData->parameters[1].number;
        // Store the value in the designated variable
        RPG::variables[variableIndex] = RPG::skills[skillIndex]->mpCost;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_skill_cost" ) )
    {   // Set skill cost
        // This will overwrite database values, but volatile, resets on reload
        // Parameter 0: The data value to change skill cost to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: Database ID of the skill
        skillIndex = (int) parsedData->parameters[1].number;
        // Alter the data to the desired value
        RPG::skills[skillIndex]->mpCost = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_skill_attack_influence" ) )
    {   // Sets a skill's attack influence
        // This will overwrite database values, but volatile, resets on reload
        // Parameter 0: The data value to change skill cost to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: Database ID of the skill
        skillIndex = (int) parsedData->parameters[1].number;
        // Alter the data to the desired value
        RPG::skills[skillIndex]->atkInfluence = dataValue;
        return false;
    }
    if( 0 == cmd.compare( "dyndataaccess_set_skill_effect_rating" ) )
    {   // Sets a skill's effect rating (damage or healing)
        // This will overwrite database values, but volatile, resets on reload
        // Parameter 0: The data value to change skill cost to
        dataValue = (int) parsedData->parameters[0].number;
        // Parameter 1: Database ID of the skill
        skillIndex = (int) parsedData->parameters[1].number;
        // Alter the data to the desired value
        RPG::skills[skillIndex]->effectRating = dataValue;
        return false;
    }
    // END OF SKILL DATA SECTION

    // TERRAIN DATA

    // Contributed by DJC

    if( 0 == cmd.compare( "dyndataaccess_set_terrain_initiative_rate" ) )
    {   // Set terrain's initiative encounter rate (as a percentage)
        // Parameter 0: The data value to change the rate to
        dataValue = (int) parsedData->parameters[0].number;
         // Parameter 1: The database ID of the terrain
        terrainIndex = (int) parsedData->parameters[1].number;
        // Alter the data to the desired value
        RPG::terrains[terrainIndex]->initiativePercent = dataValue;
        return false;
    }
    //!do the other terrain type battles, and one that cycles through all terrains
    //!maybe some for passability so you don't need to change tilesets workaround
    // END OF TERRAIN DATA SECTION

    // ATTRIBUTE DATA SECTION

    // END OF COMMENT COMMANDS

    // No comment commands for this plugin detected; pass notification on for other plugins
    return true;
}
