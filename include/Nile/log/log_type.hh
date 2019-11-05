#pragma once

namespace nile {

  enum class LogType { 
    NONE = 0x00, 
    NOTICE = 0x01, 
    ERROR = 0x02, 
    DISPLAY = 0x04, // printed out only in editor console
    WARNING = 0x08,
    PRINT = 0x10, 
    FATAL = 0x20  // printed out only in file and stderr
  };

}
