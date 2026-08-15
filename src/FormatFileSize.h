#pragma once

#include "PCH.h"

// ---------------------------------------------------------
// Format file size
// ---------------------------------------------------------
QString formatFileSize(qint64 bytes)
{
    if (bytes < 1024)
        return QString("%1 B").arg(bytes);

    if (bytes < 1024 * 1024) {
        return QString("%1 KB")
            .arg(
                static_cast<double>(bytes) /
                1024.0,
                0,
                'f',
                1);
    }

    if (bytes <
        1024LL * 1024LL * 1024LL) {
        return QString("%1 MB")
            .arg(
                static_cast<double>(bytes) /
                (1024.0 * 1024.0),
                0,
                'f',
                1);
    }

    return QString("%1 GB")
        .arg(
            static_cast<double>(bytes) /
            (1024.0 * 1024.0 * 1024.0),
            0,
            'f',
            2);
}