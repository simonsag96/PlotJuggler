#ifndef ROS_PARSER_H
#define ROS_PARSER_H

#include "PlotJuggler/contrib/fmt/core.h"
#include "PlotJuggler/messageparser_base.h"
#include "rosx_introspection/ros_parser.hpp"
#include "special_messages.h"
#include <unordered_map>
#include <utility>
#include <vector>
#include <string>

class DebugConfigStorage
{
public:
  static DebugConfigStorage& getInstance()
  {
    static DebugConfigStorage instance;
    return instance;
  }
  inline bool has_definition(std::string debug_topic) const
  {
    return debug_definitions_.count(debug_topic) > 0;
  }
  inline void register_definition(std::string debug_topic,
                                  std::vector<std::string>&& debug_signal_names)
  {
    debug_definitions_[debug_topic] = std::move(debug_signal_names);
  }
  inline void reset_definition(std::string debug_topic)
  {
    if (has_definition(debug_topic))
    {
      debug_definitions_.erase(debug_topic);
    }
  }
  inline std::vector<std::string> const get_definition(std::string debug_topic) const
  {
    return debug_definitions_.at(debug_topic);
  }

private:
  std::unordered_map<std::string, std::vector<std::string>> debug_definitions_;
  DebugConfigStorage()
  {
  }
  DebugConfigStorage(DebugConfigStorage const&);
  void operator=(DebugConfigStorage const&);
};

class ParserROS : public PJ::MessageParser
{
public:
  ParserROS(const std::string& topic_name, const std::string& type_name,
            const std::string& schema, RosMsgParser::Deserializer* deserializer,
            PJ::PlotDataMapRef& data);

  bool parseMessage(const PJ::MessageRef serialized_msg, double& timestamp) override;

  void setLargeArraysPolicy(bool clamp, unsigned max_size) override;

  void enableTruncationCheck(bool enable)
  {
    _strict_truncation_check = enable;
  }

protected:
  RosMsgParser::Parser _parser;
  std::unique_ptr<RosMsgParser::Deserializer> _deserializer;
  RosMsgParser::FlatMessage _flat_msg;
  std::string _topic;

  PJ::Msg::Header readHeader(double& timestamp);
  void parseHeader(const std::string& prefix, double& timestamp);
  void parseEmpty(const std::string& prefix, double& timestamp);

  template <size_t N>
  void parseCovariance(const std::string& prefix, double& timestamp);

  void parseVector3(const std::string& prefix, double& timestamp);
  void parsePoint(const std::string& prefix, double& timestamp);
  void parseQuaternion(const std::string& prefix, double& timestamp);

  void parseTwist(const std::string& prefix, double& timestamp);
  void parseTwistWithCovariance(const std::string& prefix, double& timestamp);

  void parseTransform(const std::string& prefix, double& timestamp);
  void parseTransformStamped(const std::string& prefix, double& timestamp);

  void parsePose(const std::string& prefix, double& timestamp);
  void parsePoseStamped(const std::string& prefix, double& timestamp);
  void parsePoseWithCovariance(const std::string& prefix, double& timestamp);

  void parseImu(const std::string& prefix, double& timestamp);
  void parseOdometry(const std::string& prefix, double& timestamp);

  void parseDiagnosticMsg(const std::string& prefix, double& timestamp);
  void parseJointStateMsg(const std::string& prefix, double& timestamp);
  void parseTF2Msg(const std::string& prefix, double& timestamp);

  void parseDataTamerSchemas(const std::string& prefix, double& timestamp);
  void parseDataTamerSnapshot(const std::string& prefix, double& timestamp);

  void parsePalStatisticsNames(const std::string& prefix, double& timestamp);
  void parsePalStatisticsValues(const std::string& prefix, double& timestamp);

  void parseTUMDebugSignalNames(const std::string& _, double& timestamp);
  void parseTUMDebugValues(const std::string& _, double& timestamp);

  void parseTSLDefinition(const std::string& prefix, double& timestamp);
  void parseTSLValues(const std::string& prefix, double& timestamp);

  // Prop

  std::function<void(const std::string& prefix, double&)> _customized_parser;

  bool _has_header = false;
  bool _strict_truncation_check = true;

private:
  // TUM Debug stuff
  DebugConfigStorage& _debug_config_storage = DebugConfigStorage::getInstance();
  std::string _debug_channel_name;
  std::vector<std::pair<double, std::vector<double>>> _values_buffer;
  std::vector<PJ::PlotData*> _series_cache;
  bool _initialized = false;
};

template <size_t N>
inline void ParserROS::parseCovariance(const std::string& prefix, double& timestamp)
{
  std::array<double, N * N> cov;
  for (auto& val : cov)
  {
    val = _deserializer->deserialize(RosMsgParser::FLOAT64).convert<double>();
  }
  for (int i = 0; i < N; i++)
  {
    for (int j = i; j < N; j++)
    {
      const size_t index = i * N + j;
      getSeries(fmt::format("{}/[{};{}]", prefix, i, j))
          .pushBack({ timestamp, cov[index] });
    }
  }
}
#endif  // ROS_PARSER_H
