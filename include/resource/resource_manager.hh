#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace nile {

  class Shader;
  class Texture2D;

  class ResourceManager {
  private:
    static ResourceManager *m_instance;
    ResourceManager() noexcept;

    // Resource Storage
    static std::map<std::string, std::shared_ptr<Shader>> shaders;
    static std::map<std::string, std::shared_ptr<Texture2D>> textures;

    // Loads and generates shader from file
    static Shader loadShaderFromFile( std::string_view vshaderFile, std::string_view fshaderFile,
                                      std::string_view gshaderFile = {} ) noexcept;

    // Loads single texture from file
    static Texture2D loadTextureFromFile( std::string_view file, bool alpha ) noexcept;

  public:
    static ResourceManager *getInstance() noexcept;
    static void destroy() noexcept;
  };

}    // namespace nile
