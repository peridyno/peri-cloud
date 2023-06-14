#include <Wt/WServer.h>
#include <Wt/WLogger.h>

#include "WtApp.h"

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env)
{
	auto app = std::make_unique<WtApp>(env);

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
