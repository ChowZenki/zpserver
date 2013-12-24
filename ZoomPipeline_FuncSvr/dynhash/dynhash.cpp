#include <stdlib.h>
#include <stdio.h>
#include "dynhash.h"
namespace ZPHashTable{
///开始键值对、桶的定义和操作
/**------------------------------------------------------------*/
//Paires
typedef struct tag_cdyn_pair
{
    struct tag_cdyn_pair * next;
    //struct tag_cdyn_pair * prev;//双向链表是占用存储的，没有必要
    unsigned int key;
    void * value;
} CDNPAIR;

//Buckets
typedef struct tag_cdyn_bucket
{
#ifdef DEBUG
    int nItems;//占用内存，耗费时钟，调试用
#endif
    CDNPAIR * FirstPair;
} CDNBCT;

CDNBCT * bucket_malloc()
{
    CDNBCT * bct = (CDNBCT *)malloc(sizeof (CDNBCT));
    if (bct)
    {
#ifdef DEBUG
        bct->nItems = 0;
#endif
        bct->FirstPair = 0;
    }
    return bct;
}
// insert an item into bucket, a bi-direction link list.
int bucket_set(CDNBCT * bucket, unsigned int key,void * value)
{
    int res = 0;
    CDNPAIR * pairCur =  bucket->FirstPair;
    while (pairCur)
    {
        if (pairCur->key == key)
            break;
        pairCur = pairCur->next;
    }
    //Not found
    if (!pairCur)
    {
        CDNPAIR * newPair = (CDNPAIR * )malloc(sizeof(CDNPAIR));
        if (newPair)
        {
            newPair->key = key;
            newPair->value = value;
            newPair->next = bucket->FirstPair;
            //newPair->prev = 0;
            //if (bucket->FirstPair)
            //	bucket->FirstPair->prev = newPair;
            bucket->FirstPair = newPair;
#ifdef DEBUG
            bucket->nItems++;
#endif
            res = -1;
        }
    }
    else
        pairCur->value = value;
    return res;
}
// get an item
void * bucket_get(CDNBCT * bucket, unsigned int key,int * bfound)
{
    CDNPAIR * pairCur =  bucket->FirstPair;
    while (pairCur)
    {
        if (pairCur->key == key)
            break;
        pairCur = pairCur->next;
    }
    //Not found
    if (!pairCur)
    {
        * bfound = 0;
        return 0;
    }
    else
    {
        * bfound = -1;
        return pairCur->value;
    }
}
// find.
int bucket_contains(CDNBCT * bucket, unsigned int key)
{
    CDNPAIR * pairCur =  bucket->FirstPair;
    while (pairCur)
    {
        if (pairCur->key == key)
            break;
        pairCur = pairCur->next;
    }
    //Not found
    if (!pairCur)
        return 0;
    else
        return -1;
}


int bucket_remove( CDNBCT * bucket, unsigned int key)
{
    int res = 0;
    CDNPAIR * pairCur =  bucket->FirstPair;
    CDNPAIR * pairLast = 0;
    while (pairCur)
    {
        if (pairCur->key == key)
            break;
        pairLast = 	pairCur;
        pairCur = pairCur->next;
    }
    //Not found
    if (!pairCur)
        return res;
    res = -1;
    /* 双向链表浪费空间
    if (pairCur->prev)
    {
        pairCur->prev->next = pairCur->next;
        if (pairCur->next)
            pairCur->next->prev = pairCur->prev;
    }
    if (pairCur->next)
    {
        pairCur->next->prev = pairCur->prev;
        if (pairCur->prev)
            pairCur->prev->next = pairCur->next;
    }
    */
    if (pairLast)
        pairLast->next = pairCur->next;
    else
        bucket->FirstPair = pairCur->next;
    //if (bucket->FirstPair == pairCur)
    //	bucket->FirstPair = bucket->FirstPair->next;

    free (pairCur);
#ifdef DEBUG
    bucket->nItems--;
#endif
    return res;
}

void bucket_free( CDNBCT * bucket)
{
    CDNPAIR * pairCur =  bucket->FirstPair;
    while (pairCur)
    {
        CDNPAIR * delptr = pairCur;
        pairCur = pairCur->next;
        free(delptr);
    }
    free(bucket);
}
///开始哈希表内部定义和操作
/**------------------------------------------------------------*/
//Hash Tables
typedef struct tag_cdyn_hash_table{
    unsigned int totalElmts;	//current size of the table
    unsigned int level;			//current hash level
    int p;						//current split point
    float splitFactorMax;		//when mean payload larger than this, split will happen
    float splitFactorMin;		//when mean payload lower than this, merge will happen
    //items hold buckets and key-value pairs;
    int dirsz;
    CDNBCT * * directory;
} INTCDYNHASHTB;

///计算地址
unsigned int address(INTCDYNHASHTB * pTable, unsigned int v)
{
    //all those addresses after p are obtained using h % 2 ^ level
    //and all those before p are obtained usng h % 2 ^ (level+1)
    unsigned int  h = v % (2 << ((pTable->level) -1));
    if(h < pTable->p){
        h = v % (2 << (pTable->level));
    }
    return h;
}
///查找元素
int findElmt(INTCDYNHASHTB * ptable, unsigned int key, void ** val)
{
    int bFound = 0;
    CDNBCT * bucket = ptable->directory[address(ptable,key)];

    * val = bucket_get(bucket, key, &bFound);
    return bFound;
}

///核心算法，分裂表
void split_bucket(INTCDYNHASHTB * ptable)
{
    unsigned int cur_loc = ptable->p;
    int new_sz = 0, i = 0;
    CDNBCT ** newPt = 0;
    CDNPAIR * pairCur = 0;
    CDNBCT * oldBucketPtr = 0,  * newBucketPtr = 0;
    ptable->p++;
    if(ptable->p >= (ptable->dirsz >> 1)){
        ptable->level++;
        ptable->p = 0;
        new_sz = ptable->dirsz << 1;
        //insert new buckets addresses
        //ptable->dirsz = nInitBuckets;
        //ptable->directory =  (CDNBCT ** )malloc(sizeof (CDNBCT *) * nInitBuckets);
        newPt = (CDNBCT ** )malloc(sizeof (CDNBCT *) * new_sz);
        for (i=0;i<new_sz;i++)
        {
            if (i<ptable->dirsz)
                newPt[i] = ptable->directory[i];
            else
                newPt[i] = bucket_malloc();
        }
        free (ptable->directory);
        ptable->directory = newPt;
        ptable->dirsz = new_sz;
    }
    oldBucketPtr = ptable->directory[cur_loc];
    newBucketPtr = bucket_malloc();
    ptable->directory[cur_loc] = newBucketPtr;

    //Re-hash
    pairCur =  oldBucketPtr->FirstPair;
    while (pairCur)
    {
        unsigned int loc = address(ptable,pairCur->key);
        if(loc == cur_loc)
            bucket_set(newBucketPtr,pairCur->key,pairCur->value);
        else
            bucket_set(ptable->directory[loc],pairCur->key,pairCur->value);
        pairCur = pairCur->next;
    }

    bucket_free(oldBucketPtr);
}
///核心算法，收缩表
void merge_bucket(INTCDYNHASHTB * ptable)
{
    //merge the directories located at p and p+directory.size()/2
    int new_sz = 0 , i = 0;
    CDNBCT ** newPt = 0;
    CDNBCT * innerBucketPtr = 0,  * outerBucketPtr = 0;
    unsigned int next_loc = 0;
    CDNPAIR * pairCur  = 0;
    ptable->p--;
    if(ptable->p < 0){//no bucket has been split in this level
        new_sz = ptable->dirsz / 2;
        ptable->p = new_sz/2 - 1;
        ptable->level--;
        newPt = (CDNBCT ** )malloc(sizeof (CDNBCT *) * new_sz);
        for (i=0;i<ptable->dirsz;i++)
        {
            if (i<new_sz)
                newPt[i] = ptable->directory[i];
            else
                bucket_free(ptable->directory[i]);
        }
        free (ptable->directory);
        ptable->directory = newPt;
        ptable->dirsz = new_sz;
    }
    next_loc = ptable->p + ((ptable->dirsz) >> 1);
    innerBucketPtr = ptable->directory[ptable->p];
    outerBucketPtr = ptable->directory[next_loc];

    //Re-hash
    pairCur =  outerBucketPtr->FirstPair;
    while (pairCur)
    {
        bucket_set(innerBucketPtr,pairCur->key,pairCur->value);
        pairCur = pairCur->next;
    }
    bucket_free(outerBucketPtr);
    ptable->directory[next_loc] = bucket_malloc();
}

///开始哈希表外部方法定义和操作
/**------------------------------------------------------------*/
//init a hash table
CDYNHASHTB hash_init(float spFacMax, float spFacMin)
{
    int i = 0;
    int nInitBuckets = 0;
    struct tag_cdyn_hash_table * ptable = (struct tag_cdyn_hash_table *)
            malloc(sizeof (struct tag_cdyn_hash_table));
    if (ptable)
    {
        ptable->totalElmts = 0;
        ptable->level = 1;
        ptable->p = 0;
        ptable->splitFactorMax = spFacMax;
        ptable->splitFactorMin = spFacMin;
        nInitBuckets =  (2 << ptable->level);
        ptable->dirsz = nInitBuckets;
        ptable->directory =  (CDNBCT ** )malloc(sizeof (CDNBCT *) * nInitBuckets);
        for (i=0;i<nInitBuckets;i++)
            ptable->directory[i] = bucket_malloc();
    }
    return (CDYNHASHTB) ptable;
}

//insert an item [key:value] into table, or change existing value
void hash_set(CDYNHASHTB table, unsigned int key, void * value)
{
    int bInserted = 0;
    INTCDYNHASHTB * ptable = (INTCDYNHASHTB *)table;
    CDNBCT * bucket = ptable->directory[address(ptable,key)];
    bInserted = bucket_set(bucket,key,value);
    if (bInserted)
    {
        ptable->totalElmts++;
        if((float)(ptable->totalElmts)*2 / ptable->dirsz > ptable->splitFactorMax)
            split_bucket(ptable);
    }
}

//remove an item [key] from table
void hash_del(CDYNHASHTB table, unsigned int key)
{
    int bDeleted = 0;
    INTCDYNHASHTB * ptable = (INTCDYNHASHTB *)table;
    CDNBCT * bucket = ptable->directory[address(ptable,key)];
    bDeleted = bucket_remove(bucket,key);
    if (bDeleted)
    {
        ptable->totalElmts--;
        if((float)(ptable->totalElmts)*2 / ptable->dirsz < ptable->splitFactorMin)
            merge_bucket(ptable);
    }
}

///查找表中的值。送入一个查找结果存储变量bfound，当bfound==0时，表示没找到
void * hash_get(CDYNHASHTB table, unsigned int key, int * bfound)
{
    void * elmtPtr;
    if(findElmt((INTCDYNHASHTB *)table,key, &elmtPtr)){
        *bfound = -1;
        return elmtPtr;
    }
    else {
        *bfound = 0;
        return 0;
    }
}

//test whether table contains key. retrun 0 if key not found, -1 when found
int hash_contains(CDYNHASHTB table, unsigned int key)
{
    void* elmtPtr = 0;
    return findElmt((INTCDYNHASHTB *)table, key, &elmtPtr);
}
///供调试使用，在stdin里，输出哈希表
void hash_dump(CDYNHASHTB table)
{
    int i = 0, n = 0;
    CDNPAIR * pairCur = 0;
    INTCDYNHASHTB * ptable = (INTCDYNHASHTB *)table;
    printf ("\np = %d, level = %d, bucket size = %d, total items = %d"
            ,ptable->p
            ,ptable->level
            ,ptable->dirsz
            ,ptable->totalElmts
            );
    for (i=0;i<ptable->dirsz;i++)
    {
        if (!ptable->directory[i]->FirstPair)
            continue;
#ifdef DEBUG
        printf ("\nBucket #%d [size:%d]:",i,ptable->directory[i]->nItems);
#else
        printf ("\nBucket #%d:",i);
#endif
        pairCur =  ptable->directory[i]->FirstPair;
        n = 0;
        while (pairCur)
        {
            if (n==0)
                printf("\n\t");
            printf ("(%d:%d) ",pairCur->key,(unsigned int)pairCur->value);
            if (++n>=5)
                n = 0;
            pairCur = pairCur->next;
        }

    }
    printf ("\n");
}
///遍历
typedef struct tag_cdyn_iterator{
    INTCDYNHASHTB * ptable;
    int nCurrIdx;
    CDNPAIR * currItem;
} CDNIT;

///遍历表中的所有元素，开始一次遍历
CDYNHASHIt hash_iterate_begin(CDYNHASHTB table)
{
    CDNIT * pIt = (CDNIT *)malloc(sizeof(CDNIT));
    if (pIt)
    {
        pIt->ptable = (INTCDYNHASHTB * )table;
        pIt->nCurrIdx = 0;
        pIt->currItem = pIt->ptable->directory[0]->FirstPair;
        //find first element
        while (!pIt->currItem &&  pIt->nCurrIdx <pIt->ptable->dirsz )
        {
            pIt->nCurrIdx++;
            if (pIt->nCurrIdx<pIt->ptable->dirsz)
                pIt->currItem = pIt->ptable->directory[pIt->nCurrIdx]->FirstPair;
        }
    }
    return (CDYNHASHIt) pIt;
}
///重置迭代器
void hash_iterate_reset(CDYNHASHIt iter)
{
    CDNIT * pIt = (CDNIT *)iter;
    pIt->nCurrIdx = 0;
    pIt->currItem = pIt->ptable->directory[0]->FirstPair;
    //find first element
    while (!pIt->currItem &&  pIt->nCurrIdx <pIt->ptable->dirsz )
    {
        pIt->nCurrIdx++;
        if (pIt->nCurrIdx<pIt->ptable->dirsz)
            pIt->currItem = pIt->ptable->directory[pIt->nCurrIdx]->FirstPair;
    }
}
///遍历表中的所有元素，继续遍历,运行后，返回值为非0表示成功取得了一组数据，为0表示到头了。
int hash_iterate_next(CDYNHASHIt iter, unsigned int * pkey, void ** pvalue)
{
    int res = 0;
    CDNIT * pIt = (CDNIT *)iter;
    if (pIt->currItem)
    {
        *pkey = pIt->currItem->key;
        *pvalue = pIt->currItem->value;
        res = -1;
        //step
        pIt->currItem = pIt->currItem->next;
        if (!pIt->currItem)
        {
            pIt->nCurrIdx++;
            //find next element
            while (pIt->nCurrIdx < pIt->ptable->dirsz)
            {
                if (pIt->ptable->directory[pIt->nCurrIdx]->FirstPair)
                {
                    pIt->currItem = pIt->ptable->directory[pIt->nCurrIdx]->FirstPair;
                    break;
                }
                pIt->nCurrIdx++;
            }
        }
    }

    return res;
}

///遍历表中的所有元素，关闭遍历，并释放内存
void hash_iterate_end(CDYNHASHIt iter)
{
    CDNIT * pIt = (CDNIT *)iter;
    free (pIt);
}
///删除表，清理所有内存
void hash_fini(CDYNHASHTB table)
{
    INTCDYNHASHTB * ptable = (INTCDYNHASHTB *)table;
    int i = 0;
    for (i=0;i<ptable->dirsz;i++)
        bucket_free(ptable->directory[i]);
    free(ptable->directory);
    free (ptable);
}
}
