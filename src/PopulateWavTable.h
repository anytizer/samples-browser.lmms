#pragma once

#include "PCH.h"

#include "FormatFileSize.h"
#include "CreateWaveformImage.h"

// ---------------------------------------------------------
// Populate table
// ---------------------------------------------------------
void populateWavTable(QTableWidget &table, const QList<WavInfo> &wavList, double commonDurationSeconds)
{
    int elements = wavList.size();
    table.setRowCount(elements);

    for (int i = 0; i < elements; ++i) {

        const WavInfo &wav = wavList[i];

        // File Name
        table.setItem(i, SCANNER_SAMPLES_COLUMN0, new QTableWidgetItem(wav.fileName));

        // File Size
        table.setItem(i, SCANNER_SAMPLES_COLUMN1, new QTableWidgetItem(formatFileSize(wav.fileSize)));

        // Sample Rate
        table.setItem(i, SCANNER_SAMPLES_COLUMN2, new QTableWidgetItem(QString::number(wav.sampleRate)));

        // Channels
        table.setItem(i, SCANNER_SAMPLES_COLUMN3, new QTableWidgetItem(QString::number(wav.channels)));

        // Bits/Sample
        table.setItem(i, SCANNER_SAMPLES_COLUMN4, new QTableWidgetItem(QString::number(wav.bitsPerSample)));

        // Playtime
        table.setItem(i, SCANNER_SAMPLES_COLUMN5, new QTableWidgetItem(QString::number(wav.durationSeconds, 'f', 2)));

        // WAV Graph
        QImage waveform = createWaveformImage(wav, commonDurationSeconds);
        QTableWidgetItem *graphItem = new QTableWidgetItem();
        graphItem->setData(Qt::DecorationRole, QPixmap::fromImage(waveform));
        table.setItem(i, SCANNER_SAMPLES_COLUMN6, graphItem);
    }
}