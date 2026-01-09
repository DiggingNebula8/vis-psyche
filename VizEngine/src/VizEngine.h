#pragma once

// =============================================================================
// VizEngine Public API
// =============================================================================
// This is the main header for VizEngine applications.
// Include this single header to access the entire public API.

// Core engine
#include "VizEngine/Application.h"
#include "VizEngine/Engine.h"
#include "VizEngine/Log.h"

// Subsystems accessible to applications
#include "VizEngine/GUI/UIManager.h"
#include "VizEngine/OpenGL/Renderer.h"
#include "VizEngine/OpenGL/Shader.h"
#include "VizEngine/OpenGL/Texture.h"
#include "VizEngine/OpenGL/Framebuffer.h"
#include "VizEngine/OpenGL/FullscreenQuad.h"
#include "VizEngine/OpenGL/CubemapUtils.h"
#include "VizEngine/Renderer/Skybox.h"

// Core types
#include "VizEngine/Core/Camera.h"
#include "VizEngine/Core/Transform.h"
#include "VizEngine/Core/Scene.h"
#include "VizEngine/Core/Mesh.h"
#include "VizEngine/Core/Light.h"
#include "VizEngine/Core/Input.h"

// Asset loading
#include "VizEngine/Core/Model.h"
#include "VizEngine/Core/Material.h"

// Events (for event-driven applications)
#include "VizEngine/Events/Event.h"
#include "VizEngine/Events/ApplicationEvent.h"
#include "VizEngine/Events/KeyEvent.h"
#include "VizEngine/Events/MouseEvent.h"

// Entry point (must be last)
#include "VizEngine/EntryPoint.h"
