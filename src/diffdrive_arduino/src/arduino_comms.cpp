#include "diffdrive_arduino/arduino_comms.h"
// #include <ros/console.h>
#include <rclcpp/rclcpp.hpp>
#include <sstream>
#include <cstdlib>
#include <mutex>   // at top if not present


void ArduinoComms::setup(const std::string &serial_device, int32_t baud_rate, int32_t timeout_ms)
{  
    serial_conn_.setPort(serial_device);
    serial_conn_.setBaudrate(baud_rate);
    serial::Timeout tt = serial::Timeout::simpleTimeout(timeout_ms);
    serial_conn_.setTimeout(tt); // This should be inline except setTimeout takes a reference and so needs a variable
    serial_conn_.open();
    // serial_conn_.(serial_device, baud_rate, serial::Timeout::simpleTimeout(timeout_ms));

}

std::string ArduinoComms::sendMsg(const std::string &msg_to_send, bool print_output)
{
    std::lock_guard<std::mutex> lock(serial_mutex_);
    serial_conn_.write(msg_to_send);
    // readline() will block until a newline or timeout
    std::string response = serial_conn_.readline();

    if (print_output)
    {
        // logging optional
    }

    return response;
}
void ArduinoComms::setRelay(char cmd)
{
    // Compose a 1-char command + carriage return to match Arduino expectations
    std::string msg;
    msg.push_back(cmd);
    msg.push_back('\r');
    // we dont force print output, but you can set true for debugging
    sendMsg(msg, false);
}


void ArduinoComms::sendEmptyMsg()
{
    std::string response = sendMsg("\r");
}

void ArduinoComms::readEncoderValues(int &val_1, int &val_2)
{
    std::string response = sendMsg("e\r");

    std::string delimiter = " ";
    size_t del_pos = response.find(delimiter);
    std::string token_1 = response.substr(0, del_pos);
    std::string token_2 = response.substr(del_pos + delimiter.length());

    val_1 = std::atoi(token_1.c_str());
    val_2 = std::atoi(token_2.c_str());
}

void ArduinoComms::setMotorValues(int val_1, int val_2)
{
    std::stringstream ss;
    ss << "m " << val_1 << " " << val_2 << "\r";
    sendMsg(ss.str(), false);
}

void ArduinoComms::setOffloaderValue(int val)
{
    std::stringstream ss;
    ss << "l " << val << "\r";
    sendMsg(ss.str(), false);
}

void ArduinoComms::setPidValues(float k_p, float k_d, float k_i, float k_o)
{
    std::stringstream ss;
    ss << "u " << k_p << ":" << k_d << ":" << k_i << ":" << k_o << "\r";
    sendMsg(ss.str());
}

void ArduinoComms::readImuValues(double &ax, double &ay, double &az,
                                 double &gx, double &gy, double &gz,
                                 double &qx, double &qy, double &qz, double &qw)
{
  std::string response = sendMsg("i\r");
  std::istringstream iss(response);
  iss >> ax >> ay >> az >> gx >> gy >> gz >> qx >> qy >> qz >> qw;
}
