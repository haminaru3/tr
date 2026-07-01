#pragma once
namespace patterns {
	const char* world = "48 8B 05 ? ? ? ? 48 8B 58 08 48 85 DB 74 32";
	const char* cameraviewangles = "48 8B 05 ? ? ? ? 48 8B 98 ? ? ? ? EB";
	const char* replayinterface = "48 8D 0D ? ? ? ? 48 8B D7 E8 ? ? ? ? 48 8D 0D ? ? ? ? 8A D8 E8 ? ? ? ? 84 DB 75 13 48 8D 0D";
	//const char* worldtoscreen = "48 89 5C 24 ?? 55 56 57 48 83 EC 70 65 4C 8B 0C 25 ?? 00 00 00 8B";
	const char* gameviewport = "48 8B 15 ? ? ? ? 48 8D 2D ? ? ? ? 48 8B CD";
	const char* getentitybonepos = "48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 60 48 8B 01 41 8B E8 48 8B F2 48 8B F9 33 DB";
	const char* bulletinstance = "48 89 5C 24 08 57 48 83 EC 30 41 8B 00 F3";

	const char* raycast_startshapetestcapsule = "41 8B CF C7 85 ? ? ? ? ? ? ? ? E8 ? ? ? ? B2 01";
	const char* raycast_getraycastresult = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 60 33 DB";
}

#ifdef _DEBUG
#define log_smth(log) std::cout << log << std::endl;
#else
#define log_smth
#endif