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
	{
		QString samplesDirectory  = "~/Desktop/samples/drums";
		
		
		
		
		
		this->setAcceptDrops(true);
		this->setMinimumSize(900, 600);
		
		CustomTableWidget *table = new CustomTableWidget(0, 0, this);
		processGUI(*table, samplesDirectory);

		KeysFilter *filter = new KeysFilter(table);
		this->installEventFilter(filter); 

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		QVBoxLayout* tl = new QVBoxLayout(); // Parent can be set via layout nesting or constructor
		tl->addWidget(table, 1);
		mainLayout->addLayout(tl);

		
		
		
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
	void ScannerView::keyPressEvent(QKeyEvent*) {}
} // namespace