#include "fonts.h"
Fonts::Fonts()
{
  H_Logger->debug("Fonts::Fonts()");
  m_Font20 = {
    Font20_Table, 14, /* Width */
    20,               /* Height */
  };
  H_Logger->debug("Fonts::Fonts() m_Font20.Width:{}", m_Font20.Width);
  H_Logger->debug("Fonts::Fonts() m_Font20.Height:{}", m_Font20.Height);
}
