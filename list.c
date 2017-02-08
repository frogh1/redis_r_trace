
#include "list.h"


void InitializeListHead(PLIST_ENTRY ListHead )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

 bool RemoveEntryList(PLIST_ENTRY Entry)
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (bool)(Flink == Blink);
}

 PLIST_ENTRY RemoveHeadList(
    PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}

 PLIST_ENTRY RemoveTailList(
    PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}


 void InsertTailList(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}


 void InsertHeadList(
    PLIST_ENTRY ListHead,
    PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

 void InsertBeforeEntry(
    PLIST_ENTRY BaseEntry,
    PLIST_ENTRY InsertEntry
    )
{
	PLIST_ENTRY Blink;

    Blink=BaseEntry->Blink;

	BaseEntry->Blink=InsertEntry;
	InsertEntry->Flink=BaseEntry;
	InsertEntry->Blink=Blink;
	Blink->Flink=InsertEntry;

}


 void ChangeHead(
    PLIST_ENTRY pOldhead,
    PLIST_ENTRY pNewhead
    )
{
	InitializeListHead(pNewhead);
    
	if(!IsListEmpty(pOldhead))
	{
		PLIST_ENTRY Flink = pOldhead->Flink;
		PLIST_ENTRY Blink = pOldhead->Blink;

		Flink->Blink = pNewhead;
		Blink->Flink = pNewhead;
		pNewhead->Flink = Flink;
		pNewhead->Blink = Blink; 
		InitializeListHead(pOldhead);
	}
}


//
//
//  PSINGLE_LIST_ENTRY
//  PopEntryList(
//      PSINGLE_LIST_ENTRY ListHead
//      );
//



//
//  void
//  PushEntryList(
//      PSINGLE_LIST_ENTRY ListHead,
//      PSINGLE_LIST_ENTRY Entry
//      );
//


/*
NTKERNELAPI
PLIST_ENTRY
FASTCALL
ExInterlockedInsertHeadList (
    PLIST_ENTRY ListHead,
    PLIST_ENTRY ListEntry,
    PKSPIN_LOCK Lock
    );

NTKERNELAPI
PLIST_ENTRY
FASTCALL
ExInterlockedInsertTailList (
    PLIST_ENTRY ListHead,
    PLIST_ENTRY ListEntry,
    PKSPIN_LOCK Lock
    );

NTKERNELAPI
PLIST_ENTRY
FASTCALL
ExInterlockedRemoveHeadList (
    PLIST_ENTRY ListHead,
    PKSPIN_LOCK Lock
    );*/







