#ifndef SPECIAL_MESSAGES_H
#define SPECIAL_MESSAGES_H

// These messages are exact equivalents of ROS messages

#include <array>
#include <vector>
#include <string>
#include <cstdint>

namespace PJ::Msg
{

struct Time
{
  uint32_t sec;
  uint32_t nanosec;

  double toSec() const
  {
    return double(sec) + double(nanosec) * 1e-9;
  }
};

struct Header
{
  uint32_t seq;
  PJ::Msg::Time stamp;
  std::string frame_id;
};

struct Empty
{
  static const char* id()
  {
    return "std_msgs/Empty";
  }
};

//--------------------
struct DiagnosticStatus
{
  uint8_t level;
  std::string name;
  std::string message;
  std::string hardware_id;
  std::vector<std::pair<std::string, std::string>> key_value;

  static const char* id()
  {
    return "diagnostic_msgs/DiagnosticStatus";
  }
};

struct DiagnosticArray
{
  Header header;
  std::vector<DiagnosticStatus> status;

  static const char* id()
  {
    return "diagnostic_msgs/DiagnosticArray";
  }
};

//--------------------
struct Vector3
{
  double x;
  double y;
  double z;

  static const char* id()
  {
    return "geometry_msgs/Vector3";
  }
};

struct Point
{
  double x;
  double y;
  double z;

  static const char* id()
  {
    return "geometry_msgs/Point";
  }
};

struct Quaternion
{
  double x;
  double y;
  double z;
  double w;

  static const char* id()
  {
    return "geometry_msgs/Quaternion";
  }
};

struct RPY
{
  double roll;
  double pitch;
  double yaw;
};

RPY QuaternionToRPY(Quaternion q);

struct Transform
{
  Point translation;
  Quaternion rotation;

  static const char* id()
  {
    return "geometry_msgs/Transform";
  }
};

struct TransformStamped
{
  Header header;
  std::string child_frame_id;
  Transform transform;

  static const char* id()
  {
    return "geometry_msgs/TransformStamped";
  }
};

struct Pose
{
  Vector3 position;
  Quaternion orientation;

  static const char* id()
  {
    return "geometry_msgs/Pose";
  }
};

struct PoseStamped
{
  Header header;
  Pose pose;

  static const char* id()
  {
    return "geometry_msgs/PoseStamped";
  }
};

struct PoseWithCovariance
{
  Pose pose;
  std::array<double, 36> covariance;

  static const char* id()
  {
    return "geometry_msgs/PoseWithCovariance";
  }
};

struct Twist
{
  Vector3 linear;
  Vector3 angular;

  static const char* id()
  {
    return "geometry_msgs/Twist";
  }
};

struct TwistWithCovariance
{
  Twist twist;
  std::array<double, 36> covariance;

  static const char* id()
  {
    return "geometry_msgs/TwistWithCovariance";
  }
};

struct TFMessage
{
  std::vector<TransformStamped> transforms;

  static const char* id()
  {
    return "tf2_msgs/TFMessage";
  }
};
//--------------------

struct Imu
{
  Header header;
  Quaternion orientation;
  std::array<double, 9> orientation_covariance;
  Vector3 angular_velocity;
  std::array<double, 9> angular_velocity_covariance;
  Vector3 linear_acceleration;
  std::array<double, 9> linear_acceleration_covariance;

  static const char* id()
  {
    return "sensor_msgs/Imu";
  }
};
//--------------------
struct Odometry
{
  Header header;
  PoseWithCovariance pose;
  TwistWithCovariance twist;

  static const char* id()
  {
    return "nav_msgs/Odometry";
  }
};

//--------------------

struct JointState
{
  Header header;
  std::vector<std::string> name;
  std::vector<double> position;
  std::vector<double> velocity;
  std::vector<double> effort;

  static const char* id()
  {
    return "sensor_msgs/JointState";
  }
};

//--------------------

struct DataTamerSchemas
{
  // no need to save any additional information

  static const char* id()
  {
    return "data_tamer_msgs/Schemas";
  }
};

struct DataTamerSnapshot
{
  std::string prefix;
  uint64_t timestamp_nsec;
  uint64_t schema_hash;
  std::vector<uint8_t> active_mask;
  std::vector<uint8_t> payload;

  static const char* id()
  {
    return "data_tamer_msgs/Snapshot";
  }
};

//--------------------

struct PalStatisticsNames
{
  Header header;
  std::vector<std::string> names;
  uint32_t names_version;

  static const char* id()
  {
    return "pal_statistics_msgs/StatisticsNames";
  }
};

struct PalStatisticsValues
{
  Header header;
  std::vector<double> names;
  uint32_t names_version;

  static const char* id()
  {
    return "pal_statistics_msgs/StatisticsValues";
  }
};
}  // namespace PJ::Msg

#endif  // SPECIAL_MESSAGES_H
