#include <QApplication>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QFile>
#include <QDataStream>
#include <QFileInfo>
#include <QDir>
#include <QVBoxLayout>
#include <QWidget>
#include <QList>
#include <QStringList>
#include <QHeaderView>
#include <QMessageBox>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QVariant>
#include <QPalette>
#include <QColor>
#include <QSizePolicy>
#include <QtMath>
#include <QSound>
#include <QSoundEffect>
#include <QUrl>
#include <QKeyEvent>
#include <QDebug>

/**
 * Which directory to scan for?
 * ./samples
 * ../samples
 * /PATH/samples
 */
#define SCANNER_SAMPLES_DIRECTORY "../samples"

/**
 * Set according to your computer's capacities
 */
#define SCANNER_SAMPLES_LIMIT 50

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

            info.durationSeconds =
                static_cast<double>(
                    info.dataSize) /
                byteRate;
        }
    }

    return info;
}

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

        painter.drawLine(
            x,
            y1,
            x,
            y2);
    }

    return image;
}

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

// ---------------------------------------------------------
// Waveform delegate
//
// The waveform is LEFT aligned inside the graph cell.
// ---------------------------------------------------------
class WaveformDelegate : public QStyledItemDelegate
{
public:

    explicit WaveformDelegate(
        QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const override
    {
        QVariant value =
            index.data(Qt::DecorationRole);

        if (!value.isValid()) {
            QStyledItemDelegate::paint(
                painter,
                option,
                index);
            return;
        }

        QPixmap pixmap =
            qvariant_cast<QPixmap>(value);

        if (pixmap.isNull()) {
            QStyledItemDelegate::paint(
                painter,
                option,
                index);
            return;
        }

        // Draw selection background.
        if (option.state &
            QStyle::State_Selected) {

            painter->fillRect(
                option.rect,
                option.palette.highlight());
        }

        QRect available =
            option.rect.adjusted(
                2,
                2,
                -2,
                -2);

        // Fit image to available height/width.
        QPixmap scaled =
            pixmap.scaled(
                available.size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);

        // LEFT ALIGN.
        const int x =
            available.left();

        // Vertically center.
        const int y =
            available.top() +
            (available.height() -
             scaled.height()) / 2;

        painter->drawPixmap(
            x,
            y,
            scaled);
    }
};

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
            info.channels <= 2 &&
            info.bitsPerSample > 0 &&
            info.dataSize > 0 &&
            // info.fileSize <= 102400 &&
            info.durationSeconds <= 10.0f &&
            counter <= maxSamples
        ) // 100kb
        {
            wavList.append(info);
            ++counter;
        }
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



class KeysFilter : public QObject {
private:
    QTableWidget* m_table;
    //QWidget* m_window;

public:
    explicit KeysFilter(QTableWidget *table) 
        : QObject(table),
        m_table(table)//,
        //m_window(window)
        {}

protected:
    // This function automatically intercepts events
    bool eventFilter(QObject *obj, QEvent *event) override {
        // 1. Check if the event is a key press
        if (event->type() == QEvent::KeyPress) {
            // 2. Convert the generic event into a key event
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            
            // 3. Check if the pressed key is exactly F5
            if (keyEvent->key() == Qt::Key_F5) {
                m_table->clearContents();
                m_table->setRowCount(0);
                
                // @todo treat the command line parameters first
                processGUI(*m_table, SCANNER_SAMPLES_DIRECTORY, SCANNER_SAMPLES_LIMIT);

                qDebug() << "The F5 key was pressed.";
                
                // Return true to stop the event from doing anything else
                return true; 
            }

            if (keyEvent->key() == Qt::Key_Escape) {
                // in main window, exit.
                // if called under LMMS, hide
                qDebug() << "The ESC key was pressed.";
            }
        }
        // Pass all other events (like mouse movement) back to the window
        return QObject::eventFilter(obj, event);
    }
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    QString samplesDirectory = SCANNER_SAMPLES_DIRECTORY;
    QStringList args = QCoreApplication::arguments();
    if (args.size() > 1) {
        samplesDirectory = args.at(1);
    }



    // -----------------------------------------------------
    // Dark application palette
    // -----------------------------------------------------
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor("#202020"));
    darkPalette.setColor(QPalette::WindowText, QColor("#E0E0E0"));
    darkPalette.setColor(QPalette::Base, QColor("#181818"));
    darkPalette.setColor(QPalette::AlternateBase, QColor("#222222"));
    darkPalette.setColor(QPalette::ToolTipBase, QColor("#303030"));
    darkPalette.setColor(QPalette::ToolTipText, QColor("#FFFFFF"));
    darkPalette.setColor(QPalette::Text, QColor("#E0E0E0"));
    darkPalette.setColor(QPalette::Button, QColor("#303030"));
    darkPalette.setColor(QPalette::ButtonText, QColor("#E0E0E0"));
    darkPalette.setColor(QPalette::BrightText, QColor("#db0589"));
    darkPalette.setColor(QPalette::Highlight, QColor("#264F78"));
    darkPalette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    
    app.setPalette(darkPalette);

    QWidget window;
    window.setWindowTitle("WAV Samples Browser and Preview");
    
    QVBoxLayout layout(&window);
    
    QTableWidget table;
    
    // populates max top 50 samples in table | no trailing /
    processGUI(table, samplesDirectory, SCANNER_SAMPLES_LIMIT);
    layout.addWidget(&table, 1);

    KeysFilter *filter = new KeysFilter(&table);
    window.installEventFilter(filter);
    
    // QKeyEvent fakeF5Press(
    //     QEvent::KeyPress,  // The type of event
    //     Qt::Key_F5,        // The key code for F5
    //     Qt::NoModifier     // No Shift, Ctrl, or Alt held down
    // );
    // QCoreApplication::sendEvent(filter, &fakeF5Press); // processGUI() call simulation

    window.resize(900, 600);
    window.setFixedSize(900, 600);
    window.show();    

    return app.exec();
}