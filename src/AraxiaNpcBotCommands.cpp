#include <string>
#include "ScriptMgr.h"
#include "Chat.h"
#include "Player.h"
#include "Unit.h"
#include "ObjectMgr.h"

#include "Creature.h"
#include "botcommon.h"
#include "botdatamgr.h"
#include "botmgr.h"

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
                {"get", HandleGetInventory, 1, Console::No },
                {"get z", HandleGetInventory, 1, Console::No }
            };

            static ChatCommandTable botsCommandTable =
            {
                {"list", HandleListBots, 1, Console::No }
            };

            static ChatCommandTable rootCommandTable =
            {
                {"inv", inventoryCommandTable},
                {"bots", botsCommandTable}
            };

            static ChatCommandTable araxiaNpcBotTable =
            {
                {"anpc", rootCommandTable}
            };

            return araxiaNpcBotTable;
        }

        static bool HandleGetInventory(ChatHandler* handler, Optional<uint32> botEntry)
        {
            Player* player = handler->GetPlayer();

            uint32 entry;

            if (!botEntry)
            {
                Unit* target = player->GetSelectedUnit();

                if (!target) {
                    return true;
                }

                entry = target->GetEntry();
            } else {
                entry = botEntry.value();
            }

            //handler->SendSysMessage("Target Id: " +
            //    std::to_string(entry));

            // load our bot data from the database;
            NpcBotData const* botData = BotDataMgr::SelectNpcBotData(entry);
            if(!botData)
            {
                LOG_DEBUG("AraxiaNpcBot", "No bot data found for entry " + std::to_string(entry));
                return false;
            }


            std::map<uint32, uint32> itemToTemplate;

            ObjectGuid ownerGuid = ObjectGuid(HighGuid::Player, 0, botData->owner);
            std::vector<Item*> items;
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_NPCBOT_EQUIP_BY_ITEM_INSTANCE);
            //        0            1                2      3         4        5      6             7                 8           9           10    11    12         13
            //"SELECT creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, randomPropertyId, durability, playedTime, text, guid, itemEntry, owner_guid "
            //  "FROM item_instance WHERE guid IN (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", CONNECTION_SYNCH

            for (uint8 i = 0; i != BOT_INVENTORY_SIZE; ++i)
                stmt->SetData(i, botData->equips[i]);

            PreparedQueryResult iiresult = CharacterDatabase.Query(stmt);
            if (iiresult)
            {
                do
                {
                    Field* fields2 = iiresult->Fetch();
                    uint32 itemGuidLow = fields2[11].Get<uint32>();
                    uint32 itemId = fields2[12].Get<uint32>();
                    //Item* item = new Item;
                    //ASSERT(item->LoadFromDB(itemGuidLow, ownerGuid, fields2, itemId));
                    itemToTemplate[itemGuidLow] = itemId;
                } while (iiresult->NextRow());
            }

            //handler->SendSysMessage("Bot Owner: " + std::to_string(botData->owner));
            std::map<std::string, uint32> itemList;
            itemList.insert(std::pair<std::string, uint32>("mainhand", itemToTemplate[botData->equips[BOT_SLOT_MAINHAND]]));
            itemList.insert(std::pair<std::string, uint32>("offhand", itemToTemplate[botData->equips[BOT_SLOT_OFFHAND]]));
            itemList.insert(std::pair<std::string, uint32>("ranged", itemToTemplate[botData->equips[BOT_SLOT_RANGED]]));
            itemList.insert(std::pair<std::string, uint32>("head", itemToTemplate[botData->equips[BOT_SLOT_HEAD]]));
            itemList.insert(std::pair<std::string, uint32>("shoulders", itemToTemplate[botData->equips[BOT_SLOT_SHOULDERS]]));
            itemList.insert(std::pair<std::string, uint32>("chest", itemToTemplate[botData->equips[BOT_SLOT_CHEST]]));
            itemList.insert(std::pair<std::string, uint32>("waist", itemToTemplate[botData->equips[BOT_SLOT_WAIST]]));
            itemList.insert(std::pair<std::string, uint32>("legs", itemToTemplate[botData->equips[BOT_SLOT_LEGS]]));
            itemList.insert(std::pair<std::string, uint32>("feet", itemToTemplate[botData->equips[BOT_SLOT_FEET]]));
            itemList.insert(std::pair<std::string, uint32>("wrist", itemToTemplate[botData->equips[BOT_SLOT_WRIST]]));
            itemList.insert(std::pair<std::string, uint32>("hands", itemToTemplate[botData->equips[BOT_SLOT_HANDS]]));
            itemList.insert(std::pair<std::string, uint32>("back", itemToTemplate[botData->equips[BOT_SLOT_BACK]]));
            itemList.insert(std::pair<std::string, uint32>("body", itemToTemplate[botData->equips[BOT_SLOT_BODY]]));
            itemList.insert(std::pair<std::string, uint32>("finger1", itemToTemplate[botData->equips[BOT_SLOT_FINGER1]]));
            itemList.insert(std::pair<std::string, uint32>("finger2", itemToTemplate[botData->equips[BOT_SLOT_FINGER2]]));
            itemList.insert(std::pair<std::string, uint32>("trinket1", itemToTemplate[botData->equips[BOT_SLOT_TRINKET1]]));
            itemList.insert(std::pair<std::string, uint32>("trinket2", itemToTemplate[botData->equips[BOT_SLOT_TRINKET2]]));
            itemList.insert(std::pair<std::string, uint32>("neck", itemToTemplate[botData->equips[BOT_SLOT_NECK]]));

            std::string inv = GetInventoryOutput(entry, itemList);

            handler->SendSysMessage(inv);

            return true;
        }

        static bool HandleListBots(ChatHandler* handler)
        {
            Player* player = handler->GetPlayer();
            BotMgr* playerBotMgr = player->GetBotMgr();
            BotMap* playerBots = playerBotMgr->GetBotMap();

            std::string output = "AraxiaNpcBot:bot.list";
            for (auto const& bot : *playerBots)
            {
                output += "#" + bot.second->GetName()
                    + ":"
                    + std::to_string(bot.second->GetEntry());
            }

            handler->SendSysMessage(output);

            return true;
        }

        static std::string GetInventoryOutput(uint32 botEntry, std::map<std::string, uint32> items)
        {
            std::string output = "AraxiaNpcBot:inv.get:" + std::to_string(botEntry) + "#";

            // loop over all of the keys in items
            for (auto const& item : items)
            {
                // ItemTemplate const* it = sObjectMgr->GetItemTemplate(item.second);
                // if (!it)
                // {
                //     continue;
                // }
                output += item.first + ":" + std::to_string(item.second) + "#";
            }

            // output += "mainhand:" + std::to_string(items["mainhand"]) + "#";
            // output += "offhand:" + std::to_string(items["offhand"]) + "#";
            // output += "ranged:" + std::to_string(items["ranged"]) + "#";
            // output += "head:" + std::to_string(items["head"]) + "#";
            // output += "shoulders:" + std::to_string(items["shoulders"]) + "#";
            // output += "chest:" + std::to_string(items["chest"]) + "#";
            // output += "waist:" + std::to_string(items["waist"]) + "#";
            // output += "legs:" + std::to_string(items["legs"]) + "#";
            // output += "feet:" + std::to_string(items["feet"]) + "#";
            // output += "wrist:" + std::to_string(items["wrist"]) + "#";
            // output += "hands:" + std::to_string(items["hands"]) + "#";
            // output += "back:" + std::to_string(items["back"]) + "#";
            // output += "body:" + std::to_string(items["body"]) + "#";
            // output += "finger1:" + std::to_string(items["finger1"]) + "#";
            // output += "finger2:" + std::to_string(items["finger2"]) + "#";
            // output += "trinket1:" + std::to_string(items["trinket1"]) + "#";
            // output += "trinket2:" + std::to_string(items["trinket2"]) + "#";
            // output += "neck:" + std::to_string(items["neck"]);

            LOG_DEBUG("AraxiaNpcBot", "Output: " + output);

            return output;
        }
};



void AddAraxiaNpcBotCommands()
{
    new AraxiaNpcBotCommands();
};
