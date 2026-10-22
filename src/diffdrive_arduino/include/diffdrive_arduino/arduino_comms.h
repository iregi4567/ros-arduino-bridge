#ifndef DIFFDRIVE_ARDUINO_ARDUINO_COMMS_H
#define DIFFDRIVE_ARDUINO_ARDUINO_COMMS_H

#include <serial/serial.h>
#include <cstring>
#include <mutex>


class ArduinoComms
{


public:

  ArduinoComms()
  {  }

  ArduinoComms(const std::string &serial_device, int32_t baud_rate, int32_t timeout_ms)
      : serial_conn_(serial_device, baud_rate, serial::Timeout::simpleTimeout(timeout_ms))
  {  }
  

  void setup(const std::string &serial_device, int32_t baud_rate, int32_t timeout_ms);
  void sendEmptyMsg();
  void readEncoderValues(int &val_1, int &val_2);
  void setMotorValues(int val_1, int val_2);
  void setOffloaderValue(int val);
  void setPidValues(float k_p, float k_d, float k_i, float k_o);

  bool connected() const { return serial_conn_.isOpen(); }
  void readImuValues(double &ax, double &ay, double &az,
                   double &gx, double &gy, double &gz,
                   double &qx, double &qy, double &qz, double &qw);
  void setRelay(char cmd);   // send a single-letter relay command (e.g. 'f', 'g', 'h', 'j')



  std::string sendMsg(const std::string &msg_to_send, bool print_output = false);


private:
  serial::Serial serial_conn_;  ///< Underlying serial connection 
  std::mutex serial_mutex_;   // protect serial access

};

#endif // DIFFDRIVE_ARDUINO_ARDUINO_COMMS_H