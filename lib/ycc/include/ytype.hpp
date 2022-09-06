#pragma once

/// @brief cli flags/args type

enum class ytype {
    HFLAG, ///< -h flag
    IFLAG, ///< -i flag (required)
    OFLAG, ///< -o flag (required)
    IARGS, ///< -i args
    OARGS, ///< -o args
};
