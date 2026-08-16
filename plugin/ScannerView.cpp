#include <QLayout>
#include <QDomElement>

#include "Scanner.h"
#include "ScannerView.h"

#include "logic/PCH.h"
#include "logic/CustomTableWidget.h"
#include "logic/ProcessGUI.h"
#include "logic/KeysFilter.h"

#include "Song.h"
#include "Track.h"
#include "SampleTrack.h"
#include "PatternStore.h"
#include "InstrumentTrack.h"
#include "Instrument.h"

#include "SamplePlayHandle.h"

namespace lmms::gui
{
	ScannerView::ScannerView(Scanner* plugin)
	: ToolPluginView(plugin)
	, m_plugin(plugin)
	, m_samplesDirectory("./samples/")
	, m_table(new CustomTableWidget(0, 0, this))
	{
		this->setAcceptDrops(true);
		this->setMinimumSize(900, 600);
		

		// KeysFilter *filter = new KeysFilter(m_table);
		// this->installEventFilter(filter);

		// on opening LMMS and first time window activation
		if(m_samplesDirectory=="")
		{
			ask();
		}			
		else
		{
			process();
			m_samplesDirectory = "";
		}

		QWidget* pw = parentWidget();
		if (pw!=nullptr)
		{
			pw->hide(); // default hidden

			Qt::WindowFlags flags = pw->windowFlags();
			//flags |= Qt::MSWindowsFixedSizeDialogHint;
			//flags |= Qt::WindowMaximizeButtonHint;
			flags |= Qt::WindowStaysOnTopHint;
			pw->setWindowFlags(flags);

			pw->adjustSize();
		}
	}

	void ScannerView::closeEvent(QCloseEvent*) {}
	void ScannerView::keyPressEvent(QKeyEvent* event) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_F5) {
				ask();
				qDebug() << "F5 key caught by event filter on Scanner!";
				return;// true; // Handled/Intercepted
			}
		}
		//return QObject::eventFilter(watched, event);
	}

	void ScannerView::ask()
	{
		QFileDialog dialog(this);
		dialog.setFileMode(QFileDialog::Directory);
		dialog.setOption(QFileDialog::ShowDirsOnly, true);
		dialog.setWindowTitle(tr("Select One-Shots Samples Directory"));
		dialog.setDirectory(m_samplesDirectory); // QDir::homePath()

		if (dialog.exec() == QDialog::Accepted) {
			QStringList selectedDirs = dialog.selectedFiles();
			if (!selectedDirs.isEmpty()) {
				m_samplesDirectory = selectedDirs.first();
				// Do something with dirPath

				process();
			}
		}
	}


	void ScannerView::process()
	{
		this->m_table = new CustomTableWidget(0, 0, this);
		processGUI(*this->m_table, m_samplesDirectory, [this](QString sample) { this->callback1(sample); }, [this](QString sample) { this->callback2(sample); });

		delete this->layout();

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		QVBoxLayout* tl = new QVBoxLayout();
		tl->addWidget(m_table, 1);
		mainLayout->addLayout(tl);
		this->setLayout(new QVBoxLayout(this));
	}

	void ScannerView::callback1(QString sample)
	{
		QFile wf(sample);
		if(wf.exists())
		{
		    /**
		     * Safety even when file missing!
		     */
		    // QSoundEffect *m_sound = new QSoundEffect;
		    // m_sound->setSource(QUrl::fromLocalFile(sample));
		    // m_sound->setVolume(1.0f);
		    // m_sound->setLoopCount(1); // QSoundEffect::Infinite
		    // m_sound->play();

		    Engine::audioEngine()->addPlayHandle(new SamplePlayHandle(sample));
		}
	}

	void ScannerView::callback2(QString sample)
	{
		// qDebug() << "ScannerView::callback() called with parameter: " << sample;
		// qDebug() << "I will now add a beat pattern.";

		PatternStore* ps = Engine::patternStore();

		QString afp = "audiofileprocessor";
		InstrumentTrack* innerTrack = new InstrumentTrack(ps); // song, ps
		innerTrack->loadInstrument(afp);
		innerTrack->setName(QFileInfo(sample).baseName());

		QDomDocument preset;
		QDomElement element = preset.createElement(afp);
		element.setAttribute("src", sample);
		innerTrack->instrument()->restoreState(element);
	}
} // namespace