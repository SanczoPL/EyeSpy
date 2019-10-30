#include "adc.h"
#include <stdio.h>

constexpr auto A0{ "A0" };
constexpr auto A1{ "A1" };
constexpr auto A2{ "A2" };
constexpr auto A3{ "A3" };
#define PCF 120

AnalogReadWrite::AnalogReadWrite(QJsonObject const &a_config)
  : counter(0)
  , m_counter(0)
  , m_addingCounter(0)
  , m_counterA0(0)
  , m_counterA1(0)
  , m_counterA2(0)
  , m_counterA3(0)
  , m_LapMeterFlag(false)
  , m_a0{ a_config[A0].toBool() }
  , m_a1{ a_config[A1].toBool() }
  , m_a2{ a_config[A2].toBool() }
  , m_a3{ a_config[A3].toBool() }
{
  H_Logger->trace("AnalogReadWrite::AnalogReadWrite()");
  wiringPiSetup();
  // Setup pcf8591 on base pin 120, and address 0x48
  pcf8591Setup(PCF, 0x48);
  m_timerA2.restart();
}
void AnalogReadWrite::configure(QJsonObject const &a_config)
{
  m_a0 = { a_config[A0].toBool() };
  m_a1 = { a_config[A1].toBool() };
  m_a2 = { a_config[A2].toBool() };
  m_a3 = { a_config[A3].toBool() };
}

void AnalogReadWrite::onUpdate()
{
  QTime myTimer;
  myTimer.start();

  m_counter++;
  m_counterA0++;
  m_counterA1++;
  m_counterA2++;
  m_counterA3++;

  counter += 10;
  analogWrite(PCF + 0, counter);

  if (m_a0)
  {
    int value;
    H_Logger->trace("AnalogReadWrite::onUpdate()");
    value = analogRead(PCF + 0);
    double volt = value / 255.0 * 3.3;
    H_Logger->debug("AnalogReadWrite::onUpdate() read A0:{} :{}", value, volt);

    if (value > 50 and value < 120 and m_counterA0 > 25)
    {
      m_counterA0 = 0;
      H_Logger->debug("(configure)AnalogReadWrite::onUpdate() read A1:{} :{}", value, volt);
      QJsonObject Control = { { "state", 1 } };
      emit(addState(Control));
    }
  }

  if (m_a1)
  {
    int value;
    H_Logger->trace("AnalogReadWrite::onUpdate()");
    value = analogRead(PCF + 1);
    double volt = value / 255.0 * 3.3;
    H_Logger->debug("AnalogReadWrite::onUpdate() read A1:{} :{}", value, volt);

    if (value > 50 and value < 120 and m_counterA1 > 25)
    {
      m_counterA1 = 0;
      H_Logger->debug("(configure)AnalogReadWrite::onUpdate() read A1:{} :{}", value, volt);
      QJsonObject Control = { { "state", 1 } };
      emit(addState(Control));
    }
  }
  if (m_a2)
  {
    int value;
    H_Logger->trace("AnalogReadWrite::onUpdate()");
    value = analogRead(PCF + 2);
    double volt = value / 255.0 * 3.3;
    // printf("%d\n", value);
    H_Logger->debug("AnalogReadWrite::onUpdate() read A2:{} :{}", value, volt);

    if (value > 40 and value < 120 and m_counterA2 > 25)
    {
      qint32 nMilliseconds2 = (qint32)m_timerA2.elapsed();
      QJsonObject toSendJSON;
      QJsonObject infoJSONOBJ = { { "A2_time", nMilliseconds2 } };
      toSendJSON = { { "method", "ADC" }, { "params", infoJSONOBJ } };
      emit(sendInfoJSON(toSendJSON));
      m_timerA2.restart();
      m_counterA2 = 0;
    }
  }

  quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
  m_addingCounter += nMilliseconds2;
  if (m_counter >= 100)
  {
    H_Logger->info("(timer) ADC time:{}", ((double)m_addingCounter / (double)m_counter));
    m_addingCounter = 0;
    m_counter = 0;
  }
}
