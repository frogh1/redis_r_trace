


#include "product.h"
#include "vhash.h"

/*

          |  HashHead  | HashBucket .....  HashBucket 

*/

typedef struct _tagVHashHead
{
	unsigned int uiModeId;
	VHASH_UINT4 uiBucketCount;
	VHASH_UINT4 uiDataLen;	
	VHASH_UINT4 (* pfnVHash_Calc)(void *pkey);
	VHASH_UINT4 (* pfnVHash_Compare)(void *pkey,void *pData);
	VHASH_UINT4 (* pfnVHash_IsUpdate)(void *pData,void *pNewData);
	VHASH_UINT4 (* pfnVHash_clean)(void *pData);
	VHASH_UINT4 (* pfnVHash_cpy)(void *pDestData,void *pSrcData,unsigned int uiLen);
	int (*pfnPrintf)(const char * format, ...);

	void* (*fncalloc)(unsigned int n,unsigned int uisize);
	unsigned int uiTotal;

	
	LIST_ITEM FreeQueue;
	
	SPINLOCK_S spinlock;
}VHASHHEAD;


typedef struct _tagVHASH_BUCKET
{
	VHASH_UINT4 ulDataNodeSum;
	LIST_ENTRY ListHead;
}VHASH_BUCKET;

typedef struct _tagDATANODE
{
	LIST_ENTRY listentry;
	char pData[1];
}DATANODE;


VHASHHEAD **g_astVHashHead = NULL;

unsigned int g_uiMaxVhashModules = 0;

#define  GET_VHASH_HEAD(HashModeID) (g_astVHashHead[HashModeID])

#define  GET_VHASH_BUCKET(pstHashHead,bucket_idx) \
	(VHASH_BUCKET *)((unsigned char *)pstHashHead + sizeof(VHASHHEAD) \
	                + sizeof(VHASH_BUCKET)*(bucket_idx))

DATANODE *VHash_Malloc(VHASHHEAD *pstVHashHead)
{
	DATANODE * pstDateNode = NULL;

	LIST_ENTRY *pListHead;
	LIST_ENTRY *entry;		
	LIST_ITEM *pListItem;

	//SpinLock_Lock(&pstVHashHead->spinlock);

	pListItem = &pstVHashHead->FreeQueue;	
	pListHead = &pListItem->ListHead;

	if(NULL != pstVHashHead->pfnPrintf)
	{
		pstVHashHead->pfnPrintf("VHash_Malloc 1 tail %p head %p Flink %p FlinkFlink %p %u\n",pListHead->Blink,pListHead,pListHead->Flink,pListHead->Flink->Flink,pListItem->ulListLen);
	}

	if(!IsListEmpty(pListHead))
	{
		entry = RemoveHeadList(pListHead);
		pListItem->ulListLen--;
		pstDateNode = CONTAINING_RECORD(entry,DATANODE,listentry);	
	}

	if(NULL != pstVHashHead->pfnPrintf)
	{
		pstVHashHead->pfnPrintf("VHash_Malloc 2 tail %p head %p Flink %p FlinkFlink %p\n",pListHead->Blink,pListHead,pListHead->Flink,pListHead->Flink->Flink);
	}
	
	

	return pstDateNode;
	
}

void VHash_Free(VHASHHEAD *pstVHashHead,DATANODE *pstDateNode)
{

		LIST_ENTRY *pListHead;	
		LIST_ITEM *pListItem;

		//SpinLock_Lock(&pstVHashHead->spinlock);

		pListItem = &pstVHashHead->FreeQueue;	
		pListHead = &pListItem->ListHead;

		InsertTailList(&pListItem->ListHead,&pstDateNode->listentry);
		pListItem->ulListLen++;	
}




int VHash_Init(unsigned int uiMaxVhashModules)
{
	VHASH_UINT4 i;
	VHASHHEAD **ppstVHashHead = NULL;
	ppstVHashHead = (VHASHHEAD **)malloc(uiMaxVhashModules * sizeof(VHASHHEAD *));
	if(NULL == ppstVHashHead)
	{
		return -1;
	}

	g_astVHashHead = ppstVHashHead;

	g_uiMaxVhashModules = uiMaxVhashModules;
	
	for(i = 0; i < uiMaxVhashModules; i++)
	{
		GET_VHASH_HEAD(i) = NULL;
	}

	return 0;
}



VHASH_UINT4 VHash_Create_Moudle(unsigned int en_moudle_id,VHASH_MOUDLE *pVHashMoudle)
{
	unsigned long ulSize = 0;
	VHASH_UINT4 i;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;

	if(NULL == pVHashMoudle)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL != pstVHashHead)
		return __VHASH_OK;

	ulSize = sizeof(VHASHHEAD) + ( sizeof(VHASH_BUCKET) * pVHashMoudle->uiBucketCount ); 
	pstVHashHead = (VHASHHEAD *)malloc(ulSize);

	if(NULL == pstVHashHead)
		return __VHASH_NOT_INNIF;


	memset(pstVHashHead,0,sizeof(VHASHHEAD));

	pstVHashHead->uiModeId = pVHashMoudle->uiModeId;
	pstVHashHead->uiBucketCount = pVHashMoudle->uiBucketCount;
	pstVHashHead->uiDataLen = pVHashMoudle->uiDataLen;

	pstVHashHead->pfnVHash_Calc = pVHashMoudle->pfnVHash_Calc;
	pstVHashHead->pfnVHash_Compare = pVHashMoudle->pfnVHash_Compare;
	pstVHashHead->pfnVHash_IsUpdate = pVHashMoudle->pfnVHash_IsUpdate;
	pstVHashHead->pfnVHash_clean = pVHashMoudle->pfnVHash_clean;
	pstVHashHead->pfnVHash_cpy = pVHashMoudle->pfnVHash_cpy;
	pstVHashHead->pfnPrintf = pVHashMoudle->pfnPrintf;
	pstVHashHead->fncalloc = pVHashMoudle->fncalloc;

	pstVHashHead->uiTotal = 0;
	InitSpinLock(&pstVHashHead->spinlock);

	/* 初始化bucket */
	for(i = 0; i < pstVHashHead->uiBucketCount; i++)
	{
		pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,i);
		pstVHashBucket->ulDataNodeSum = 0;
		InitializeListHead(&pstVHashBucket->ListHead);
	}


	GET_VHASH_HEAD(en_moudle_id) = pstVHashHead;

	
	return __VHASH_OK;
}


VHASH_UINT4 VHash_Destory_Moudle(unsigned int en_moudle_id)
{
	VHASH_UINT4 i = 0;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	LIST_ENTRY *pListHead;
	LIST_ENTRY *entry;
	DATANODE * pstDateNode = NULL;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return __VHASH_ERR;


	SpinLock_Lock(&pstVHashHead->spinlock);

	/* 扫描bucket */
	for(i = 0; i < pstVHashHead->uiBucketCount; i++)
	{
		pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,i);
		pListHead = &pstVHashBucket->ListHead;

		/* 清空队列 */
		while(!IsListEmpty(pListHead))
		{
			entry = RemoveHeadList(pListHead);
			pstDateNode = CONTAINING_RECORD(entry,DATANODE,listentry);
			if(NULL != pstVHashHead->pfnVHash_clean)
			{
				pstVHashHead->pfnVHash_clean((void *)pstDateNode->pData);
			}
			
			free(pstDateNode);
		}
		
		pstVHashBucket->ulDataNodeSum = 0;
	}	

	SpinLock_unLock(&pstVHashHead->spinlock);

	DestroySpinLock(&pstVHashHead->spinlock);

	pstVHashHead->uiTotal = 0;

	GET_VHASH_HEAD(en_moudle_id) = NULL;

	free(pstVHashHead);
	
	return __VHASH_OK;
}


VHASH_UINT4 VHash_Clean_Moudle(unsigned int en_moudle_id)
{
	VHASH_UINT4 i = 0;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	LIST_ENTRY *pListHead;
	LIST_ENTRY *entry;
	DATANODE * pstDateNode = NULL;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return __VHASH_ERR;


	SpinLock_Lock(&pstVHashHead->spinlock);

	/* 扫描bucket */
	for(i = 0; i < pstVHashHead->uiBucketCount; i++)
	{
		pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,i);
		pListHead = &pstVHashBucket->ListHead;

		/* 清空队列 */
		while(!IsListEmpty(pListHead))
		{
			entry = RemoveHeadList(pListHead);
			pstDateNode = CONTAINING_RECORD(entry,DATANODE,listentry);
			if(NULL != pstVHashHead->pfnVHash_clean)
			{
				pstVHashHead->pfnVHash_clean((void *)pstDateNode->pData);
			}
			
			free(pstDateNode);
		}
		
		pstVHashBucket->ulDataNodeSum = 0;
	}	

	pstVHashHead->uiTotal = 0;
	
	SpinLock_unLock(&pstVHashHead->spinlock);

	return __VHASH_OK;
}

VHASH_UINT4 VHash_Get_Node(unsigned int en_moudle_id,void *pKey,void **ppData)
{
	VHASH_UINT4 ulHash = 0;
	VHASH_UINT4 ulRet;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	LIST_ENTRY *pListHead;
	LIST_ENTRY *entry;
	DATANODE * pstDateNode = NULL;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;

	if(NULL == pKey)
		return __VHASH_ERR;
	

	if(NULL == ppData)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return __VHASH_ERR;

	SpinLock_Lock(&pstVHashHead->spinlock);
	
	ulHash = pstVHashHead->pfnVHash_Calc(pKey);
	pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,ulHash);
	pListHead = &pstVHashBucket->ListHead;

	for(entry = pListHead->Flink; entry != pListHead; entry = entry->Flink)
	{
		pstDateNode = CONTAINING_RECORD(entry,DATANODE,listentry);
		ulRet = pstVHashHead->pfnVHash_Compare(pKey,(void *)pstDateNode->pData);
		if(__VHASH_OK == ulRet)
		{
			*ppData = (void *)pstDateNode->pData;
			
			SpinLock_unLock(&pstVHashHead->spinlock);
			return __VHASH_OK;
		}
	}


	SpinLock_unLock(&pstVHashHead->spinlock);
	return __VHASH_NOT_FOUND;


}


VHASH_UINT4 VHash_Add_Node(unsigned int en_moudle_id,void *pKey,void *pData)
{
	VHASH_UINT4 ulHash = 0;
	VHASH_UINT4 ulRet = 0;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	LIST_ENTRY *pListHead;
	LIST_ENTRY *entry;
	DATANODE * pstDateNode = NULL;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;
	
	if(NULL == pKey)
		return __VHASH_ERR;

	if(NULL == pData)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return __VHASH_ERR;


	if(NULL == pstVHashHead->pfnVHash_Calc)
		return __VHASH_INTERFACE_ERR;

	if(NULL == pstVHashHead->pfnVHash_Compare)
		return __VHASH_INTERFACE_ERR;

	if(NULL == pstVHashHead->pfnVHash_IsUpdate)
		return __VHASH_INTERFACE_ERR;

	if(NULL == pstVHashHead->pfnVHash_cpy)
		return __VHASH_INTERFACE_ERR;

	SpinLock_Lock(&pstVHashHead->spinlock);
	
	ulHash = pstVHashHead->pfnVHash_Calc(pKey);
	pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,ulHash);
	pListHead = &pstVHashBucket->ListHead;

	/* 遍历冲突队列*/
	for(entry = pListHead->Flink; entry != pListHead; entry = entry->Flink)
	{
		pstDateNode = CONTAINING_RECORD(entry,DATANODE,listentry);
		ulRet = pstVHashHead->pfnVHash_Compare(pKey,(void *)pstDateNode->pData);

		if(__VHASH_OK == ulRet)
		{
			if(__VHASH_OK == pstVHashHead->pfnVHash_IsUpdate(pstDateNode->pData,pData))
			{
				pstVHashHead->pfnVHash_cpy(pstDateNode->pData,pData,pstVHashHead->uiDataLen);
				//memcpy(pstDateNode->pData,pData,pstVHashHead->uiDataLen);

				SpinLock_unLock(&pstVHashHead->spinlock);
				return __VHASH_OK;
			}

			SpinLock_unLock(&pstVHashHead->spinlock);
			return __VHASH_EXIST;
		}
	}

	
	if(NULL == pstDateNode)
	{
		SpinLock_unLock(&pstVHashHead->spinlock);
		return __VHASH_NOT_INNIF;
	}

	memset(pstDateNode->pData,0,pstVHashHead->uiDataLen);
	pstVHashHead->pfnVHash_cpy(pstDateNode->pData,pData,pstVHashHead->uiDataLen);
	//memcpy(pstDateNode->pData,pData,pstVHashHead->uiDataLen);
	InsertTailList(pListHead,&pstDateNode->listentry);
	pstVHashBucket->ulDataNodeSum++;
	pstVHashHead->uiTotal++;

	SpinLock_unLock(&pstVHashHead->spinlock);
	return __VHASH_OK;
}


VHASH_UINT4 VHash_Add_Node2(unsigned int en_moudle_id,void *pKey,void *pData,void **ppNodeData)
{
	VHASH_UINT4 ulHash = 0;
	VHASH_UINT4 ulRet = 0;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	LIST_ENTRY *pListHead;
	LIST_ENTRY *entry;
	DATANODE * pstDateNode = NULL;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;
	
	if(NULL == pKey)
		return __VHASH_ERR;

	if(NULL == pData)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return __VHASH_ERR;


	if(NULL == pstVHashHead->pfnVHash_Calc)
		return __VHASH_INTERFACE_ERR;

	if(NULL == pstVHashHead->pfnVHash_Compare)
		return __VHASH_INTERFACE_ERR;

	if(NULL == pstVHashHead->pfnVHash_IsUpdate)
		return __VHASH_INTERFACE_ERR;

	if(NULL == pstVHashHead->pfnVHash_cpy)
		return __VHASH_INTERFACE_ERR;

	SpinLock_Lock(&pstVHashHead->spinlock);
	
	ulHash = pstVHashHead->pfnVHash_Calc(pKey);
	pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,ulHash);
	pListHead = &pstVHashBucket->ListHead;

	/* 遍历冲突队列*/
	for(entry = pListHead->Flink; entry != pListHead; entry = entry->Flink)
	{
		pstDateNode = CONTAINING_RECORD(entry,DATANODE,listentry);
		ulRet = pstVHashHead->pfnVHash_Compare(pKey,(void *)pstDateNode->pData);

		if(__VHASH_OK == ulRet)
		{
			if(__VHASH_OK == pstVHashHead->pfnVHash_IsUpdate(pstDateNode->pData,pData))
			{
				if(NULL != pstVHashHead->pfnVHash_clean)
				{
					pstVHashHead->pfnVHash_clean((void *)pstDateNode->pData);
				}
				
				ulRet = pstVHashHead->pfnVHash_cpy(pstDateNode->pData,pData,pstVHashHead->uiDataLen);
				//memcpy(pstDateNode->pData,pData,pstVHashHead->uiDataLen);
				if(__VHASH_OK != ulRet)
				{
					if(NULL != pstVHashHead->pfnVHash_clean)
					{
						pstVHashHead->pfnVHash_clean((void *)pstDateNode->pData);
					}
					
					SpinLock_unLock(&pstVHashHead->spinlock);
					return ulRet;
				}
				
				if(NULL != ppNodeData)
				{
					*ppNodeData = (void *)pstDateNode->pData;
				}

				SpinLock_unLock(&pstVHashHead->spinlock);
				return __VHASH_UPDATE_OK;
			}

			SpinLock_unLock(&pstVHashHead->spinlock);
			return __VHASH_EXIST;
		}
	}

	pstDateNode = (DATANODE *)malloc(sizeof(DATANODE) + pstVHashHead->uiDataLen);
	if(NULL == pstDateNode)
	{
		SpinLock_unLock(&pstVHashHead->spinlock);
		return __VHASH_NOT_INNIF;
	}

	memset(pstDateNode->pData,0,pstVHashHead->uiDataLen);
	ulRet = pstVHashHead->pfnVHash_cpy(pstDateNode->pData,pData,pstVHashHead->uiDataLen);
	//memcpy(pstDateNode->pData,pData,pstVHashHead->uiDataLen);
	if(__VHASH_OK != ulRet)
	{
		if(NULL != pstVHashHead->pfnVHash_clean)
		{
			pstVHashHead->pfnVHash_clean((void *)pstDateNode->pData);
		}

		free(pstDateNode);
		SpinLock_unLock(&pstVHashHead->spinlock);
		return ulRet;
	}
	
	InsertTailList(pListHead,&pstDateNode->listentry);
	pstVHashBucket->ulDataNodeSum++;
	pstVHashHead->uiTotal++;

	if(NULL != ppNodeData)
	{
		*ppNodeData = (void *)pstDateNode->pData;
	}

	SpinLock_unLock(&pstVHashHead->spinlock);
	return __VHASH_OK;
}



VHASH_UINT4 VHash_Get_NextNode(unsigned int en_moudle_id,void *pKey,void *pData,void **ppNodeData)
{
	VHASH_UINT4 ulHash = 0;
	VHASH_UINT4 ulRet = 0;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	LIST_ENTRY *pListHead;
	LIST_ENTRY *entry;
	DATANODE * pstDateNode = NULL;
	unsigned int i = 0;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return __VHASH_ERR;


	if(NULL == pstVHashHead->pfnVHash_Calc)
		return __VHASH_INTERFACE_ERR;

	if(NULL != pKey && NULL == pData)
		return __VHASH_ERR;
	
	SpinLock_Lock(&pstVHashHead->spinlock);

	if(NULL != pKey)
	{
		ulHash = pstVHashHead->pfnVHash_Calc(pKey);
		i = ulHash;
		pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,i);
		pListHead = &pstVHashBucket->ListHead;

		pstDateNode = CONTAINING_RECORD(pData,DATANODE,pData[0]);
		entry = &pstDateNode->listentry;
		entry = entry->Flink;

		if(entry != pListHead)
		{
			pstDateNode = CONTAINING_RECORD(entry,DATANODE,listentry);
			if(NULL != ppNodeData)
			{
				*ppNodeData = (void *)pstDateNode->pData;
				goto exit;
			}
		}
		else
		{
			i++;
		}
				
	}

	for(; i < pstVHashHead->uiBucketCount; i++)
	{
		pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,i);
		pListHead = &pstVHashBucket->ListHead;

		entry = pListHead;

		entry = entry->Flink;

		if(entry == pListHead)
		{
			continue;
		}
		
		
		pstDateNode = CONTAINING_RECORD(entry,DATANODE,listentry);
		if(NULL != ppNodeData)
		{
			*ppNodeData = (void *)pstDateNode->pData;
		}

		break;

	}


exit:

	SpinLock_unLock(&pstVHashHead->spinlock);
	return __VHASH_OK;
}


VHASH_UINT4 VHash_Del_Node(unsigned int en_moudle_id,void *pKey)
{
	VHASH_UINT4 ulHash = 0;
	VHASH_UINT4 ulRet = 0;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	LIST_ENTRY *pListHead;
	LIST_ENTRY *entry;
	DATANODE * pstDateNode = NULL;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;
	
	if(NULL == pKey)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return __VHASH_ERR;


	if(NULL == pstVHashHead->pfnVHash_Calc)
		return __VHASH_INTERFACE_ERR;

	if(NULL == pstVHashHead->pfnVHash_Compare)
		return __VHASH_INTERFACE_ERR;
	
	SpinLock_Lock(&pstVHashHead->spinlock);

	ulHash = pstVHashHead->pfnVHash_Calc(pKey);
	pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,ulHash);
	pListHead = &pstVHashBucket->ListHead;

	/* 遍历冲突队列*/
	for(entry = pListHead->Flink; entry != pListHead; )
	{
		pstDateNode = CONTAINING_RECORD(entry,DATANODE,listentry);
		entry = entry->Flink;
		ulRet = pstVHashHead->pfnVHash_Compare(pKey,pstDateNode->pData);

		if(__VHASH_OK == ulRet)
		{
			RemoveEntryList(&pstDateNode->listentry);
			if(NULL != pstVHashHead->pfnVHash_clean)
			{
				pstVHashHead->pfnVHash_clean((void *)pstDateNode->pData);
			}	

			free(pstDateNode);
			pstVHashBucket->ulDataNodeSum--;
			pstVHashHead->uiTotal--;

			SpinLock_unLock(&pstVHashHead->spinlock);
			return __VHASH_OK;
		}
	}

	SpinLock_unLock(&pstVHashHead->spinlock);
	return __VHASH_NOT_FOUND;
}


VHASH_UINT4 VHash_Node_Stat(unsigned int en_moudle_id,unsigned int *puiNodeStat)
{
	VHASH_UINT4 ulHash = 0;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;
	
	if(NULL == puiNodeStat)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return __VHASH_ERR;

	SpinLock_Lock(&pstVHashHead->spinlock);

	for(ulHash = 0; ulHash < pstVHashHead->uiBucketCount;ulHash++)
	{
		pstVHashBucket = GET_VHASH_BUCKET(pstVHashHead,ulHash);
		puiNodeStat[ulHash] = pstVHashBucket->ulDataNodeSum;
	}

	SpinLock_unLock(&pstVHashHead->spinlock);
	return __VHASH_OK;
}

VHASH_UINT4 VHash_Node_Total(unsigned int en_moudle_id,unsigned int *puiTotal)
{
	VHASH_UINT4 ulHash = 0;
	VHASHHEAD *pstVHashHead = NULL;
	VHASH_BUCKET *pstVHashBucket = NULL;
	
	if(en_moudle_id >= g_uiMaxVhashModules)
		return __VHASH_ERR;
	
	if(NULL == puiTotal)
		return __VHASH_ERR;
	
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return __VHASH_ERR;

	*puiTotal = pstVHashHead->uiTotal;
	
	return __VHASH_OK;
}

void VHash_Lock(unsigned int en_moudle_id)
{
	VHASHHEAD *pstVHashHead = NULL;
	if(en_moudle_id >= g_uiMaxVhashModules)
		return ;
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return ;

	SpinLock_Lock(&pstVHashHead->spinlock);
}

void VHash_unLock(unsigned int en_moudle_id)
{
	VHASHHEAD *pstVHashHead = NULL;
	if(en_moudle_id >= g_uiMaxVhashModules)
		return ;
	pstVHashHead = GET_VHASH_HEAD(en_moudle_id);
	if(NULL == pstVHashHead)
		return ;

	SpinLock_unLock(&pstVHashHead->spinlock);
}



