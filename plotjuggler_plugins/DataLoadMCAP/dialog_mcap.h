#ifndef DIALOG_MCAP_H
#define DIALOG_MCAP_H

#include <QDialog>

#include "mcap/reader.hpp"
#include "dataload_params.h"

namespace Ui
{
class dialog_mcap;
}

class DialogMCAP : public QDialog
{
  Q_OBJECT

public:
  explicit DialogMCAP(const std::unordered_map<int, mcap::ChannelPtr>& channels,
                      const std::unordered_map<int, mcap::SchemaPtr>& schemas,
                      std::optional<mcap::LoadParams> default_parameters,
                      QWidget* parent = nullptr);
  ~DialogMCAP();

  mcap::LoadParams getParams() const;

private slots:
  void on_tableWidget_itemSelectionChanged();
  void accept() override;

private:
  Ui::dialog_mcap* ui;

  static const QString prefix;
};

#endif  // DIALOG_MCAP_H
