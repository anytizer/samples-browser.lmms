#include <QSoundEffect>
#include <QApplication>
#include <QCoreApplication>
#include <QPalette>

#include "CustomTableWidget.h"
#include "ProcessGUI.h"
#include "KeysFilter.h"

/**
 * Use this function from the Host DAW.
 */
bool isDir(QString directory)
{
    QFileInfo fileInfo(directory);
    return fileInfo.exists() && fileInfo.isDir();
}

void help()
{
    qDebug("Help:\n\n    scanner /path/to/wav/samples/\n");
}

/**
 * Sould preview callback
 */
void callback1(QString sample)
{
    /**
     * Safety even when file missing!
     */
    QFile wf(sample);
	if(wf.exists())
    {
        QSoundEffect *m_sound = new QSoundEffect;
        m_sound->setSource(QUrl::fromLocalFile(sample));
        m_sound->setVolume(1.0f);
        m_sound->setLoopCount(1); // QSoundEffect::Infinite
        m_sound->play();
    }
}

/**
 * Track creation callback
 */
void callback2(QString sample)
{
    // add track in LMMS
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString samplesDirectory = SCANNER_SAMPLES_DIRECTORY;
    QStringList args = QCoreApplication::arguments();
    if (args.size() > 1) {
        samplesDirectory = args.at(1);
    }

    if(!isDir(samplesDirectory))
    {
        help();
        return -1;
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
    
    CustomTableWidget table(0, 0, &window); // @see SCANNER_SAMPLES_COLUMNX
    processGUI(table, samplesDirectory, callback1, callback2); // callback 1/2: can be nullptr
    layout.addWidget(&table, 1);

    KeysFilter *filter = new KeysFilter(&table);
    window.installEventFilter(filter);    
    // QKeyEvent fakeF5Press(
    //     QEvent::KeyPress,  // The type of event
    //     Qt::Key_F5,        // The key code for F5
    //     Qt::NoModifier     // No Shift, Ctrl, or Alt held down
    // );
    // processGUI() call simulation
    // QCoreApplication::sendEvent(filter, &fakeF5Press);

    window.setFixedSize(900, 600);
    window.resize(900, 600);
    window.show();

    return app.exec();
}