#include <Wt/WApplication.h>
#include <Wt/WEnvironment.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WServer.h>
#include <Wt/WLinkedCssStyleSheet.h>

#include "WMainWindow.h"
#include <iostream>


std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env)
{
	auto app = std::make_unique<Wt::WApplication>(env);

	// ace editor
	app->require("lib/ace.js");

	// use default bootstrap theme
	auto theme = std::make_shared<Wt::WBootstrapTheme>();
	theme->setVersion(Wt::BootstrapVersion::v3);
	theme->setResponsive(true);
	app->setTheme(theme);

	app->setTitle("PeriDyno: An AI-targeted physics simulation platform");

	// style sheet for the canvas
	app->styleSheet().addRule(
		".remote-framebuffer",
		// flip
		"transform: scaleY(-1) !important;"
		// disable drag...
		"-webkit-user-drag: none !important;"
		"-khtml-user-drag: none !important;"
		"-moz-user-drag: none !important;"
		"-o-user-drag: none !important;"
		"user-drag: none !important;"
	);

	// override internal padding for panel...
	app->styleSheet().addRule(
		".panel-body",
		"padding: 0!important;"
	);

	app->styleSheet().addRule(
		".sample-item",
		"border-radius: 5px;"
	);

	// add logo to navbar
	app->styleSheet().addRule(
		".navbar-header",
		"background-image: url(\"logo.png\");"
		"background-repeat: no-repeat;"
		"background-size: 36px 36px;"
		"background-position: 12px 6px;"
		"padding-left: 36px;"
	);

	// color picker button style
	app->styleSheet().addRule(
		".color-picker",
		"border: 0!important;"
		"padding: 0!important;"
	);

	// set layout and add main window
	auto layout = app->root()->setLayout(std::make_unique<Wt::WHBoxLayout>());
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(std::make_unique<WMainWindow>());

	return app;
}


int main(int argc, char **argv)
{
	try {

		if (argc == 1)
		{
			// default args
			std::vector<std::string> args;
			args.push_back("--http-listen");
			args.push_back("0.0.0.0:5000");
			args.push_back("--docroot");
			args.push_back("docroot");
			return Wt::WRun(argv[0], args, &createApplication);
		}
		else
		{
			Wt::WServer server(argc, argv);
			server.addEntryPoint(Wt::EntryPointType::Application, &createApplication);
			server.run();
		}

	}
	catch (Wt::WServer::Exception& e) {
		std::cerr << e.what() << "\n";
		return 1;
	}
	catch (std::exception& e) {
		std::cerr << "exception: " << e.what() << "\n";
		return 1;
	}
}
