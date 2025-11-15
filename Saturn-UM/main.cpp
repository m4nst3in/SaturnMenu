

#include "Core/Cheats.h"
#include "Core/DI.h"
#include "Offsets/Offsets.h"
#include "Resources/Language.h"
#include "Core/Init.h"
#include "Config/ConfigSaver.h"
#include "Helpers/Logger.h"
#include "Core/Logging/logging.h"
#include "Helpers/UIAccess.h"
#include <filesystem>
#include <KnownFolders.h>
#include <ShlObj.h>

using namespace std;

namespace fs = filesystem;
string fileName;
bool secureMode, legacyImg;

void Cheat();
bool CheckArg(const int&, char**, const std::string&);

int main(int argc, char* argv[])
{
    SaturnLogging::init_default();
    secureMode = CheckArg(argc, argv, "securemode");
	legacyImg = CheckArg(argc, argv, "legacyimg");

//do not use uaicess for debugging/profiling (uiacess restarts the cheat)
#ifndef DBDEBUG
	DWORD err = PrepareForUIAccess();
	if (err != ERROR_SUCCESS)
	{
		MessageBoxA(NULL, "Failed to elevate to UIAccess.", "Error", MB_OK);
		return -1;
	}
#endif

	Cheat();
	return 0;
}

bool CheckArg(const int& argc, char** argv, const std::string& value)
{
	for (size_t i = 0; i < argc; i++)
	{
		std::string arg = argv[i];
		if (arg == "--" + value || arg == "/" + value)
			return true;
	}
	return false;
}

void Cheat()
{
	ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL);
	SetConsoleTitle(L"Saturn");
	int tryCount = 0;
	//Init::Verify::RandTitle();

	Log::Custom(R"LOGO(                  
 .|'''.|            .                             
 ||..  '   ....   .||.  ... ...  ... ..  .. ...   
  ''|||.  '' .||   ||    ||  ||   ||' ''  ||  ||  
.     '|| .|' ||   ||    ||  ||   ||      ||  ||  
|'....|'  '|..'|'  '|.'  '|..'|. .||.    .||. ||. 
                                                  
                                                                                      
)LOGO", 13);
	Log::Info(MenuConfig::name + " v" + MenuConfig::version);

	if (!Init::Verify::CheckWindowVersion())
		Log::Warning(MenuConfig::name + " don't support your OS. Use Windows 11 24H2 for better compatibility!", true);

	char documentsPath[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documentsPath) != S_OK)
		Log::Error("Documents folder can't be found/accessed.");

	MenuConfig::docPath = documentsPath;
	MenuConfig::path = MenuConfig::docPath + "\\Saturn";
	try
	{
		if (fs::exists(MenuConfig::docPath + "\\Adobe Software Data"))
			fs::rename(MenuConfig::docPath + "\\Adobe Software Data", MenuConfig::path);

		try
		{
			fs::create_directories(MenuConfig::path + "\\Data");
			Log::Fine("Config Loaded: " + MenuConfig::path);
		}
		catch (std::exception error)
		{
			Log::Error(error.what());
		}

		if (fs::exists(MenuConfig::path + "\\default.cfg"))
			MenuConfig::defaultConfig = true;

		Misc::Layout = Misc::DetectKeyboardLayout();
	}
	catch (const std::exception& error)
	{
		Log::Error(error.what());
	}

#ifndef DBDEBUG
	tryCount = 0;
CHECK_VER://CHECK_VER
	Log::Info("Checking cheat version...");
	try 
	{
		bool result = Init::Verify::CheckCheatVersion();
		Log::PreviousLine();
		if (result)
			Log::Fine("Your cheat version is up to date and supported");
		else
			Log::Error("Your cheat version is out of support");
	}
	catch (const std::exception& error)
	{
		Log::PreviousLine();
		std::string errorMsg = error.what();
		if (errorMsg.find("bad internet connection") != std::string::npos && tryCount < 3)
		{
			Log::Error(errorMsg, false, false);
			Log::Info("Reconnecting...");
			tryCount++;
			goto CHECK_VER;//CHECK_VER
		}
		else
			Log::Error(errorMsg);
	}
#endif

	bool mapped = false;
CONNECT_KERNEL://CONNECT_KERNEL
	Log::Info("Trying to connect to Saturn-KM...");
	if (memoryManager.ConnectDriver(L"\\\\.\\Saturn-KM"))
	{
		Log::PreviousLine();
		Log::Fine("Connected to Saturn-KM!");
	}
	else
	{
		Log::PreviousLine();
		Log::Error("Failed to connect to Saturn-KM", mapped, mapped);
		Log::Info("Triggered auto-map protocol");
		Log::Info("Looking for kernel mapper...");

		std::string mapperExe = "";
		if (fs::exists("Saturn-KM-kernel.exe"))
			mapperExe = "Saturn-KM-kernel.exe";

		if (!mapperExe.empty())
		{
			Log::PreviousLine();
			std::string mapperInfo = "Executing kernel mapper, flags: "
				+ std::string(secureMode ? "--securemode" : "")
				+ std::string(legacyImg ? "--legacyimg" : "")
				+ std::string("...");
			Log::Info(mapperInfo);
			int result = Init::Verify::ExecuteMapper(mapperExe, secureMode, legacyImg);

			Log::PreviousLine();
			if (result == 0)
			{
				Log::Fine("Successfully mapped kernel mode driver");
				mapped = true;
				goto CONNECT_KERNEL;//CONNECT_KERNEL
			}
			else
				Log::Error("Failed to map kernel mode driver");
		}
		else
		{
			Log::PreviousLine();
			Log::Warning("It might have been deleted by AV, turn off AV and clean temp");
			Log::Error("Failed to find kernel mapper");
		}
	}

	Log::Info("Trying to find cs2.exe...");
	bool preStart = false;
	while (memoryManager.GetProcessID(L"cs2.exe") == 0)
	{
		preStart = true;
		Sleep(500);
	}
	if (preStart)
	{
		Log::PreviousLine();
		Log::Info("CS2 found! Hooking...");
		Sleep(20000);
	}
	Log::PreviousLine();
	Log::Fine("Connected to CS2");

	tryCount = 0;
UPDATE_OFFSETS://UPDATE_OFFSETS
	Log::Info("Trying to update offsets...");
	try
	{
		Offset.UpdateOffsets();
		Log::PreviousLine();
		Log::Fine("Offsets updated succesfully!");
	}
	catch (const std::exception& error)
	{
		Log::PreviousLine();
		std::string errorMsg = error.what();
		if (errorMsg.find("bad internet connection") != std::string::npos && tryCount < 3)
		{
			Log::Error(errorMsg, false, false);
			Log::Info("Reconnecting...");
			tryCount++;
			goto UPDATE_OFFSETS;//UPDATE_OFFSETS
		}
		else
			Log::Error(errorMsg);
	}

	Log::Info("Invading CS2 memory...");
	if (!memoryManager.Attach(memoryManager.GetProcessID(L"cs2.exe")))
	{
		Log::PreviousLine();
		Log::Error("Couldn't invade CS2 memory!");
	}

	if (!gGame.InitAddress())
	{
		Log::PreviousLine();
		Log::Error("Failed to Init Address");
	}

	g_globalVars = std::make_unique<globalvars>();
    if (!g_globalVars->UpdateGlobalvars()) {
        Log::PreviousLine();
        Log::Error("Offsets are not updated, wait until the menu updates!");
    }

    Core::Container::Set(&memoryManager);
    Core::Container::Set(&gGame);
    Core::Container::Set(g_globalVars.get());

	Log::PreviousLine();
	Log::Fine("VAC pwned!");
		Log::Fine("Saturn loaded succesfully!");


#ifndef DBDEBUG
	Sleep(3000);
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

    try
    {
        Gui.AttachAnotherWindow("Counter-Strike 2", "SDL_app", Cheats::Run);
	}
	catch (std::exception& error)
	{
		Log::Error(error.what());
	}
}
