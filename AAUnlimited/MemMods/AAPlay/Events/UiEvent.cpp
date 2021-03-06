#include "StdAfx.h"

namespace PlayInjections {
namespace UIEvent {

DWORD orig_uievent;

int __declspec(naked) __cdecl OrigUiEvent(int evt, int par1, int par2, int par3, int time1, int time2, int tim3)
{
	__asm {
		push ebp
		mov ebp, esp
		and esp, 0FFFFFFF8h
		jmp[orig_uievent]
	}
}

// These events take place to transition game state according various UI actions
// (show class roster, show navigation map etc).
//
// state - is the state number, ie identifies what the user has done, small number between 0 - ~20
// par1 and par2 are usually between 0..2
// par3 unknown
// time1 and time2 are time values in ms, probably speed of fadeout
int __cdecl UiEventHook(int state, int par1, int par2, int par3, int time1, int time2, int time3) {
	LUA_EVENT_NORET("ui_event", state,par1,par2,par3,time1,time2, time3)
	return OrigUiEvent(state, par1, par2, par3, time1, time2, time3);
}

void Inject() {
	orig_uievent = General::GameBase + 0xFC7A6;
	Hook((BYTE*)orig_uievent-6,
	{ 0x55, 0x8b, 0xec, 0x83, 0xe4, 0xf8 },                                                               //expected values
	{ 0x90, 0xE9, HookControl::RELATIVE_DWORD, (DWORD)&UiEventHook}, //redirect to our function
		NULL);
}
}
}