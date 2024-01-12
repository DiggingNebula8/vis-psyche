#include <VizEngine.h>

class ExampleLayer : public VizEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		VE_INFO("ExampleLayer::Update");
	}

	void OnEvent(VizEngine::Event& event) override
	{
		VE_TRACE("{0}", event);
	}

};

class Sandbox : public VizEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new VizEngine::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

VizEngine::Application* VizEngine::CreateApplication()
{
	return new Sandbox();
}