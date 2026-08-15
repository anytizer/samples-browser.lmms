#pragma once

#include "PCH.h"

// ---------------------------------------------------------
// Parse WAV header
// ---------------------------------------------------------
WavInfo parseWavFile(const QString &filePath)
{
    WavInfo info;

    QFileInfo fileInfo(filePath);

    info.fileName = fileInfo.fileName();
    info.filePath = filePath;
    info.fileSize = fileInfo.size();

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        return info;

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);

    char chunk[4];

    // RIFF
    if (file.read(chunk, 4) != 4 ||
        QByteArray(chunk, 4) != "RIFF") {
        return info;
    }

    quint32 chunkSize;
    in >> chunkSize;
    Q_UNUSED(chunkSize);

    // WAVE
    if (file.read(chunk, 4) != 4 ||
        QByteArray(chunk, 4) != "WAVE") {
        return info;
    }

    // Parse WAV chunks
    while (!file.atEnd()) {

        if (file.read(chunk, 4) != 4)
            break;

        quint32 subChunkSize;
        in >> subChunkSize;

        QByteArray chunkId(chunk, 4);

        if (chunkId == "fmt ") {

            quint16 audioFormat;
            quint32 byteRate;
            quint16 blockAlign;

            in >> audioFormat
               >> info.channels
               >> info.sampleRate
               >> byteRate
               >> blockAlign
               >> info.bitsPerSample;

            Q_UNUSED(audioFormat);
            Q_UNUSED(byteRate);
            Q_UNUSED(blockAlign);

            if (subChunkSize > 16) {
                file.seek(
                    file.pos() +
                    (subChunkSize - 16));
            }
        }
        else if (chunkId == "data") {

            info.dataOffset = file.pos();
            info.dataSize = subChunkSize;

            break;
        }
        else {

            file.seek(
                file.pos() +
                subChunkSize);
        }
    }

    if (info.sampleRate > 0 &&
        info.channels > 0 &&
        info.bitsPerSample > 0) {

        quint32 byteRate =
            info.sampleRate *
            info.channels *
            (info.bitsPerSample / 8);

        if (byteRate > 0) {
            info.durationSeconds = static_cast<double>(info.dataSize) / byteRate;
        }
    }

    return info;
}