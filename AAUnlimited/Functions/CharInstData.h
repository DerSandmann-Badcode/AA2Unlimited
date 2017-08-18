#pragma once

#include <vector>

#include "External\ExternalClasses\CharacterStruct.h"
#include "Functions\AAUCardData.h"

/*
 * Contains data about a character that currently exists in a game.
 * Apart from the pointer to the original struct, this includes the AAU card data,
 * currently loaded hair pointers etc
 */
class CharInstData
{
public:
	struct ActionParamStruct {
		DWORD conversationId = -1;
		DWORD movementType;
		DWORD roomTarget;
		DWORD unknown; //always -1
		ExtClass::CharacterActivity* target1;
		ExtClass::CharacterActivity* target2;
		DWORD unknown2; //always 1, though initialized to 2
		static inline void bindLua() {
#define LUA_CLASS CharInstData::ActionParamStruct
			LUA_BIND(conversationId)
			LUA_BIND(movementType)
			LUA_BIND(roomTarget)
			LUA_BIND(target1)
			LUA_BIND(target2)
		}
#undef LUA_CLASS
	};

public:

	CharInstData();
	~CharInstData();

	static inline void bindLua() {
#define LUA_CLASS CharInstData
		LUA_NAME;
		LUA_BIND(m_char);
#undef LUA_CLASS
	}



	ExtClass::CharacterStruct* m_char;
	AAUCardData m_cardData;

	std::vector<std::pair<AAUCardData::HairPart,ExtClass::XXFile*>> m_hairs[4];

	ActionParamStruct m_forceAction;

	int charOffset;

	void Reset();
	inline bool IsValid() { return m_char != NULL; }
	inline bool Editable() {
		const DWORD femaleRule[]{ 0x353254, 0x2C, 0 };
		const DWORD maleRule[]{ 0x353254, 0x30, 0 };
		m_char = (ExtClass::CharacterStruct*) ExtVars::ApplyRule(femaleRule);
		if (m_char == NULL) (ExtClass::CharacterStruct*) ExtVars::ApplyRule(maleRule);
		return m_char != NULL;
	}

};

