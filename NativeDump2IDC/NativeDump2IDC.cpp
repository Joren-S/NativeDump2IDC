
// Standard C++
#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <string.h>
#include <vector>
#include <stdint.h>

// Extra
#include "json.hpp"
#include "HashMap.h"

#pragma region Declarations

typedef uint64_t Hash;

typedef char* Address;
typedef char* HashString;
typedef char* Namespace;

using namespace std;
using json = nlohmann::json;

#pragma endregion

#pragma region Variables + Structures

struct Native {
	char native_hash[30];
	string native_name;
	char native_address[30];
};

json NativesJSON;
char stringbuffer[100];
char hashbuffer[100];
vector<Native*> Natives;

Namespace Namespaces[] = {
	"PLAYER", "ENTITY", "PED", "VEHICLE", "OBJECT", "AI", "GAMEPLAY", "AUDIO", "CUTSCENE", "INTERIOR", "CAM", "WEAPON",
	"ITEMSET", "STREAMING", "SCRIPT", "UI", "GRAPHICS", "STATS", "APP", "TIME", "PATHFIND", "CONTROLS", "DATAFILE", "FIRE",
	"DECISIONEVENT", "ZONE", "ROPE", "WATER", "WORLDPROBE", "NETWORK", "NETWORKCASH", "DLC1", "DLC2", "SYSTEM", "DECORATOR",
	"SOCIALCLUB", "UNK", "UNK1", "UNK2", "UNK3"
};

#pragma endregion

#pragma region Natives JSON

bool LoadNativeNames() {
	printf("\nOpening natives.json ...");
	ifstream file("natives.json");
	if (!file.fail()) {
		printf("\n\tInterpreting natives.json ...");
		try {
			file >> NativesJSON;
			printf("\nFinished loading natives.json!\n");
			return true;
		}
		catch (...) { printf("\nError while interpreting JSON from \"natives.json\"\n\tMake sure the format is correct!\n\tDownload the latest version here: http://www.dev-c.com/nativedb/natives.json \n"); }
	}
	else printf("\nError while opening file:\n\tFile \"natives.json\" doesn't exist!\n\tDownload the latest version here: http://www.dev-c.com/nativedb/natives.json \n");
	return false;
}

char *GetNativeName(HashString in_hash, bool *found) {
	*found = false;
	int NamespaceCount = sizeof(Namespaces) / sizeof(Namespace);
	for (int i = 0; i < NamespaceCount; i++) {
		auto result = NativesJSON.find(Namespaces[i]);
		if (result != NativesJSON.end()) {
			auto res = result.value().find(in_hash);
			if (res != result.value().end()) {
				*found = true;
				memset(stringbuffer, 0, 100);
				strcpy(stringbuffer, res.value()["name"].get<string>().c_str());
				return stringbuffer;
			}
		}
	}
	return "\0";
}

#pragma endregion

#pragma region Natives + Addresses Dump

void AddNativeToVector(string line)
{
	char *split;
	Native *cur_native = new Native;
	split = strtok((char*)line.c_str(), " : ");
	strcpy(cur_native->native_hash, split);
	split = strtok(NULL, " : ");
	strcpy(cur_native->native_address, split);
	Natives.push_back(cur_native);
}

bool LoadHashesAndAddresses()
{
	string line;
	printf("\nOpening natives_dump.txt ...");
	ifstream file("natives_dump.txt");
	if (!file.fail()) {
		printf("\n\tLoading natives ...");
		while (getline(file, line)) {
			AddNativeToVector(line);
		}
		printf("\n\tCounting natives ...");
		printf("\n\tFound %i natives!", Natives.size());
		printf("\nFinished loading natives_dump.txt!\n");
		return true;
	}
	else printf("\nError while opening file:\n\tFile \"natives_dump.txt\" doesn't exist!\n");
	return false;
}

#pragma endregion

#pragma region Hash Rotation

char *FindOriginalHash(char *HashAsString)
{
	uint64_t HashAsUINT64 = strtoull(HashAsString + 2, NULL, 16);
	int TranslationCount = (sizeof(HASH_MAP) / sizeof(Hash)) / 2;
	for (int i = 0; i < TranslationCount; i++)
	{
		int cur_index = 1 + (i * 2);
		if (HASH_MAP[cur_index] == HashAsUINT64)
		{
			memset(hashbuffer, 0, 100);
			snprintf(hashbuffer, 100, "0x%llX", HASH_MAP[cur_index - 1]);
			return hashbuffer;
		}
	}
	return "\0";
}

#pragma endregion

int main()
{
	if (LoadNativeNames() && LoadHashesAndAddresses())
	{
		printf("\n\nBuilding IDC Script...");
		bool found;
		char *native_name;
		char *ori_hash;
		Native *current_native = new Native;
		FILE * output = fopen("NativeNamer.idc", "w");
		fprintf(output, "#include <idc.idc>\nstatic main()\n{\n");
		for (int native_index = 0; native_index < Natives.size(); native_index++)
		{
			found = false;
			current_native = Natives.at(native_index);
			ori_hash = FindOriginalHash(current_native->native_hash);
			native_name = GetNativeName(ori_hash, &found);
			if (found)
			{
				bool unk_native = (native_name[0] == '\0');
				fprintf(output, "\tMakeName(%s, \"%s%s\");\n", current_native->native_address, unk_native ? "UNK_" : "", unk_native ? ori_hash : native_name);
			}
			else
			{
				fprintf(output, "\tMakeName(%s, \"UNK_%s\");\n", current_native->native_address, FindOriginalHash(current_native->native_hash));
			}
		}

		fprintf(output, "}");
		fflush(output);
		fclose(output);

		printf("\n\nFINISHED\n");
	}
	return 0;
}