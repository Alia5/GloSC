/*
Copyright 2018 Peter Repukat - FlatspotSoftware

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
 
GloSC_Watchdog, unhooks Steam if SteamTarget gets killed.

*/

#include <Windows.h>
#include <tlhelp32.h>

#include "../common/Injector.h"

//stolen from: https://stackoverflow.com/questions/1591342/c-how-to-determine-if-a-windows-process-is-running
bool IsProcessRunning(const wchar_t *processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_wcsicmp(entry.szExeFile, processName))
			{
				exists = true;
				break;
			}

	CloseHandle(snapshot);
	return exists;
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	while (IsProcessRunning(L"SteamTarget.exe"))
	{
		Sleep(1000);
	} 
	
	Injector::unhookSteam();

	return 0;
}