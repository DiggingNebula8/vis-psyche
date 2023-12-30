#include <VizPsyche.h>

class Sandbox : public VizPsyche::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

VizPsyche::Application* VizPsyche::CreateApplication()
{
	return new Sandbox();
}