#include "rtimuloop.h"

RTIMULoop::~RTIMULoop()
{
  delete m_settings;
  delete m_imu;
}

RTIMULoop::RTIMULoop()
  : m_settings(new RTIMUSettings("RTIMULib.ini"))
  , m_imu(RTIMU::createIMU(m_settings))
  , m_counter(0)
  , m_addingCounter(0)
  , m_imuExist(true)
{
  if ((m_imu == NULL) || (m_imu->IMUType() == RTIMU_TYPE_NULL))
  {
    printf("No IMU found\n");
    H_Logger->error("RTIMULoop No IMU found");
    m_imuExist = false;
  }
  if (m_imuExist)
  {
    m_imu->IMUInit();
    m_imu->setSlerpPower(0.02);
    m_imu->setGyroEnable(true);
    m_imu->setAccelEnable(true);
    m_imu->setCompassEnable(true);
    connect(this, &RTIMULoop::update, this, &RTIMULoop::onShowDate);
  }
}

void RTIMULoop::onUpdate()
{
  if (m_imuExist)
  {
    m_counter++;
    QTime myTimer;
    int sampleCount = 0;
    int sampleRate = 0;
    uint64_t rateTimer;
    uint64_t displayTimer;
    uint64_t now;
    rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();
    H_Logger->trace("RTIMULoop::onUpdate()");
    // usleep(m_imu->IMUGetPollInterval() * 1000); // 10 ms

    if (m_imu->IMURead())
    {
      RTIMU_DATA imuData = m_imu->getIMUData();
      // sampleCount++;

      now = RTMath::currentUSecsSinceEpoch();
      emit(update(imuData.fusionPose));
      emit(setActual((double)imuData.fusionPose.x()));
      emit(showDateFromRTIMUL(imuData.fusionPose));

      QJsonObject toSendJSON;
      QJsonObject infoJSONOBJ = { { "time", qint64(imuData.timestamp) },
                                  { "x", (imuData.accel.x()) },
                                  { "y", (imuData.accel.y()) },
                                  { "z", (imuData.accel.z()) } };
      toSendJSON = { { "method", "RTI" }, { "params", infoJSONOBJ } };
      emit(sendInfoJSON(toSendJSON));
    }
    quint32 nMilliseconds2 = (quint32)myTimer.elapsed();
    m_addingCounter += nMilliseconds2;
    if (m_counter >= 100)
    {
      H_Logger->info("(timer)RTIMULoop::onUpdate time:{}", ((double)m_addingCounter / (double)m_counter));
      m_addingCounter = 0;
      m_counter = 0;
    }
  }
}

void RTIMULoop::onShowDate(RTVector3 data)
{
  // printf("Sample rate %s\r", RTMath::displayDegrees("", data));
  // H_Logger->debug("{}", RTMath::displayDegrees("", data));
  // fflush(stdout);
}
