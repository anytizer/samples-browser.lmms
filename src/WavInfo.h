#pragma once

// ---------------------------------------------------------
// WAV metadata
// ---------------------------------------------------------
struct WavInfo {
    QString fileName;
    QString filePath;

    qint64 fileSize = 0;

    quint32 sampleRate = 0;
    quint16 channels = 0;
    quint16 bitsPerSample = 0;

    quint32 dataSize = 0;
    qint64 dataOffset = 0;

    double durationSeconds = 0.0;
};