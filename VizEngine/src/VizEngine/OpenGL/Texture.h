#pragma once

#include <iostream>
#include<glad/glad.h>
#include"stb_image.h"

class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	// Prevent copying (Rule of 5)
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Allow moving
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline unsigned int GetID() const { return m_RendererID; }

private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
};