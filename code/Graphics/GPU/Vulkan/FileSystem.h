#pragma once

#include <vector>

namespace GPUAPI {

    class FileSystem {
    public:
        std::vector<char> readBytes(const char* path) const;
    };
}
