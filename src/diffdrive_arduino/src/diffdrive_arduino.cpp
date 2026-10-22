// #include "diffdrive_arduino/diffdrive_arduino.h"


// #include "hardware_interface/types/hardware_interface_type_values.hpp"
// #include "hardware_interface/types/hardware_interface_return_values.hpp"

// #include "rclcpp/rclcpp.hpp"

// namespace diffdrive_arduino
// {


// DiffDriveArduino::DiffDriveArduino()
//     : logger_(rclcpp::get_logger("DiffDriveArduino"))
// {}

// CallbackReturn DiffDriveArduino::on_init(const hardware_interface::HardwareInfo & info)
// {
//   if (hardware_interface::SystemInterface::on_init(info) != CallbackReturn::SUCCESS)
// {
//   return CallbackReturn::ERROR;
// }

//   RCLCPP_INFO(logger_, "Configuring...");

//   time_ = std::chrono::system_clock::now();

//   cfg_.left_wheel_name = info_.hardware_parameters["left_wheel_name"];
//   cfg_.right_wheel_name = info_.hardware_parameters["right_wheel_name"];
//   cfg_.loop_rate = std::stof(info_.hardware_parameters["loop_rate"]);
//   cfg_.device = info_.hardware_parameters["device"];
//   cfg_.baud_rate = std::stoi(info_.hardware_parameters["baud_rate"]);
//   cfg_.timeout = std::stoi(info_.hardware_parameters["timeout"]);
//   cfg_.enc_counts_per_rev = std::stoi(info_.hardware_parameters["enc_counts_per_rev"]);

//   // Set up the wheels
//   l_wheel_.setup(cfg_.left_wheel_name, cfg_.enc_counts_per_rev);
//   r_wheel_.setup(cfg_.right_wheel_name, cfg_.enc_counts_per_rev);

//   // Set up the Arduino
//   arduino_.setup(cfg_.device, cfg_.baud_rate, cfg_.timeout);  

//   RCLCPP_INFO(logger_, "Finished Configuration");

//   return CallbackReturn::SUCCESS;
// }

// std::vector<hardware_interface::StateInterface> DiffDriveArduino::export_state_interfaces()
// {
//   // We need to set up a position and a velocity interface for each wheel

//   std::vector<hardware_interface::StateInterface> state_interfaces;

//   state_interfaces.emplace_back(hardware_interface::StateInterface(l_wheel_.name, hardware_interface::HW_IF_VELOCITY, &l_wheel_.vel));
//   state_interfaces.emplace_back(hardware_interface::StateInterface(l_wheel_.name, hardware_interface::HW_IF_POSITION, &l_wheel_.pos));
//   state_interfaces.emplace_back(hardware_interface::StateInterface(r_wheel_.name, hardware_interface::HW_IF_VELOCITY, &r_wheel_.vel));
//   state_interfaces.emplace_back(hardware_interface::StateInterface(r_wheel_.name, hardware_interface::HW_IF_POSITION, &r_wheel_.pos));
//   // Linear acceleration

//   // --- IMU ---
//   state_interfaces.emplace_back("imu_sensor", "orientation.x", &imu_orientation_[0]);
//   state_interfaces.emplace_back("imu_sensor", "orientation.y", &imu_orientation_[1]);
//   state_interfaces.emplace_back("imu_sensor", "orientation.z", &imu_orientation_[2]);
//   state_interfaces.emplace_back("imu_sensor", "orientation.w", &imu_orientation_[3]);

// state_interfaces.emplace_back(
//   hardware_interface::StateInterface("imu_sensor", "linear_acceleration.x", &imu_ax_));
// state_interfaces.emplace_back(
//   hardware_interface::StateInterface("imu_sensor", "linear_acceleration.y", &imu_ay_));
// state_interfaces.emplace_back(
//   hardware_interface::StateInterface("imu_sensor", "linear_acceleration.z", &imu_az_));

// // Angular velocity
// state_interfaces.emplace_back(
//   hardware_interface::StateInterface("imu_sensor", "angular_velocity.x", &imu_gx_));
// state_interfaces.emplace_back(
//   hardware_interface::StateInterface("imu_sensor", "angular_velocity.y", &imu_gy_));
// state_interfaces.emplace_back(
//   hardware_interface::StateInterface("imu_sensor", "angular_velocity.z", &imu_gz_));



//   return state_interfaces;
// }

// std::vector<hardware_interface::CommandInterface> DiffDriveArduino::export_command_interfaces()
// {
//   // We need to set up a velocity command interface for each wheel

//   std::vector<hardware_interface::CommandInterface> command_interfaces;

//   command_interfaces.emplace_back(hardware_interface::CommandInterface(l_wheel_.name, hardware_interface::HW_IF_VELOCITY, &l_wheel_.cmd));
//   command_interfaces.emplace_back(hardware_interface::CommandInterface(r_wheel_.name, hardware_interface::HW_IF_VELOCITY, &r_wheel_.cmd));

//   return command_interfaces;
// }


// CallbackReturn DiffDriveArduino::on_activate(const rclcpp_lifecycle::State & /* previous_state */)
// {
//   RCLCPP_INFO(logger_, "Starting Controller...");
//   arduino_.sendEmptyMsg();
//   // arduino.setPidValues(9,7,0,100);
//   // arduino.setPidValues(14,7,0,100);
//   arduino_.setPidValues(30, 20, 0, 100);

//   return CallbackReturn::SUCCESS;
// }

// CallbackReturn DiffDriveArduino::on_deactivate(const rclcpp_lifecycle::State & /* previous_state */)
// {
//   RCLCPP_INFO(logger_, "Stopping Controller...");

//   return CallbackReturn::SUCCESS;
// }

// return_type DiffDriveArduino::read(const rclcpp::Time & /* time */, const rclcpp::Duration & /* period */)
// {

//   // TODO fix chrono duration

//   // Calculate time delta
//   auto new_time = std::chrono::system_clock::now();
//   std::chrono::duration<double> diff = new_time - time_;
//   double deltaSeconds = diff.count();
//   time_ = new_time;


//   if (!arduino_.connected())
//   {
//     return return_type::ERROR;
//   }

//   arduino_.readEncoderValues(l_wheel_.enc, r_wheel_.enc);

//   double pos_prev = l_wheel_.pos;
//   l_wheel_.pos = l_wheel_.calcEncAngle();
//   l_wheel_.vel = (l_wheel_.pos - pos_prev) / deltaSeconds;

//   pos_prev = r_wheel_.pos;
//   r_wheel_.pos = r_wheel_.calcEncAngle();
//   r_wheel_.vel = (r_wheel_.pos - pos_prev) / deltaSeconds;
//   // ---------------- IMU ----------------
// double ax, ay, az, gx, gy, gz, qx, qy, qz, qw;
// arduino_.readImuValues(ax, ay, az, gx, gy, gz, qx, qy, qz, qw);

// // Convert accel to m/s²
// imu_ax_ = ax * 9.80665;
// imu_ay_ = ay * 9.80665;
// imu_az_ = az * 9.80665;

// // Convert gyro to rad/s
// imu_gx_ = gx * M_PI / 180.0;
// imu_gy_ = gy * M_PI / 180.0;
// imu_gz_ = gz * M_PI / 180.0;

// // Convert IMU quaternion (NED) → ROS (ENU)
// // Swap Y/Z, invert new X
// imu_orientation_[0] = qy;  // x
// imu_orientation_[1] =  qx;  // y
// imu_orientation_[2] =  qz;  // z
// imu_orientation_[3] =  qw;  // w

//   return return_type::OK;

  
// }

// return_type DiffDriveArduino::write(const rclcpp::Time & /* time */, const rclcpp::Duration & /* period */)
// {

//   if (!arduino_.connected())
//   {
//     return return_type::ERROR;
//   }

//   arduino_.setMotorValues(l_wheel_.cmd / l_wheel_.rads_per_count / cfg_.loop_rate, r_wheel_.cmd / r_wheel_.rads_per_count / cfg_.loop_rate);




//   return return_type::OK;


  
// }

// } // namespace diffdrive_arduino

// #include "pluginlib/class_list_macros.hpp"

// PLUGINLIB_EXPORT_CLASS(
//   diffdrive_arduino::DiffDriveArduino,
//   hardware_interface::SystemInterface
// )


#include "diffdrive_arduino/diffdrive_arduino.h"

#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/bool.hpp"
#include <functional>

namespace diffdrive_arduino
{

DiffDriveArduino::DiffDriveArduino()
    : logger_(rclcpp::get_logger("DiffDriveArduino"))
{}

CallbackReturn DiffDriveArduino::on_init(const hardware_interface::HardwareInfo & info)
{
  if (hardware_interface::SystemInterface::on_init(info) != CallbackReturn::SUCCESS)
  {
    return CallbackReturn::ERROR;
  }

  RCLCPP_INFO(logger_, "Configuring...");

  time_ = std::chrono::system_clock::now();

  cfg_.left_wheel_name = info_.hardware_parameters["left_wheel_name"];
  cfg_.right_wheel_name = info_.hardware_parameters["right_wheel_name"];
  cfg_.loop_rate = std::stof(info_.hardware_parameters["loop_rate"]);
  cfg_.device = info_.hardware_parameters["device"];
  cfg_.baud_rate = std::stoi(info_.hardware_parameters["baud_rate"]);
  cfg_.timeout = std::stoi(info_.hardware_parameters["timeout"]);
  cfg_.enc_counts_per_rev = std::stoi(info_.hardware_parameters["enc_counts_per_rev"]);

  // Set up wheels and Arduino
  l_wheel_.setup(cfg_.left_wheel_name, cfg_.enc_counts_per_rev);
  r_wheel_.setup(cfg_.right_wheel_name, cfg_.enc_counts_per_rev);
  arduino_.setup(cfg_.device, cfg_.baud_rate, cfg_.timeout);

  // ---------------- CREATE RELAY SUBSCRIPTIONS ----------------
  try {
    node_ = std::make_shared<rclcpp::Node>("diffdrive_arduino_relay_node");

    pump_sub_ = node_->create_subscription<std_msgs::msg::Bool>(
      "pump_relay", 10,
      std::bind(&DiffDriveArduino::pumpCallback, this, std::placeholders::_1));

    vac_sub_ = node_->create_subscription<std_msgs::msg::Bool>(
      "vacuum_relay", 10,
      std::bind(&DiffDriveArduino::vacuumCallback, this, std::placeholders::_1));

    RCLCPP_INFO(logger_, "Relay subscriptions created (pump_relay, vacuum_relay)");

    // 🌀 Start spinning thread
    spinning_ = true;
    spin_thread_ = std::thread([this]() {
      rclcpp::executors::SingleThreadedExecutor executor;
      executor.add_node(node_);
      while (spinning_) {
        executor.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }
    });

  } catch (const std::exception &e) {
    RCLCPP_WARN(logger_, "Failed to create relay subscriptions: %s", e.what());
  }
  // ------------------------------------------------------------

  RCLCPP_INFO(logger_, "Finished Configuration");
  return CallbackReturn::SUCCESS;
}

// ---------------------------------------------
std::vector<hardware_interface::StateInterface> DiffDriveArduino::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;

  state_interfaces.emplace_back(l_wheel_.name, hardware_interface::HW_IF_VELOCITY, &l_wheel_.vel);
  state_interfaces.emplace_back(l_wheel_.name, hardware_interface::HW_IF_POSITION, &l_wheel_.pos);
  state_interfaces.emplace_back(r_wheel_.name, hardware_interface::HW_IF_VELOCITY, &r_wheel_.vel);
  state_interfaces.emplace_back(r_wheel_.name, hardware_interface::HW_IF_POSITION, &r_wheel_.pos);

  // IMU interfaces
  state_interfaces.emplace_back("imu_sensor", "orientation.x", &imu_orientation_[0]);
  state_interfaces.emplace_back("imu_sensor", "orientation.y", &imu_orientation_[1]);
  state_interfaces.emplace_back("imu_sensor", "orientation.z", &imu_orientation_[2]);
  state_interfaces.emplace_back("imu_sensor", "orientation.w", &imu_orientation_[3]);
  state_interfaces.emplace_back("imu_sensor", "linear_acceleration.x", &imu_ax_);
  state_interfaces.emplace_back("imu_sensor", "linear_acceleration.y", &imu_ay_);
  state_interfaces.emplace_back("imu_sensor", "linear_acceleration.z", &imu_az_);
  state_interfaces.emplace_back("imu_sensor", "angular_velocity.x", &imu_gx_);
  state_interfaces.emplace_back("imu_sensor", "angular_velocity.y", &imu_gy_);
  state_interfaces.emplace_back("imu_sensor", "angular_velocity.z", &imu_gz_);

  return state_interfaces;
}

// ---------------------------------------------
std::vector<hardware_interface::CommandInterface> DiffDriveArduino::export_command_interfaces()
{
  std::vector<hardware_interface::CommandInterface> command_interfaces;

  command_interfaces.emplace_back(l_wheel_.name, hardware_interface::HW_IF_VELOCITY, &l_wheel_.cmd);
  command_interfaces.emplace_back(r_wheel_.name, hardware_interface::HW_IF_VELOCITY, &r_wheel_.cmd);
  command_interfaces.emplace_back("offloader_joint", hardware_interface::HW_IF_VELOCITY, &offloader_cmd_);

  return command_interfaces;
}

// ---------------------------------------------
CallbackReturn DiffDriveArduino::on_activate(const rclcpp_lifecycle::State &)
{
  RCLCPP_INFO(logger_, "Starting Controller...");
  arduino_.sendEmptyMsg();
  arduino_.setPidValues(30, 20, 0, 100);
  arduino_.setOffloaderValue(static_cast<int>(offloader_cmd_));
  return CallbackReturn::SUCCESS;
}

// ---------------------------------------------
CallbackReturn DiffDriveArduino::on_deactivate(const rclcpp_lifecycle::State &)
{
  RCLCPP_INFO(logger_, "Stopping Controller...");

  // Stop background spin thread
  if (spinning_) {
    spinning_ = false;
    if (spin_thread_.joinable())
      spin_thread_.join();
  }

  return CallbackReturn::SUCCESS;
}

// ---------------------------------------------
return_type DiffDriveArduino::read(const rclcpp::Time &, const rclcpp::Duration &)
{
  auto new_time = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = new_time - time_;
  double deltaSeconds = diff.count();
  time_ = new_time;

  if (!arduino_.connected())
    return return_type::ERROR;

  arduino_.readEncoderValues(l_wheel_.enc, r_wheel_.enc);

  double pos_prev = l_wheel_.pos;
  l_wheel_.pos = l_wheel_.calcEncAngle();
  l_wheel_.vel = (l_wheel_.pos - pos_prev) / deltaSeconds;

  pos_prev = r_wheel_.pos;
  r_wheel_.pos = r_wheel_.calcEncAngle();
  r_wheel_.vel = (r_wheel_.pos - pos_prev) / deltaSeconds;

  double ax, ay, az, gx, gy, gz, qx, qy, qz, qw;
  arduino_.readImuValues(ax, ay, az, gx, gy, gz, qx, qy, qz, qw);

  imu_ax_ = ax * 9.80665;
  imu_ay_ = ay * 9.80665;
  imu_az_ = az * 9.80665;
  imu_gx_ = gx * M_PI / 180.0;
  imu_gy_ = gy * M_PI / 180.0;
  imu_gz_ = gz * M_PI / 180.0;

  imu_orientation_[0] = qy;
  imu_orientation_[1] = qx;
  imu_orientation_[2] = qz;
  imu_orientation_[3] = qw;

  return return_type::OK;
}

// ---------------------------------------------
return_type DiffDriveArduino::write(const rclcpp::Time &, const rclcpp::Duration &)
{
  if (!arduino_.connected())
    return return_type::ERROR;

  arduino_.setMotorValues(
    l_wheel_.cmd / l_wheel_.rads_per_count / cfg_.loop_rate,
    r_wheel_.cmd / r_wheel_.rads_per_count / cfg_.loop_rate);

  return return_type::OK;
}

// ------------------- Relay Callbacks -------------------
void DiffDriveArduino::pumpCallback(const std_msgs::msg::Bool::SharedPtr msg)
{
  if (!arduino_.connected()) {
    RCLCPP_WARN(logger_, "Pump callback: Arduino not connected, ignoring");
    return;
  }
  try {
    if (msg->data)
      arduino_.setRelay('f');  // ON
    else
      arduino_.setRelay('g');  // OFF
  } catch (const std::exception &e) {
    RCLCPP_WARN(logger_, "Pump callback exception: %s", e.what());
  }
}

void DiffDriveArduino::vacuumCallback(const std_msgs::msg::Bool::SharedPtr msg)
{
  if (!arduino_.connected()) {
    RCLCPP_WARN(logger_, "Vacuum callback: Arduino not connected, ignoring");
    return;
  }
  try {
    if (msg->data)
      arduino_.setRelay('h');  // ON
    else
      arduino_.setRelay('j');  // OFF
  } catch (const std::exception &e) {
    RCLCPP_WARN(logger_, "Vacuum callback exception: %s", e.what());
  }
}
// -------------------------------------------------------

} // namespace diffdrive_arduino

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(diffdrive_arduino::DiffDriveArduino, hardware_interface::SystemInterface)
