#include "epd.h"

EPD::~EPD()
{
  H_Logger->info("EPD::~EPD()");
  delete m_epd;
  delete m_gui_paint;
}

EPD::EPD(QJsonObject const &a_config)
  : m_epd{ new EPD_2in13() }
  , m_gui_paint{ new GUI_Paint() }
  , m_font{ new Fonts() }
  , m_imageProcessing(0)
  , m_imageAcc(0)
  , m_mode("none")
{
  m_epd->m_devEPDConfig->DEV_ModuleInit();

  if (m_epd->EPD_Init(FULL_UPDATE) != 0)
  {
    printf("e-Paper init failed\r\n");
  }
  m_epd->EPD_Clear();
  DEV_Delay_ms2(500);

  UWORD Imagesize = ((EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT;
  if ((m_blackImage = (UBYTE *)malloc(Imagesize)) == NULL)
  {
    printf("Failed to apply for memory...\r\n");
    H_Logger->error("Failed to apply for memory...\r\n");
  }

  H_Logger->trace("m_gui_paint->Paint_NewImage");
  m_gui_paint->Paint_NewImage(m_blackImage, EPD_WIDTH, EPD_HEIGHT, 270, WHITE);
  H_Logger->trace("m_gui_paint->Paint_SelectImage");
  m_gui_paint->Paint_SelectImage(m_blackImage);

  m_gui_paint->Paint_SetMirroring(MIRROR_HORIZONTAL);
  m_gui_paint->Paint_Clear(WHITE);

  m_gui_paint->Paint_SelectImage(m_blackImage);
  m_gui_paint->Paint_Clear(WHITE);

  // 2.Drawing on the image
  m_gui_paint->Paint_DrawPoint(5, 10, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
  m_gui_paint->Paint_DrawPoint(5, 25, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
  m_gui_paint->Paint_DrawPoint(5, 40, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
  m_gui_paint->Paint_DrawPoint(5, 55, BLACK, DOT_PIXEL_4X4, DOT_STYLE_DFT);

  m_gui_paint->Paint_DrawLine(20, 10, 70, 60, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);
  m_gui_paint->Paint_DrawLine(70, 10, 20, 60, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);
  m_gui_paint->Paint_DrawRectangle(20, 10, 70, 60, BLACK, DRAW_FILL_EMPTY, DOT_PIXEL_1X1);
  m_gui_paint->Paint_DrawRectangle(85, 10, 135, 60, BLACK, DRAW_FILL_FULL, DOT_PIXEL_1X1);

  m_gui_paint->Paint_DrawLine(45, 15, 45, 55, BLACK, LINE_STYLE_DOTTED, DOT_PIXEL_1X1);
  m_gui_paint->Paint_DrawLine(25, 35, 70, 35, BLACK, LINE_STYLE_DOTTED, DOT_PIXEL_1X1);
  m_gui_paint->Paint_DrawCircle(45, 35, 20, BLACK, DRAW_FILL_EMPTY, DOT_PIXEL_1X1);
  m_gui_paint->Paint_DrawCircle(110, 35, 20, WHITE, DRAW_FILL_FULL, DOT_PIXEL_1X1);

  m_gui_paint->Paint_DrawString_EN(150, 5, "EyeSpy", &m_font->m_Font20, WHITE, BLACK);
  // m_gui_paint->Paint_DrawNum(140, 40, 123456, &m_font->m_Font20, BLACK, WHITE);

  // m_gui_paint->Paint_DrawString_CN(140, 60, "EyeSpy", &m_font->m_Font20, BLACK, WHITE);
  // m_gui_paint->Paint_DrawString_CN(5, 65, "JAZDA!!", &m_font->m_Font20, WHITE, BLACK);

  m_epd->EPD_Display(m_blackImage);

  if (m_epd->EPD_Init(PART_UPDATE) != 0)
  {
    H_Logger->error("e-Paper init failed");
  }
  m_gui_paint->Paint_SelectImage(m_blackImage);
}

void EPD::onUpdate()
{
  PAINT_TIME sPaint_time;
  struct tm *t;
  time_t tt;
  H_Logger->trace("EPD::update():");
  time(&tt);
  H_Logger->trace("time2:");
  t = localtime(&tt);
  H_Logger->trace("time3:");
  sPaint_time.Hour = t->tm_hour;
  sPaint_time.Min = t->tm_min;
  sPaint_time.Sec = t->tm_sec;
  H_Logger->trace("Paint_ClearWindows:");
  H_Logger->trace("Fonts::Fonts() m_font->m_Font20.Width:{}", m_font->m_Font20.Width);
  H_Logger->trace("Fonts::Fonts() m_font->m_Font20.Height:{}", m_font->m_Font20.Height);
  m_gui_paint->Paint_ClearWindows(150, 90, 150 + m_font->m_Font20.Width * 7, 90 + m_font->m_Font20.Height, WHITE);
  H_Logger->trace("Paint_DrawTime:");
  m_gui_paint->Paint_DrawTime(150, 90, &sPaint_time, &(m_font->m_Font20), WHITE, BLACK);

  // m_gui_paint->Paint_ClearWindows(0, 0, 100, 100, WHITE);
  // m_gui_paint->Paint_DrawLine(t->tm_sec, 0, 60 - t->tm_sec, 100, BLACK, LINE_STYLE_SOLID, DOT_PIXEL_1X1);

  // onRefreshStats();
  m_gui_paint->Paint_ClearWindows(0, 0, 150, 122, WHITE);
  H_Logger->trace("ip:{},acc:{}", m_imageProcessing, m_imageAcc);
  m_gui_paint->Paint_DrawString_EN(1, 10, ("iproc:" + QString::number(m_imageProcessing)).toStdString().c_str(),
                                   &m_font->m_Font20, WHITE, BLACK);
  m_gui_paint->Paint_DrawString_EN(1, 30, ("acc:" + QString::number(m_imageAcc)).toStdString().c_str(),
                                   &m_font->m_Font20, WHITE, BLACK);
  m_gui_paint->Paint_DrawString_EN(1, 50, ("mode:" + m_mode).toStdString().c_str(), &m_font->m_Font20, WHITE, BLACK);
  H_Logger->trace("EPD_DisplayPart:");
  m_epd->EPD_DisplayPart(m_blackImage);
}

void EPD::onRefreshIP(qint32 imageProcessing)
{
  m_imageProcessing = imageProcessing;
  H_Logger->trace("EPD::onRefreshAcc() m_imageProcessing:{}", m_imageProcessing);
}

void EPD::onRefreshAcc(qint32 imageAcc)
{
  m_imageAcc = imageAcc;
  H_Logger->trace("EPD::onRefreshAcc() m_imageAcc:{}", m_imageAcc);
}
void EPD::onRefreshMode(QString mode)
{
  m_mode = mode;
  H_Logger->debug("EPD::onRefreshMode() m_mode:{}", m_mode.toStdString());
}
