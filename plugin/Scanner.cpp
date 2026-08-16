/*
 * Scanner.cpp - Scanner.
 *
 */
#include "Scanner.h"
#include "ScannerView.h"

#include "Song.h"
#include "embed.h"
#include "plugin_export.h"

namespace lmms
{
	extern "C"
	{
		Plugin::Descriptor PLUGIN_EXPORT scanner_plugin_descriptor =
		{
			LMMS_STRINGIFY(PLUGIN_NAME),
			"Samples Visualizer",
			QT_TRANSLATE_NOOP("PluginBrowser", "Fetch, Preview and Import .wav samples"),
			"@anytizer <anytizer@users.noreply.github.com>",
			0x0100,
			Plugin::Type::Tool,
			new PluginPixmapLoader("logo"),
			"wav",
			nullptr,
		};

		PLUGIN_EXPORT Plugin* lmms_plugin_main(Model*, void*)
		{
			return new Scanner();
		}
	}

	QString Scanner::nodeName() const
	{
		return scanner_plugin_descriptor.name;
	}

	Scanner::Scanner()
		: ToolPlugin(&scanner_plugin_descriptor, nullptr)
	{
	}

} // namespace lmms
