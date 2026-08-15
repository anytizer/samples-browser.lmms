#include "PCH.h"

#include "Configurations.h"

#include "WavInfo.h"
#include "WaveformDelegate.h"
#include "PopulateWavTable.h"
#include "ParseWavFile.h"

void processGUI(QTableWidget& table, QString samplesPath, int maxSamples)
{
    // QDir samplesDir(samplesPath);
    // //QFileInfo samplesDir(samplesPath);
    // if (!samplesDir.exists() || !samplesDir.isDir()) {
    //     qDebug() << "It exists and it is a directory.";
    //     return;
    // }

    table.setColumnCount(7); // 0 - 6

    table.setHorizontalHeaderLabels({
        "Sample File Name",
        "File Size",
        "Sample Rate",
        "Channels",
        "Bits/Sample",
        "Playtime (s)",
        "WAV Graph"
    });

    // Read-only.
    table.setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Select entire row.
    table.setSelectionBehavior(QAbstractItemView::SelectRows);

    // Only one row at a time.
    table.setSelectionMode(QAbstractItemView::SingleSelection);

    // Remove numeric row header.
    table.verticalHeader()->setVisible(false);

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

    table.verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table.verticalHeader()->setDefaultSectionSize(50);

    // -----------------------------------------------------
    // Column sizing
    // -----------------------------------------------------

    // Filename.
    table.setColumnWidth(0, 250);

    // Playtime.
    // table.setColumnWidth(5, 150);

    table.horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    table.horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table.horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table.horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table.horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    table.horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    // WAV Graph uses remaining table width.
    table.horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    // -----------------------------------------------------
    // Waveform delegate
    // -----------------------------------------------------

    table.setItemDelegateForColumn(6, new WaveformDelegate(&table));

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
        
        QFile sample(fileName);

        // do not process large samples
        if(sample.size()>=102400) continue;

        QString fullPath = samplesDir.absoluteFilePath(fileName);

        WavInfo info = parseWavFile(fullPath);

        if (info.sampleRate > 0 &&
            info.channels > 0 &&
            // info.channels <= 2 &&
            info.bitsPerSample > 0 &&
            info.dataSize > 0 &&
            // info.fileSize <= 102400 &&
            info.durationSeconds <= 10.0f
        ) // 100kb
        {
            wavList.append(info);
        }

        if(++counter > maxSamples) break;
    }


    // -----------------------------------------------------
    // Determine common waveform duration.
    //
    // Minimum = 1 second.
    // Otherwise = longest sample.
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
        &QTableWidget::cellClicked, // cellDoubleClicked | cellClicked
        [&table, samplesPath](
            int row,
            int column) {

            if(column!=6) return; // wav graph

            QTableWidgetItem *item = table.item(row, 0);
            if (!item) return;
            
            QString wavfile = QDir::cleanPath(QString("%1/%2").arg(samplesPath).arg(item->text()));
            QFile wf(wavfile);
            if(wf.exists())
            {
                /**
                 * Safe even when file missing!
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
}
