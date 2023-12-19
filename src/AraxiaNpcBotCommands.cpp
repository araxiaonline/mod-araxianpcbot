#include <string>
#include "ScriptMgr.h"
#include "Chat.h"
#include "Player.h"
#include "Unit.h"

#include "Creature.h"
#include "botcommon.h"
#include "botdatamgr.h"

#include "AraxiaNpcBot.h"



using namespace Acore::ChatCommands;

class AraxiaNpcBotCommands : public CommandScript
{
    public:
        AraxiaNpcBotCommands() : CommandScript("AraxiaNpcBotCommands") {}

        ChatCommandTable GetCommands() const override
        {
            static ChatCommandTable inventoryCommandTable = 
            {
                {"get", HandleGetInventory, 1, Console::No }
            };

            static ChatCommandTable rootCommandTable = 
            {
                {"inv", inventoryCommandTable}
            };

            static ChatCommandTable araxiaNpcBotTable = 
            {
                {"anpc", rootCommandTable}
            };

            return araxiaNpcBotTable;
        }

        static bool HandleGetInventory(ChatHandler* handler)
        {
            Player* player = handler->GetPlayer();
            Unit* target = player->GetSelectedUnit();

            if (!target) {
                return true;
            }

            uint32 entry = target->GetEntry();

            //handler->SendSysMessage("Target Id: " + 
            //    std::to_string(entry));

            // load our bot data from the database;
            NpcBotData const* botData = BotDataMgr::SelectNpcBotData(entry);
            if(!botData)
            {
                return false;
            }

            EquipmentInfo const* deinfo = BotDataMgr::GetBotEquipmentInfo(entry);
            if (!deinfo)
            {
                return false;
            }

            std::map<std::string, uint32> itemList;

            //handler->SendSysMessage("Bot Owner: " + std::to_string(botData->owner));

            itemList.insert(std::pair<std::string, uint32>("mainhand", botData->equips[BOT_SLOT_MAINHAND]));
            itemList.insert(std::pair<std::string, uint32>("offhand", botData->equips[BOT_SLOT_OFFHAND]));
            itemList.insert(std::pair<std::string, uint32>("ranged", botData->equips[BOT_SLOT_RANGED]));
            itemList.insert(std::pair<std::string, uint32>("head", botData->equips[BOT_SLOT_HEAD]));
            itemList.insert(std::pair<std::string, uint32>("shoulders", botData->equips[BOT_SLOT_SHOULDERS]));
            itemList.insert(std::pair<std::string, uint32>("chest", botData->equips[BOT_SLOT_CHEST]));
            itemList.insert(std::pair<std::string, uint32>("waist", botData->equips[BOT_SLOT_WAIST]));
            itemList.insert(std::pair<std::string, uint32>("legs", botData->equips[BOT_SLOT_LEGS]));
            itemList.insert(std::pair<std::string, uint32>("feet", botData->equips[BOT_SLOT_FEET]));
            itemList.insert(std::pair<std::string, uint32>("wrist", botData->equips[BOT_SLOT_WRIST]));
            itemList.insert(std::pair<std::string, uint32>("hands", botData->equips[BOT_SLOT_HANDS]));
            itemList.insert(std::pair<std::string, uint32>("back", botData->equips[BOT_SLOT_BACK]));
            itemList.insert(std::pair<std::string, uint32>("body", botData->equips[BOT_SLOT_BODY]));
            itemList.insert(std::pair<std::string, uint32>("finger1", botData->equips[BOT_SLOT_FINGER1]));
            itemList.insert(std::pair<std::string, uint32>("finger2", botData->equips[BOT_SLOT_FINGER2]));
            itemList.insert(std::pair<std::string, uint32>("trinket1", botData->equips[BOT_SLOT_TRINKET1]));
            itemList.insert(std::pair<std::string, uint32>("trinket2", botData->equips[BOT_SLOT_TRINKET2]));
            itemList.insert(std::pair<std::string, uint32>("neck", botData->equips[BOT_SLOT_NECK]));

/*
            handler->SendSysMessage("MainHand: " + std::to_string(itemList["mainhand"]));
            handler->SendSysMessage("OffHand: " + std::to_string(itemList["offhand"]));
            handler->SendSysMessage("Ranged: " + std::to_string(itemList["ranged"]));
            handler->SendSysMessage("Head: " + std::to_string(itemList["head"]));
            handler->SendSysMessage("Shoulders: " + std::to_string(itemList["shoulders"]));
            handler->SendSysMessage("Chest: " + std::to_string(itemList["chest"]));
            handler->SendSysMessage("Waist: " + std::to_string(itemList["waist"]));
            handler->SendSysMessage("Legs: " + std::to_string(itemList["legs"]));
            handler->SendSysMessage("Feet: " + std::to_string(itemList["feet"]));
            handler->SendSysMessage("Wrist: " + std::to_string(itemList["wrist"]));
            handler->SendSysMessage("Hands: " + std::to_string(itemList["hands"]));
            handler->SendSysMessage("Back: " + std::to_string(itemList["back"]));
            handler->SendSysMessage("Body: " + std::to_string(itemList["body"]));
            handler->SendSysMessage("Finger1: " + std::to_string(itemList["finger1"]));
            handler->SendSysMessage("Finger2: " + std::to_string(itemList["finger2"]));
            handler->SendSysMessage("Trinket1: " + std::to_string(itemList["trinket1"]));
            handler->SendSysMessage("Trinket2: " + std::to_string(itemList["trinket2"]));
            handler->SendSysMessage("Neck: " + std::to_string(itemList["neck"]));
*/

            std::string inv = GetInventoryOutput(entry, itemList);

            handler->SendSysMessage(inv);

            return true;
        }

        static std::string GetInventoryOutput(uint32 botEntry, std::map<std::string, uint32> items)
        {
            std::string output = "AraxiaNpcBot:inv.get:" + std::to_string(botEntry) + "::";
            output += "mainhand:" + std::to_string(items["mainhand"]) + "::";
            output += "offhand:" + std::to_string(items["offhand"]) + "::";
            output += "ranged:" + std::to_string(items["ranged"]) + "::";
            output += "head:" + std::to_string(items["head"]) + "::";
            output += "shoulders:" + std::to_string(items["shoulders"]) + "::";
            output += "chest:" + std::to_string(items["chest"]) + "::";
            output += "waist:" + std::to_string(items["waist"]) + "::";
            output += "legs:" + std::to_string(items["legs"]) + "::";
            output += "feet:" + std::to_string(items["feet"]) + "::";
            output += "wrist:" + std::to_string(items["wrist"]) + "::";
            output += "hands:" + std::to_string(items["hands"]) + "::";
            output += "back:" + std::to_string(items["back"]) + "::";
            output += "body:" + std::to_string(items["body"]) + "::";
            output += "finger1:" + std::to_string(items["finger1"]) + "::";
            output += "finger2:" + std::to_string(items["finger2"]) + "::";
            output += "trinket1:" + std::to_string(items["trinket1"]) + "::";
            output += "trinket2:" + std::to_string(items["trinket2"]) + "::";
            output += "neck:" + std::to_string(items["neck"]);

            return output;
        }
};



void AddAraxiaNpcBotCommands()
{
    new AraxiaNpcBotCommands();
};
