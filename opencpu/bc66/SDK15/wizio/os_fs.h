////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Georgi Angelov
//      Simple UART driver, Arduino style, used for Serial class
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef OS_FS_H_
#define OS_FS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include <reent.h>
#include "fs/lfs.h"

    typedef struct vfs_file_s
    {
        int fd; /* 1000 ... 1004 */
        unsigned int hash;
        lfs_file_t *file;
    } vfs_file_t;

#define FILES_FD_BASE 1000

#define MAX_OPEN_FILES 4

    int os_fs_mount(void);
    void os_fs_unmount(void);

    int vfs_open(const char *path, int flags, int mode);
    int vfs_close(int fd);
    size_t vfs_write(int fd, const char *buf, size_t size);
    size_t vfs_read(int fd, char *dst, size_t size);
    _off_t vfs_seek(int fd, _off_t where, int whence);

#ifdef __cplusplus
}
#endif
#endif