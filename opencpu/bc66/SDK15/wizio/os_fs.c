#include "os_wizio.h"
#include "os_fs.h"

#ifdef USE_FS

#define BLOCK_SIZE 0x1000L /* must be 4096 */

/* over epo */
#define EPO_SIZE 0x10000L         /* must be 65536 */
uint8_t *s_epo_mem = 0x083F0000L; /* must be */

/* over fota */
//  #define OTA_SIZE 0xBF000        /* must be 782336 */
//  uint8_t *s_ota_mem = 0x82E6000; /* must  3A4E00*/

#define CACHE_SIZE 256 /* ??? multiple of block size */

static int epo_provided_block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    memcpy(buffer, s_epo_mem + c->block_size * block + off, size);
    return 0;
}

static int epo_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    uint32_t A = s_epo_mem + block * c->block_size + off;
    if (A < (uint32_t)s_epo_mem || A >= (uint32_t)s_epo_mem + EPO_SIZE || size > BLOCK_SIZE)
    {
        printf("ERROR WRITE LIMITS\n");
        return -1;
    }
    A -= 0x8000000;
    int res = API->hal_flash_write(A, buffer, size);
    return res;
}

static int epo_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block)
{
    uint32_t A = s_epo_mem + block * c->block_size;
    if (A < (uint32_t)s_epo_mem || A >= (uint32_t)s_epo_mem + EPO_SIZE)
    {
        printf("ERROR ERASE LIMIT\n");
        return -1;
    }
    A -= 0x8000000;
    int res = API->hal_flash_erase(A, 0 /*4k*/);
    return res;
}

int epo_provided_block_device_sync(const struct lfs_config *c) { return 0; }

lfs_t epo_lfs; // variables used by the filesystem

// configuration of the filesystem is provided by this struct
static const struct lfs_config epo_cfg =
    {
        .read = epo_provided_block_device_read,
        .prog = epo_provided_block_device_prog,
        .erase = epo_provided_block_device_erase,
        .sync = epo_provided_block_device_sync,
        .read_size = CACHE_SIZE,
        .prog_size = CACHE_SIZE,
        .block_size = BLOCK_SIZE,             // must be 4096
        .block_count = EPO_SIZE / BLOCK_SIZE, // for EPO = 16
        .cache_size = CACHE_SIZE,
        .lookahead_size = CACHE_SIZE,

        .block_cycles = 500,
};

int os_fs_mount(void)
{
    int err = lfs_mount(&epo_lfs, &epo_cfg);
    if (err)
    {
        err = lfs_format(&epo_lfs, &epo_cfg);
        if (err)
        {
            printf("[ERROR] lfs_format = %d\n", err);
            return err;
        }
        err = lfs_mount(&epo_lfs, &epo_cfg);
        if (err)
        {
            printf("[ERROR] lfs_mount(2) = %d\n", err);
            return err;
        }
    }
    return err;
}

void os_fs_unmount(void) { lfs_unmount(&epo_lfs); }

vfs_file_t vfs_files[MAX_OPEN_FILES] = {0};

static bool vfs_is_open(const char *path)
{
    int h = HASH(path);
    for (int i = 0; i < MAX_OPEN_FILES; i++)
    {
        if (vfs_files[i].fd && vfs_files[i].file && vfs_files[i].hash && h == vfs_files[i].hash)
            return true;
    }
    return false;
}

static bool vfs_is_file(int fd)
{
    return (fd >= FILES_FD_BASE && fd < FILES_FD_BASE + MAX_OPEN_FILES);
}

static int vfs_get_index(unsigned int fd)
{
    if (vfs_is_file(fd))
        for (int i = 0; i < MAX_OPEN_FILES; i++)
            if (vfs_files[i].fd == fd)
                return i;
    return -1;
}

static vfs_file_t *vfs_get_context(int fd)
{
    int i = vfs_get_index(fd);
    if (-1 == i)
        return 0;
    return &vfs_files[i];
}

static int vfs_get_free_index(void)
{
    for (int i = 0; i < MAX_OPEN_FILES; i++)
        if (0 == vfs_files[i].fd && 0 == vfs_files[i].hash)
            return i;
    return -1;
}

int vfs_open(const char *path, int flags, int mode)
{
    (void)mode;
    int res, i = -1;
    if (vfs_is_open(path))
    {
        printf("[ERROR] vfs_open IS OPEN\n");
        return -1;
    }

    if (-1 == (i = vfs_get_free_index()))
    {
        printf("[ERROR] vfs_open NO FREE\n");
        return -1;
    }

    vfs_files[i].file = calloc(1, sizeof(vfs_file_t));
    if (NULL == vfs_files[i].file)
    {
        printf("[ERROR] vfs_open ALLOC\n");
        return -1;
    }

    if ((res = lfs_file_open(&epo_lfs, vfs_files[i].file, path, flags)))
    {
        printf("[ERROR] vfs_open OPEN\n");
        free(vfs_files[i].file);
        vfs_files[i].file = NULL;
        return -1;
    }
    vfs_files[i].fd = i + FILES_FD_BASE;
    vfs_files[i].hash = os_hash(path);
    return vfs_files[i].fd;
}

int vfs_close(int fd)
{
    vfs_file_t *p = vfs_get_context(fd);
    if (NULL == p)
    {
        printf("[ERROR] vfs_close FIND\n");
        return -1;
    }
    int res = lfs_file_close(&epo_lfs, p->file);
    free(p->file);
    memset(p, 0, sizeof(vfs_file_t));
    return res;
}

size_t vfs_write(int fd, const char *buf, size_t size)
{
    int res = 0;
    vfs_file_t *p = vfs_get_context(fd);
    if (p && buf && size && vfs_is_file(p->fd))
        return lfs_file_write(&epo_lfs, p->file, buf, size);
    return res;
}

size_t vfs_read(int fd, char *buf, size_t size)
{
    int res = 0;
    vfs_file_t *p = vfs_get_context(fd);
    if (p && buf && size && vfs_is_file(p->fd))
        return lfs_file_read(&epo_lfs, p->file, buf, size);
    return res;
}

_off_t vfs_seek(int fd, _off_t where, int whence)
{
    vfs_file_t *p = vfs_get_context(fd);
    if (p && vfs_is_file(p->fd))
        return lfs_file_seek(&epo_lfs, p->file, where, whence);
    return -1;
}

#endif