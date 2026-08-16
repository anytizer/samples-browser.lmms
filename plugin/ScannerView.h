#pragma once

#include "ScannerView.h"
#include "ToolPluginView.h"

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
	
		public:
			ScannerView(Scanner* plugin);
		
			void closeEvent(QCloseEvent*) override;
			void keyPressEvent(QKeyEvent*) override;
	};
}