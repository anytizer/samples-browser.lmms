#pragma once

#include "PCH.h"

#include "FormatFileSize.h"
#include "CreateWaveformImage.h"

// ---------------------------------------------------------
// Populate table
// ---------------------------------------------------------
void populateWavTable(QTableWidget &table, const QList<WavInfo> &wavList, double commonDurationSeconds)
{
    table.setRowCount(
        wavList.size());

    for (int i = 0; i < wavList.size(); ++i) {

        const WavInfo &wav = wavList[i];

        // File Name
        table.setItem(
            i,
            0,
            new QTableWidgetItem(
                wav.fileName));

        // File Size
        table.setItem(
            i,
            1,
            new QTableWidgetItem(
                formatFileSize(
                    wav.fileSize)));

        // Sample Rate
        table.setItem(
            i,
            2,
            new QTableWidgetItem(
                QString::number(
                    wav.sampleRate)));

        // Channels
        table.setItem(
            i,
            3,
            new QTableWidgetItem(
                QString::number(
                    wav.channels)));

        // Bits/Sample
        table.setItem(
            i,
            4,
            new QTableWidgetItem(
                QString::number(
                    wav.bitsPerSample)));

        // Playtime
        table.setItem(
            i,
            5,
            new QTableWidgetItem(
                QString::number(
                    wav.durationSeconds,
                    'f',
                    2)));

        // WAV Graph
        QImage waveform =
            createWaveformImage(
                wav,
                commonDurationSeconds,
                200,
                50);

        QTableWidgetItem *graphItem =
            new QTableWidgetItem();

        graphItem->setData(
            Qt::DecorationRole,
            QPixmap::fromImage(
                waveform));

        table.setItem(
            i,
            6,
            graphItem);
    }
}