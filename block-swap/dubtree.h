#ifndef __DUBTREE_H__
#define __DUBTREE_H__

#if defined (QEMU_UXEN) || defined(LIBIMG)
#include <dm/config.h>
#else
#include "config.h"
#endif

#include "dubtree_constants.h"
#include "hashtable.h"
#include "lrucache.h"

#define DUBTREE_MAX_FALLBACKS 8
#define DUBTREE_HASH_SIZE (256/8)

/* The per-instance in-memory representation of a dubtree. */

typedef struct {
    union {
        uint64_t first64;
        uint8_t full[DUBTREE_HASH_SIZE];
    } id;
} chunk_id_t;

static inline void clear_chunk_id(chunk_id_t *chunk_id)
{
    memset(chunk_id->id.full, 0, sizeof(chunk_id->id.full));
}

static inline int cmp_chunk_ids(const chunk_id_t *a, const chunk_id_t *b)
{
    return (memcmp(a->id.full, b->id.full, sizeof(a->id.full)) == 0);
}

static inline int valid_chunk_id(const chunk_id_t *chunk_id)
{
    chunk_id_t nil = {};
    return !cmp_chunk_ids(&nil, chunk_id);
}

typedef struct DubTreeLevels {
    volatile uint64_t version;
    chunk_id_t levels[DUBTREE_MAX_LEVELS];
} DubTreeLevels;

typedef struct DubTreeHeader {
    uint32_t magic, version;
    uint32_t dubtree_m;
    uint32_t dubtree_slot_size;
    uint32_t dubtree_max_levels;
    uint32_t dubtree_initialized;
    volatile uint64_t out_chunk;
    DubTreeLevels buffers[2];
} DubTreeHeader;

typedef void (*read_callback) (void *opaque, int result);
typedef void *(*malloc_callback) (void *opaque, size_t sz);
typedef void (*free_callback) (void *opaque, void *ptr);

typedef struct DubTree {
    critical_section write_lock;
    DubTreeHeader *header;
    const chunk_id_t *levels;

    char *fallbacks[DUBTREE_MAX_FALLBACKS + 1];
    critical_section cache_lock;
    HashTable ht;
    LruCache lru;
    int buffer_max;
    void *buffered;
    malloc_callback malloc_cb;
    free_callback free_cb;
    void *opaque;

} DubTree;

int dubtree_insert(DubTree *t, int numKeys, uint64_t* keys, uint8_t *values,
        uint32_t *sizes, int force_level);

void *dubtree_prepare_find(DubTree *t);
void dubtree_end_find(DubTree *t, void *ctx);

int dubtree_find(DubTree *t, uint64_t start, int num_keys,
        uint8_t *out, uint8_t *map, uint32_t *sizes,
        read_callback cb, void *opaque, void *ctx);

int dubtree_init(DubTree *t, char **fallbacks, malloc_callback malloc_cb,
    free_callback free_cb, void *opaque);
void dubtree_close(DubTree *t);
int dubtree_delete(DubTree *t);
void dubtree_quiesce(DubTree *t);
int dubtree_sanity_check(DubTree *t);

#endif /* __DUBTREE_H__ */
