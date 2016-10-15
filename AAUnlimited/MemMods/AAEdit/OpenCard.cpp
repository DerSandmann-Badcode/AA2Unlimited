#include "OpenCard.h"

#include <Windows.h>
#include <intrin.h>

#include "MemMods\Hook.h"
#include "General\ModuleInfo.h"
#include "General\Util.h"
#include "Files\Config.h"
#include "Functions\AAEdit\Globals.h"
#include "Functions\AAEdit\UnlimitedDialog.h"

namespace EditInjections {
namespace OpenCard {


void __stdcall ReadUnlimitData(HANDLE hFile, DWORD /*illusionDataOffset*/) {
	//clear current data
	AAEdit::g_currChar.m_cardData.Reset();
	//first, find our unlimited data
	DWORD lo, hi;
	lo = GetFileSize(hFile, &hi);
	BYTE* needed = new BYTE[lo];
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	ReadFile(hFile, needed, lo, &hi, NULL);
	//load data
	AAEdit::g_currChar.m_cardData.FromFileBuffer((char*)needed, lo);
	AAEdit::g_currChar.m_cardData.DumpSavedOverrideFiles();
	BOOL savedFiles = AAEdit::g_currChar.m_cardData.HasFilesSaved();
	AAEdit::g_AAUnlimitDialog.SetSaveFiles(savedFiles);
	AAEdit::g_AAUnlimitDialog.Refresh();
}

void __stdcall ReadUnlimitDataV2(const wchar_t* card) {
	AAEdit::g_currChar.m_cardData.Reset();

	HANDLE hFile = CreateFile(card,FILE_GENERIC_READ | FILE_GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if (hFile == INVALID_HANDLE_VALUE || hFile == NULL) return;

	//read file first, we will need to truncate it anyway
	DWORD hi,lo = GetFileSize(hFile,&hi);
	BYTE* fileBuffer = new BYTE[lo];
	ReadFile(hFile,fileBuffer,lo,&hi,NULL);
	//read from data
	bool suc = AAEdit::g_currChar.m_cardData.FromFileBuffer((char*)fileBuffer,lo);
	BOOL savedFiles = AAEdit::g_currChar.m_cardData.HasFilesSaved();
	AAEdit::g_AAUnlimitDialog.SetSaveFiles(savedFiles);
	AAEdit::g_AAUnlimitDialog.Refresh();

	suc = suc && AAEdit::g_currChar.m_cardData.DumpSavedOverrideFiles();
	if(!suc || !savedFiles || !g_Config.GetKeyValue(Config::SAVED_FILE_REMOVE).bVal) {
		//not an aau card
		CloseHandle(hFile);
		delete[] fileBuffer;
		return;
	}
	
	//remove the saved files
	BYTE* start = (BYTE*)AAEdit::g_currChar.m_cardData.ret_fileStart;
	BYTE* end = (BYTE*)AAEdit::g_currChar.m_cardData.ret_fileEnd;
	DWORD* chunkSize = (DWORD*)AAEdit::g_currChar.m_cardData.ret_chunkSize;
	DWORD currSize = _byteswap_ulong(*chunkSize);
	currSize -= (end-start);
	*chunkSize = _byteswap_ulong(currSize);

	LONG himove = 0;
	SetFilePointer(hFile,0,&himove,FILE_BEGIN);
	WriteFile(hFile,fileBuffer,(DWORD)(start-fileBuffer),&hi,NULL);
	WriteFile(hFile,end,lo - (end-fileBuffer),&hi,NULL);
	SetEndOfFile(hFile);

	CloseHandle(hFile);
	delete[] fileBuffer;
}

DWORD ReadUnlimitDataOriginal;
void __declspec(naked) ReadUnlimitDataRedirect() {
	__asm {
		push [esp+8] //offset of illusions data
		push [esp+8] //formerly esp+4, file handle
		call ReadUnlimitData
		mov eax, [ReadUnlimitDataOriginal] //note: eax contains AA2Edit-exe+2C41EC now
		jmp dword ptr [eax] //redirect to original function; it will do the return for us
	}
}

void __declspec(naked) ReadUnlimitDataRedirectV2() {
	__asm {
		push [esp+8]
		push [esp+8]
		call [ReadUnlimitDataOriginal]
		add esp, 8
		push eax
		push [esp+8]
		call ReadUnlimitDataV2
		pop eax
		ret
	}
}

void ReadUnlimitDataInject() {
	//Part of the open card function. The last DWORD in the png file actually indicates the start of the custom data,
	//so that filesize-lastDword = offset of custom data.
	//edi is that offset at this point, and the SetFilePointer call will put the file (ebp) to that location.
	//the custom data part.
	/*AA2Edit.exe+127E1A - 6A 00                 - push 00 { 0 }
	AA2Edit.exe+127E1C - 6A 00                 - push 00 { 0 }
	AA2Edit.exe+127E1E - 57                    - push edi
	AA2Edit.exe+127E1F - 55                    - push ebp
	AA2Edit.exe+127E20 - FF 15 EC414F00        - call dword ptr [AA2Edit.exe+2C41EC] { ->->KERNELBASE.SetFilePointer }
	*/
	/*DWORD address = General::GameBase + 0x127E20;
	DWORD redirectAddress = (DWORD)(&ReadUnlimitDataRedirect);
	Hook((BYTE*)address,
		{ 0xFF, 0x15, 0xEC, 0x41, 0x4f, 0x00 },							//expected values
		{ 0xE8, HookControl::RELATIVE_DWORD, redirectAddress, 0x90 },	//redirect to our function
		NULL);
	ReadUnlimitDataOriginal = General::GameBase + 0x2C41EC;*/

	//the open card function. eax (second paramter) is the full path to the card, as a wchar_t
	/*AA2Edit.exe+1270BF - 50                    - push eax
	AA2Edit.exe+1270C0 - 51                    - push ecx
	AA2Edit.exe+1270C1 - 88 5C 24 34           - mov [esp+34],bl
	AA2Edit.exe+1270C5 - 89 5C 24 38           - mov [esp+38],ebx
	AA2Edit.exe+1270C9 - E8 920C0000           - call AA2Edit.exe+127D60
	AA2Edit.exe+1270CE - 83 C4 08              - add esp,08 { 8 }
	*/
	DWORD address = General::GameBase + 0x1270C9;
	DWORD redirectAddress = (DWORD)(&ReadUnlimitDataRedirectV2);
	Hook((BYTE*)address,
	{ 0xE8, 0x92, 0x0C, 0x00, 0x00 },							//expected values
	{ 0xE8, HookControl::RELATIVE_DWORD, redirectAddress },	//redirect to our function
		&ReadUnlimitDataOriginal);
}


}
}