#include "Application.hpp"

int main()
{
	TerracottaEngine::Application app(1920, 1080);

	while (app.IsAppRunning()) {
		app.Run();
	}

	return 0;
}
