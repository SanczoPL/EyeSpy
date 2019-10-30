#ifndef EPD_H
#define EPD_H

#include <QJsonArray>
#include <QJsonObject>

#include <QObject>
#include "../EPD/DEV_EPD_Config.h"
#include "../EPD/EPD_2in13.h"
#include "../EPD/Fonts/fonts.h"
#include "../EPD/GUI_Paint.h"

class EPD : public QObject
{
  Q_OBJECT
 public:
  EPD(QJsonObject const &a_config);
  ~EPD();

 signals:

 public slots:
  void onUpdate();
  void onRefreshAcc(qint32 imageAcc);
  void onRefreshIP(qint32 imageProcessing);
  void onRefreshMode(QString mode);

 private:
  EPD_2in13 *m_epd;
  GUI_Paint *m_gui_paint;
  Fonts *m_font;

  UBYTE *m_blackImage;

 private:
  qint32 m_imageProcessing{};
  qint32 m_imageAcc;
  QString m_mode;
};

#endif // EPD_H
