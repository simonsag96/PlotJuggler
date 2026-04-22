#pragma once

#include <QHash>
#include <QMap>
#include <QPointer>
#include <QTimer>
#include <QWebSocket>

#include "PlotJuggler/datastreamer_base.h"
#include "PlotJuggler/messageparser_base.h"
#include "foxglove_client_config.h"
#include "foxglove_dialog.h"

enum class FoxgloveState
{
  Disconnected,
  SelectingTopics,
  Streaming
};

/// PlotJuggler streaming plugin that subscribes to ROS 2 Foxglove bridge channels.
class FoxgloveBridgeClient : public PJ::DataStreamer
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "facontidavide.PlotJuggler3.DataStreamer")
  Q_INTERFACES(PJ::DataStreamer)

public:
  FoxgloveBridgeClient();
  ~FoxgloveBridgeClient() override
  {
    shutdown();
  }

  const std::vector<QAction*>& availableActions() override;
  bool start(QStringList*) override;
  void shutdown() override;
  bool isRunning() const override
  {
    return _running;
  }

  const char* name() const override
  {
    return "Foxglove ROS2 Bridge";
  }

  bool isDebugPlugin() override
  {
    return false;
  }

  bool xmlSaveState(QDomDocument& doc, QDomElement& parent_element) const override;
  bool xmlLoadState(const QDomElement& parent_element) override;

private:
  QAction* _action_pause = nullptr;
  std::vector<QAction*> _actions;

  FoxgloveClientConfig _config;

  QWebSocket _socket;
  QUrl _url;
  bool _running = false;
  bool _closing = false;
  bool _paused = false;
  FoxgloveState _state = FoxgloveState::Disconnected;

  QPointer<FoxgloveDialog> _dialog;

  QMap<quint64, FoxgloveChannelInfo> _channels;
  QHash<quint32, quint64> _subscriptions;
  QHash<quint64, quint32> _subscription_by_channel;

#ifdef PJ_BUILD
  QHash<quint32, PJ::MessageParserPtr> _parsers_by_subscription;
#endif

  quint32 _next_subscription_id = 1;

  QTimer _data_received_timer;
  bool _data_received_pending = false;

  void setupSettings();
  void saveDefaultSettings();
  void loadDefaultSettings();
  void updateOkButton();
  void resetState();

  /// Validate whether an advertised Foxglove channel is a ROS 2 CDR stream this plugin can parse.
  bool canUseChannel(const FoxgloveChannelInfo& channel) const;
  /// Create parsers and send the Foxglove subscribe request for the chosen channels.
  bool subscribeSelectedChannels(const std::vector<FoxgloveChannelInfo>& channels);
  void unsubscribeActiveSubscriptions();
  /// Send a control message if the socket is still connected.
  bool sendJsonMessage(const QJsonObject& obj);
  /// Coalesce high-rate incoming packets into fewer PlotJuggler updates.
  void flushPendingData();
  bool openDialogConnection();

private slots:
  void onPauseTriggered();
  void onDialogConnectToggled(bool checked);
  void onDialogAccepted();
  void onDialogRejected();
  void onConnected();
  void onDisconnected();
  void onError(QAbstractSocket::SocketError);
  void onTextMessageReceived(const QString& message);
  void onBinaryMessageReceived(const QByteArray& message);
};
