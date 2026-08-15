#pragma once

#include "PCH.h"

#include "WavInfo.h"
#include "WaveformDelegate.h"

#include "Configurations.h"



// ---------------------------------------------------------
// Create waveform image using a common time scale.
//
// All WAV images represent the same total duration:
//
//     max(1 second, longest WAV duration)
//
// The time axis is divided into 32 equal sections.
// A faded vertical line is drawn every 8 sections.
//
// The image background is transparent.
// ---------------------------------------------------------
QImage createWaveformImage(
    const WavInfo &wav,
    double commonDurationSeconds,
    int width = 200,
    int height = 100)
{
    width = qMin(width, 200);
    height = qMax(height, 50);

    QImage image(
        width,
        height,
        QImage::Format_ARGB32);

    // Transparent background.
    image.fill(Qt::transparent);

    if (commonDurationSeconds <= 0.0)
        return image;

    QPainter painter(&image);

    painter.setRenderHint(
        QPainter::Antialiasing,
        false);

    // -----------------------------------------------------
    // 32 time divisions.
    //
    // Faded lines at:
    //
    // 8/32
    // 16/32
    // 24/32
    // -----------------------------------------------------

    const int divisions = 32;

    painter.setPen(
        QPen(
            QColor(180, 180, 180, 45),
            1));

    for (int division = 8;
         division < divisions;
         division += 8) {

        int x =
            qRound(
                (static_cast<double>(division) /
                 divisions) *
                (width - 1));

        painter.drawLine(
            x,
            0,
            x,
            height - 1);
    }

    // -----------------------------------------------------
    // WAV validation
    // -----------------------------------------------------

    if (wav.dataOffset <= 0 ||
        wav.dataSize == 0 ||
        wav.channels == 0 ||
        wav.bitsPerSample == 0 ||
        wav.sampleRate == 0) {
        return image;
    }

    QFile file(wav.filePath);

    if (!file.open(QIODevice::ReadOnly))
        return image;

    if (!file.seek(wav.dataOffset))
        return image;

    const int bytesPerSample = wav.bitsPerSample / 8;
    const int bytesPerFrame = bytesPerSample * wav.channels;

    if (bytesPerSample <= 0 ||
        bytesPerFrame <= 0) {
        return image;
    }

    const qint64 totalFrames = wav.dataSize / bytesPerFrame;
    if (totalFrames <= 0)
        return image;

    // -----------------------------------------------------
    // Center line
    // -----------------------------------------------------

    const int centerY = height / 2;

    painter.setPen(QPen(QColor(100, 100, 100, 130), 1));
    painter.drawLine(0, centerY, width - 1, centerY);

    // -----------------------------------------------------
    // Waveform
    // -----------------------------------------------------

    painter.setPen(QPen(QColor("#4FC3F7"), 1));

    // Every image uses the SAME time-to-pixel mapping.
    for (int x = 0; x < width; ++x) {

        const double startTime =
            (static_cast<double>(x) /
             width) *
            commonDurationSeconds;

        const double endTime =
            (static_cast<double>(x + 1) /
             width) *
            commonDurationSeconds;

        // Nothing to draw after this WAV ends.
        if (startTime >= wav.durationSeconds)
            continue;

        const double clippedEndTime =
            qMin(
                endTime,
                wav.durationSeconds);

        qint64 startFrame =
            static_cast<qint64>(
                startTime *
                wav.sampleRate);

        qint64 endFrame =
            static_cast<qint64>(
                clippedEndTime *
                wav.sampleRate);

        if (startFrame < 0)
            startFrame = 0;

        if (endFrame > totalFrames)
            endFrame = totalFrames;

        if (endFrame <= startFrame)
            endFrame = startFrame + 1;

        if (endFrame > totalFrames)
            endFrame = totalFrames;

        float minValue = 1.0f;
        float maxValue = -1.0f;

        bool foundSample = false;

        for (qint64 frame = startFrame;
             frame < endFrame;
             ++frame) {

            qint64 sampleOffset =
                wav.dataOffset +
                frame * bytesPerFrame;

            if (!file.seek(sampleOffset))
                break;

            QByteArray sampleData =
                file.read(bytesPerFrame);

            if (sampleData.size() <
                bytesPerFrame) {
                break;
            }

            // First channel only.
            const char *p =
                sampleData.constData();

            float value = 0.0f;

            if (wav.bitsPerSample == 8) {

                quint8 sample =
                    static_cast<quint8>(
                        static_cast<unsigned char>(
                            p[0]));

                value =
                    (static_cast<float>(sample) -
                     128.0f) /
                    128.0f;
            }
            else if (wav.bitsPerSample == 16) {

                qint16 sample =
                    static_cast<qint16>(
                        static_cast<quint8>(p[0]) |
                        (static_cast<qint16>(
                             static_cast<quint8>(p[1]))
                         << 8));

                value =
                    static_cast<float>(sample) /
                    32768.0f;
            }
            else if (wav.bitsPerSample == 24) {

                qint32 sample =
                    static_cast<qint32>(
                        static_cast<quint8>(p[0]) |
                        (static_cast<qint32>(
                             static_cast<quint8>(p[1]))
                         << 8) |
                        (static_cast<qint32>(
                             static_cast<quint8>(p[2]))
                         << 16));

                if (sample & 0x800000)
                    sample |= 0xFF000000;

                value =
                    static_cast<float>(sample) /
                    8388608.0f;
            }
            else if (wav.bitsPerSample == 32) {

                qint32 sample =
                    static_cast<qint32>(
                        static_cast<quint8>(p[0]) |
                        (static_cast<qint32>(
                             static_cast<quint8>(p[1]))
                         << 8) |
                        (static_cast<qint32>(
                             static_cast<quint8>(p[2]))
                         << 16) |
                        (static_cast<qint32>(
                             static_cast<quint8>(p[3]))
                         << 24));

                value =
                    static_cast<float>(sample) /
                    2147483648.0f;
            }
            else {
                return image;
            }

            minValue =
                qMin(minValue, value);

            maxValue =
                qMax(maxValue, value);

            foundSample = true;
        }

        if (!foundSample)
            continue;

        int y1 =
            centerY -
            static_cast<int>(
                maxValue *
                (height / 2 - 2));

        int y2 =
            centerY -
            static_cast<int>(
                minValue *
                (height / 2 - 2));

        painter.drawLine(x, y1, x, y2);
    }

    return image;
}





