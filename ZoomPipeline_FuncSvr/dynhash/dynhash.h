#if !defined (hash_V100_H_INCLUDE)
#define hash_V100_H_INCLUDE

/*!
    this structure hold an linear dynamic hash table.
    the key type is an unsigned integer, value type is an void *
    hash table can hold variant value types , using void *.
*/
namespace ZPHashTable{
typedef void * CDYNHASHTB;
typedef void * CDYNHASHIt;
/*!
   新建一个哈希表 spFacMax 是触发分裂的门限(可以取>=2), spFacMin是触发合并的门限可以取>=1。
   返回一个句柄，所有的操作均针对这个句柄操作。
*/
CDYNHASHTB hash_init(float spFacMax, float spFacMin);

///插入或者修改一个值，
void hash_set(CDYNHASHTB table, unsigned int key, void * value);

///删除 key
void hash_del(CDYNHASHTB table, unsigned int key);

///查找表中的值。送入一个查找结果存储变量bfound，当bfound==0时，表示没找到
void * hash_get(CDYNHASHTB table, unsigned int key, int * bfound);

///看看表中是否含有键 key, 返回-1表示含有，0表示木有
int hash_contains(CDYNHASHTB table, unsigned int key);

///供调试使用，在stdin里，输出哈希表
void hash_dump(CDYNHASHTB table);

///遍历表中的所有元素，开始一次遍历
CDYNHASHIt hash_iterate_begin(CDYNHASHTB table);

///重置迭代器
void hash_iterate_reset(CDYNHASHIt iter);

///遍历表中的所有元素，继续遍历,运行后，返回值为非0表示成功取得了一组数据，为0表示到头了。
int hash_iterate_next(CDYNHASHIt iter, unsigned int * pkey, void ** pvalue);

///遍历表中的所有元素，关闭遍历，并释放内存
void hash_iterate_end(CDYNHASHIt iter);

///删除表，清理所有内存
void hash_fini(CDYNHASHTB table);
}
#endif
