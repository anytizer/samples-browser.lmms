/*
 * Scanner.h - header for Scanner
 */
#pragma once

#include "ScannerView.h"
#include "ToolPlugin.h"

namespace lmms
{
    using lmms::ToolPlugin;
    using lmms::gui::PluginView;

    class Scanner : public ToolPlugin
    {
        private:
            Q_OBJECT

        public:
            Scanner();
            QString nodeName() const override;
            void saveSettings(QDomDocument&, QDomElement&) override {}
	        void loadSettings(const QDomElement&) override {}

            gui::PluginView* instantiateView(QWidget*) override { return new gui::ScannerView(this); }
    };
} // namespace lmms
