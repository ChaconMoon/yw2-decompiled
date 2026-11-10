/**
 * @file fs.h
 * @brief Nintendo CTR-SDK File System Library (`nn::fs`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"
#include "nn/Handle.h"
#include "nn/Result.h"

#ifdef __cplusplus
#include "nn/fs/fs_Api.h"
#include "nn/fs/detail/fs_FileSystemBase.h"
#include "nn/fs/ipc/fs_FileSystem.h"
#include "nn/fs/CTR/MPCore/detail/fs_UserFileSystem.h"

namespace nn {
namespace fs {

    enum Position {
        POSITION_BEGIN   = 0,
        POSITION_CURRENT = 1,
        POSITION_END     = 2
    };

    enum OpenFlag {
        OPEN_READ   = (1 << 0),
        OPEN_WRITE  = (1 << 1),
        OPEN_CREATE = (1 << 2)
    };

    enum ArchiveId {
        ARCHIVE_ROM           = 0x00000003,
        ARCHIVE_SAVEDATA      = 0x00000004,
        ARCHIVE_SDCARD        = 0x00000009,
        ARCHIVE_SAVEDATA_EXT  = 0x0000000A,
        ARCHIVE_SHARED_EXT    = 0x0000000E
    };

    class File {
    private:
        Handle m_Handle;

    public:
        File() : m_Handle(0) {}
        ~File();

        Result Open(const char* path, u32 openFlag);
        Result Close();
        Result Read(s64* pOutBytesRead, void* buffer, s64 bytesToRead);
        Result Write(s64* pOutBytesWritten, const void* buffer, s64 bytesToWrite);
        Result SetPosition(s64 position, Position origin);
        Result GetPosition(s64* pOutPosition) const;
        Result GetSize(s64* pOutSize) const;
        Result SetSize(s64 size);

        Handle GetHandle() const { return m_Handle; }
    };

    class Directory {
    private:
        Handle m_Handle;

    public:
        Directory() : m_Handle(0) {}
        ~Directory();

        Result Open(const char* path);
        Result Close();
        Result Read(u32* pOutCount, void* entryBuffer, u32 entryCount);

        Handle GetHandle() const { return m_Handle; }
    };

    /* High-level File System API */
    Result OpenFile(File* file, const char* path, u32 openFlag);
    Result CreateFile(const char* path, s64 initialSize);
    Result DeleteFile(const char* path);
    Result RenameFile(const char* oldPath, const char* newPath);
    Result CreateDirectory(const char* path);
    Result DeleteDirectory(const char* path);

    Result MountRom(const char* mountName);
    Result MountSaveData(const char* mountName);
    Result MountSdCard(const char* mountName);
    Result Unmount(const char* mountName);

    void   Finalize();

} // namespace fs
} // namespace nn

#else /* !__cplusplus (Pure C) */

typedef struct nn_fs_File {
    u32 m_Handle;
} nn_fs_File;

typedef struct nn_fs_Directory {
    u32 m_Handle;
} nn_fs_Directory;

s32  nn_fs_Initialize(void);
void nn_fs_Finalize(void);
s32  nn_fs_MountRom(const char* mountName);
s32  nn_fs_MountSaveData(const char* mountName);
s32  nn_fs_MountSdCard(const char* mountName);
s32  nn_fs_Unmount(const char* mountName);

#endif /* __cplusplus */
