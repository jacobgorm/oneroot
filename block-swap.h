typedef struct BlockDriverState {
    void *opaque;
    uint64_t total_sectors;
} BlockDriverState;

typedef void BlockDriverCompletionFunc(void *opaque, int ret);

typedef struct BlockDriverAIOCB {
    BlockDriverCompletionFunc *cb;
    void *opaque;
} BlockDriverAIOCB;


BlockDriverAIOCB *swap_aio_write(BlockDriverState *bs, int64_t sector_num,
        const uint8_t *buf, int nb_sectors,
        BlockDriverCompletionFunc *cb, void *opaque);

BlockDriverAIOCB *swap_aio_read(BlockDriverState *bs,
        int64_t sector_num, uint8_t *buf, int nb_sectors,
        BlockDriverCompletionFunc *cb, void *opaque);

int swap_flush(BlockDriverState *bs);
void swap_close(BlockDriverState *bs);
int swap_create(const char *filename, int64_t size, int flags);
int swap_open(BlockDriverState *bs, const char *filename, int flags);
