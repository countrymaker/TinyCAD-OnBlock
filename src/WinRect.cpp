////////////////////////////////////////////////////////////////
// MSDN Magazine -- October 2002
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 6.0 on Windows XP. Tab size=3.
//
#include "StdAfx.h"
#include "WinMgr.h"

//////////////////
// Construct from args
//
WINRECT::WINRECT(WORD f, UINT id, LONG p)
{
	memset(this, 0, sizeof(WINRECT));
	flags = f;
	nID = id;
	param = p;
}

//////////////////
// Get the parent of a given WINRECT. To find the parent, chase the prev
// pointer to the start of the list, then take the item before that in
// memory. 
//
WINRECT* WINRECT::Parent()
{
	WINRECT* pEntry;
	for (pEntry = this; pEntry->Prev(); pEntry = pEntry->Prev())
	{
		; // go backwards to the end
	}
	// the entry before the first child is the group
	WINRECT *parent = pEntry - 1;
	ASSERT(parent->IsGroup());
	return parent;
}

//////////////////
// Get group margins
//
BOOL WINRECT::GetMargins(int& w, int& h)
{
	if (IsGroup())
	{
		w = (short) LOWORD(param);
		h = (short) HIWORD(param);
		return TRUE;
	}
	w = h = 0;
	return FALSE;
}

//////////////////
// Initialize map: set up all the next/prev pointers. This converts the
// linear array to a more convenient linked list. Called from END_WINDOW_MAP.
//
WINRECT* WINRECT::InitMap(WINRECT* pWinMap, WINRECT* parent)
{
	ASSERT(pWinMap);

	WINRECT* pwrc = pWinMap; // current table entry
	WINRECT* prev = NULL; // previous entry starts out none

	while (!pwrc->IsEndGroup())
	{
		pwrc->prev = prev;
		pwrc->next = NULL;
		if (prev) prev->next = pwrc;
		prev = pwrc;
		if (pwrc->IsGroup())
		{
			pwrc = InitMap(pwrc + 1, pwrc); // recurse! Returns end-of-grp
			ASSERT(pwrc->IsEndGroup());
		}
		pwrc++;
	}
	// safety checks
	ASSERT(pwrc->IsEndGroup());
	ASSERT(prev);
	ASSERT(prev->next==NULL);
	return parent ? pwrc : NULL;
}

