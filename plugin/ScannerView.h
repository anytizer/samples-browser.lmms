#pragma once

#include "ScannerView.h"
#include "ToolPluginView.h"

#include "logic/CustomTableWidget.h"

namespace lmms
{
	class Scanner;
}

namespace lmms::gui
{
	class ScannerView : public ToolPluginView
	{
		private:
			Q_OBJECT
			
			Scanner* m_plugin;
			QString m_samplesDirectory;
			CustomTableWidget *m_table;

			void process();
	
		public:
			ScannerView(Scanner* plugin);
		
			void closeEvent(QCloseEvent*) override;
			void keyPressEvent(QKeyEvent*) override;

			void ask(); // calls ==> process();
			void callback(QString sample);
	};
}