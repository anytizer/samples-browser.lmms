#include "PCH.h"

#include "Configurations.h"

#include "WavInfo.h"
#include "WaveformDelegate.h"
#include "CustomTableWidget.h"
#include "PopulateWavTable.h"
#include "ParseWavFile.h"

void processGUI(CustomTableWidget& table, QString samplesPath)
{
    /**
     * DO NOT ALTER the total size: 7
     */
    QList<QString> labels = {
        "Sample File Name",
        "File Size",
        "Sample Rate",
        "Channels",
        "Bits/Sample",
        "Playtime (s)",
        "WAV Graph"
    };

    table.clearContents();
    table.setColumnCount(labels.size()); // 0 - 6 = 7
    table.setRowCount(0); // we will add rows later

    table.setHorizontalHeaderLabels(labels);

    // Read-only.
    table.setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Select entire row.
    table.setSelectionBehavior(QAbstractItemView::SelectRows);

    // Only one row at a time.
    table.setSelectionMode(QAbstractItemView::SingleSelection);

    // Fill parent.
    table.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // -----------------------------------------------------
    // Dark table appearance
    // -----------------------------------------------------

    table.setStyleSheet(
        "QTableWidget {"
        "    background-color: #181818;"
        "    alternate-background-color: #222222;"
        "    color: #E0E0E0;"
        "    gridline-color: #383838;"
        "    selection-background-color: #264F78;"
        "    selection-color: #FFFFFF;"
        "}"
        ""
        "QHeaderView::section {"
        "    background-color: #303030;"
        "    color: #E0E0E0;"
        "    border: 1px solid #404040;"
        "    padding: 4px;"
        "}"
        ""
        "QTableCornerButton::section {"
        "    background-color: #303030;"
        "    border: 1px solid #404040;"
        "}"
    );

    table.setAlternatingRowColors(true);

    // -----------------------------------------------------
    // Fixed row height: 50 pixels
    // -----------------------------------------------------
    QHeaderView *vh = table.verticalHeader();
    vh->setVisible(false); // Remove numeric row header.
    vh->setMinimumWidth(100);
    vh->setSectionResizeMode(QHeaderView::Fixed);
    vh->setDefaultSectionSize(50);

    // -----------------------------------------------------
    // Column sizing
    // -----------------------------------------------------

    // Filename.
    table.setColumnWidth(SCANNER_SAMPLES_COLUMN0, 250);

    // Playtime.
    // table.setColumnWidth(5, 150);

    QHeaderView *hh = table.horizontalHeader();
    hh->setMinimumSectionSize(50);
    hh->setSectionResizeMode(SCANNER_SAMPLES_COLUMN0, QHeaderView::Interactive);
    hh->setSectionResizeMode(SCANNER_SAMPLES_COLUMN1, QHeaderView::ResizeToContents);
    hh->setSectionResizeMode(SCANNER_SAMPLES_COLUMN2, QHeaderView::ResizeToContents);
    hh->setSectionResizeMode(SCANNER_SAMPLES_COLUMN3, QHeaderView::ResizeToContents);
    hh->setSectionResizeMode(SCANNER_SAMPLES_COLUMN4, QHeaderView::ResizeToContents);
    hh->setSectionResizeMode(SCANNER_SAMPLES_COLUMN5, QHeaderView::ResizeToContents);

    // WAV Graph uses remaining table width.
    hh->setSectionResizeMode(SCANNER_SAMPLES_COLUMN6, QHeaderView::Stretch);

    // -----------------------------------------------------
    // Waveform delegate
    // -----------------------------------------------------

    table.setItemDelegateForColumn(SCANNER_SAMPLES_COLUMN6, new WaveformDelegate(&table));

    // -----------------------------------------------------
    // Scan WAV files at runtime
    // -----------------------------------------------------

    QDir samplesDir(samplesPath);
    QStringList filePaths =
        samplesDir.entryList(
            QStringList()
                << "*.wav"
                << "*.WAV",
            QDir::Files |
            QDir::Readable,
            QDir::Name);

    QList<WavInfo> wavList;

    int counter = 0;
    for (const QString &fileName : filePaths) {
        QString fullPath = samplesDir.absoluteFilePath(fileName);
        
        QFileInfo sample(fullPath);
        if(sample.size()>=SCANNER_SAMPLES_FILESIZE_LIMIT) continue;
 
        WavInfo info = parseWavFile(fullPath);

        if (info.sampleRate > 0 &&
            info.channels > 0 &&
            info.bitsPerSample > 0 &&
            info.dataSize > 0 &&
            info.durationSeconds <= SCANNER_SAMPLES_MAX_PLAYTIME
        )
        {
            wavList.append(info);
        }

        if(++counter > SCANNER_SAMPLES_COUNT_LIMIT) break;

        /**
         * Total checks applied so far:
         * - file size limits - 100kb
         * - play time - 10s
         * - top/max count - 50
         * - .wav format files only
         */
    }


    // -----------------------------------------------------
    // Determine common waveform duration.
    //
    // Minimum = 1 second.
    // Otherwise = longest sample, ie. 10 seconds
    // -----------------------------------------------------

    double longestDuration = 0.0;
    for (const WavInfo &wav : wavList)
    {
        longestDuration = qMax(longestDuration, wav.durationSeconds);
    }

    const double commonDurationSeconds = qMax(1.0, longestDuration);

    // -----------------------------------------------------
    // Populate table
    // -----------------------------------------------------

    populateWavTable(table, wavList, commonDurationSeconds);

    // -----------------------------------------------------
    // Double-click handler
    // -----------------------------------------------------

    QObject::connect(
        &table,
        &QTableWidget::cellClicked, // Hand Pointer Icon visualized already
        [&table, samplesPath](
            int row,
            int column) {

            if(column!=SCANNER_SAMPLES_COLUMN6) return; // wav graph

            // row, 0 | first item, full wav filename
            QTableWidgetItem *item = table.item(row, SCANNER_SAMPLES_COLUMN0);
            if (!item) return;
            
            QString wavfile = QDir::cleanPath(QString("%1/%2").arg(samplesPath).arg(item->text()));
            QFile wf(wavfile);
            if(wf.exists())
            {
                /**
                 * Safety even when file missing!
                 */
                QSoundEffect *m_sound = new QSoundEffect;
                m_sound->setSource(QUrl::fromLocalFile(wavfile));
                m_sound->setVolume(1.0f);
                m_sound->setLoopCount(1); // QSoundEffect::Infinite
                m_sound->play();
            }

            // @todo On double click, send sample path to LMMS
        }
    );


    QObject::connect(
        &table,
        &QTableWidget::cellDoubleClicked,
        [&table, samplesPath](
            int row,
            int column) {

            // @todo: Sinlge Click is also entertained together.

            // row, 0 | first item, full wav filename
            QTableWidgetItem *item = table.item(row, SCANNER_SAMPLES_COLUMN0);
            if (!item) return;

            qDebug() << "Sending Item to LMMS:" << samplesPath << item->text();
        }
    );
}
