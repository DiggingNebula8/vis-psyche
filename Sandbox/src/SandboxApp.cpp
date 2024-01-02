#include <VizEngine.h>

class Sandbox : public VizEngine::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

VizEngine::Application* VizEngine::CreateApplication()
{
	return new Sandbox();
}