#include <QLayout>

#include "Scanner.h"
#include "ScannerView.h"

#include "logic/PCH.h"
#include "logic/CustomTableWidget.h"
#include "logic/ProcessGUI.h"
#include "logic/KeysFilter.h"

namespace lmms::gui
{
	ScannerView::ScannerView(Scanner* plugin)
	: ToolPluginView(plugin)
	, m_plugin(plugin)
	, m_samplesDirectory("")
	, m_table(new CustomTableWidget(0, 0, this))
	{
		this->setAcceptDrops(true);
		this->setMinimumSize(900, 600);
		
		// process();
		// KeysFilter *filter = new KeysFilter(m_table);
		// this->installEventFilter(filter);
		if(m_samplesDirectory=="")
			ask();
		
		
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
				qDebug() << "F5 key caught by event filter!";
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
		dialog.setWindowTitle(tr("Select Directory"));
		dialog.setDirectory(QDir::homePath());

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

		// clean layout, table...



		this->m_table = new CustomTableWidget(0, 0, this);
		processGUI(*this->m_table, m_samplesDirectory);

		//clearLayout(this->layout());
		delete this->layout();

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		QVBoxLayout* tl = new QVBoxLayout(); // Parent can be set via layout nesting or constructor
		tl->addWidget(m_table, 1);
		mainLayout->addLayout(tl);



		
		// 
		// this->setLayout(new QVBoxLayout(this));
	}
} // namespace