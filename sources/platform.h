#pragma once

#include "streams.h"
#include "utils.h"

#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

struct statvfs;
struct timespec;

#ifdef _WIN32
typedef unsigned mode_t;
typedef long long ssize_t;

#include <io.h>

#define O_CREAT _O_CREAT
#define O_APPEND _O_APPEND
#define O_RDONLY _O_RDONLY
#define O_RDWR _O_RDWR
#define O_EXCL _O_EXCL

#define S_IFMT 00170000
#define S_IFSOCK 0140000
#define S_IFLNK 0120000
#define S_IFREG 0100000
#define S_IFBLK 0060000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFIFO 0010000
#define S_ISUID 0004000
#define S_ISGID 0002000
#define S_ISVTX 0001000

#endif

#ifdef _WIN32
typedef FUSE_STAT real_stat_type;
#else
typedef struct stat real_stat_type;
#endif

namespace securefs
{

class FileStream : public StreamBase
{
public:
    virtual int get_native_handle() noexcept = 0;
    virtual void fsync() = 0;
    virtual void utimens(const struct timespec ts[2]) = 0;
};

class FileSystemService
{
private:
    class Impl;
    std::unique_ptr<Impl> impl;

public:
    FileSystemService();
    FileSystemService(const std::string& path);
    ~FileSystemService();
    std::shared_ptr<FileStream> open_file_stream(const std::string& path, int flags, unsigned mode);
    bool remove_file(const std::string& path) noexcept;
    bool remove_directory(const std::string& path) noexcept;
    void rename(const std::string& a, const std::string& b);
    void lock();
    void ensure_directory(const std::string& path, unsigned mode);
    void statfs(struct statvfs*);

public:
    static uint32_t getuid() noexcept;
    static uint32_t getgid() noexcept;
    static bool raise_fd_limit() noexcept;

    static std::string temp_name(const std::string& prefix, const std::string& suffix)
    {
        return prefix + random_hex_string(16) + suffix;
    }
};
}
