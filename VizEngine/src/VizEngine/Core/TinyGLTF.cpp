/**
 * TinyGLTF Implementation
 * 
 * This file contains the implementation of the tinygltf library.
 * tinygltf is header-only - we define TINYGLTF_IMPLEMENTATION once here.
 * 
 * We let tinygltf use stb_image for loading, but don't redefine STB_IMAGE_IMPLEMENTATION
 * since it's already defined in vendor/stb_image/stb_image.cpp
 */

// Silence some warnings in the tinygltf code
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4018)  // signed/unsigned mismatch
#pragma warning(disable: 4267)  // conversion from size_t
#pragma warning(disable: 4244)  // conversion from double to float
#pragma warning(disable: 4706)  // assignment within conditional
#endif

#define TINYGLTF_IMPLEMENTATION

// We already have stb_image implemented in vendor/stb_image/stb_image.cpp
// So we tell tinygltf to NOT define STB_IMAGE_IMPLEMENTATION again
// but still use stb_image functions (which are already available)
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE

// Include our stb_image header (which has the function declarations)
#include "stb_image.h"

#include "tiny_gltf.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif
