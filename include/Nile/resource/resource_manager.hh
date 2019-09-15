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

    // Free all resources
    static void clear() noexcept;

    // Load shaders from file and returns the generated shader object
    static std::shared_ptr<Shader> loadShader( std::string_view vshaderFile, std::string_view fshaderFile,
                              std::string_view gshaderFile, const std::string &name ) noexcept;

    // Load texture from file and returns the texture
    static std::shared_ptr<Texture2D> loadTexture( std::string_view file, bool alpha,
                                  const std::string &name ) noexcept;

    // Retrives a stored shader
    static std::shared_ptr<Shader> getShader( const std::string &name ) noexcept;

    // Retrives a stored Texture
    static std::shared_ptr<Texture2D> getTexture( const std::string &name ) noexcept;
  };

}    // namespace nile
