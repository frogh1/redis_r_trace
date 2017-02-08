

#ifndef _LIST_H_
#define _LIST_H_

typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

#define CONTAINING_RECORD(address, type, field) ((type *)( \
                                                  (char *)(address) - \
                                                  (unsigned long)(&((type *)0)->field)))

#define bool int
#define true 1
#define false 0

typedef struct _tagLIST_ITEM
{
	LIST_ENTRY ListHead;
	unsigned long ulListLen;
}LIST_ITEM;

#define IsListEmpty(ListHead) ((ListHead)->Flink == (ListHead))

void InitializeListHead(PLIST_ENTRY ListHead );

bool RemoveEntryList(PLIST_ENTRY Entry);

PLIST_ENTRY RemoveHeadList(PLIST_ENTRY ListHead);

PLIST_ENTRY RemoveTailList(PLIST_ENTRY ListHead);

void InsertTailList(PLIST_ENTRY ListHead,PLIST_ENTRY Entry);

void InsertHeadList(PLIST_ENTRY ListHead,PLIST_ENTRY Entry);

void InsertBeforeEntry(PLIST_ENTRY BaseEntry,PLIST_ENTRY InsertEntry);

void ChangeHead(PLIST_ENTRY pOldhead,PLIST_ENTRY pNewhead);
 
#define PopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
        PSINGLE_LIST_ENTRY FirstEntry;\
        FirstEntry = (ListHead)->Next;\
        if (FirstEntry != NULL) {     \
            (ListHead)->Next = FirstEntry->Next;\
        }                             \
    }

#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)


#endif // _LIST_H_

