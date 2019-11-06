#pragma once

#include "Nile/core/types.hh"

#include <string>
#include <vector>

namespace nile::editor {

  class InfoWindow {
  private:
    static const u32 MAX_LABELS_SIZE = 32;
    bool m_infoWindowIsOpen = true;

    // Those are labels ( key/values ) paris that
    // will be printed in the infowindow
    std::vector<std::string> m_labels;

    u32 m_labelsSize = 0;

  public:
    InfoWindow() noexcept;
    void render( float dt ) noexcept;
    void update( float dt ) noexcept;

    void setInfoWindowIsOpen( bool value ) noexcept {
      m_infoWindowIsOpen = value;
    }

    bool isInfoWindowOpen() const noexcept {
      return m_infoWindowIsOpen;
    }

    void addLabel( const char *fmt, ... ) noexcept;
  };

}    // namespace nile::editor