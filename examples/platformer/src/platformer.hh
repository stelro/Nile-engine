#pragma once

#include "Nile/core/types.hh"
#include "text_buffer.hh"

#include <Nile/application/game.hh>
#include <Nile/asset/asset_manager.hh>
#include <Nile/core/input_manager.hh>
#include <Nile/core/settings.hh>
#include <Nile/ecs/ecs_coordinator.hh>
#include <Nile/experimental/asset/asset_manager_helper.hh>
#include <Nile/platform/game_host.hh>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace platformer {

  using nile::f32;

  class Platformer : public nile::Game {
  private:
    std::shared_ptr<nile::GameHost> game_host_;
    std::shared_ptr<nile::InputManager> input_manager_;
    std::shared_ptr<nile::BaseRenderer> renderer_;
    std::shared_ptr<nile::Settings> settings_;
    std::shared_ptr<nile::AssetManager> assets_manager_;
    std::shared_ptr<nile::Coordinator> ecs_coordinator_;
    ///    std::shared_ptr<nile::AssetManagerHelper> m_assetManagerHelper;

    std::unique_ptr<TextBuffer> text_buffer_;

    nile::Entity camera_entity_;
    nile::Entity lamp_entity_;
    nile::Entity nano_model_entity_;
    nile::Entity subscene_entity_;
    nile::Entity sprite_entity_;

    nile::Entity test_entity_;

    void draw_stone_tiles() noexcept;
    void draw_nano_model() noexcept;
    void draw_textured_floor() noexcept;
    void draw_containers() noexcept;
    void draw_grass() noexcept;
    void draw_windows() noexcept;
    void draw_text_font() noexcept;
    void draw_point_lights() noexcept;
    void draw_sprites_test() noexcept;
    void draw_pizza_box() noexcept;
    void draw_micro_subscene() noexcept;

    void process_keyboard_events( f32 dt ) noexcept;
    void process_mouse_events( f32 dt ) noexcept;
    void process_mouse_scrolling_events( f32 dt ) noexcept;

    // Used for FPS-like camrea
    f32 last_x_;
    f32 last_y_;
    bool first_mouse_entry_ = true;
    glm::ivec2 mouse_position_;

    glm::vec3 default_light_color_ { 1.0f, 1.0f, 1.0f };

    std::vector<glm::vec3> point_lights_positions_;

  public:
    Platformer( const std::shared_ptr<nile::GameHost> &gameHost ) noexcept;
    ~Platformer() = default;
    virtual void initialize() noexcept override;
    virtual void draw( f32 deltaTime ) noexcept override;
    virtual void update( f32 deltaTime ) noexcept override;
  };


}    // namespace platformer

