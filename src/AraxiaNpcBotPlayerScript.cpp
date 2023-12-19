
#include "ScriptMgr.h"

class AraxiaNpcBotPlayerScript : public PlayerScript
{
    public:
        AraxiaNpcBotPlayerScript() : PlayerScript("AraxiaNpcBotPlayerScript") { }
};

void AddAraxiaNpcBotScripts()
{
    new AraxiaNpcBotPlayerScript();
};
