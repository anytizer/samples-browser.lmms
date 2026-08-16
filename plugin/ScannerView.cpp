#include <QLayout>

#include "Scanner.h"
#include "ScannerView.h"

namespace lmms::gui
{
	ScannerView::ScannerView(Scanner* plugin)
	: ToolPluginView(plugin)
	, m_plugin(plugin)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		
		QWidget *table = new QWidget();
		table->setFixedSize(QSize(400, 300));

		mainLayout->addWidget(table);
		this->setLayout(mainLayout);




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