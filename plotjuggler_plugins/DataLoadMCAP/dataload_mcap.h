#pragma once

#include <optional>
#include <QObject>
#include <QtPlugin>
#include <QStandardItemModel>
#include "PlotJuggler/dataloader_base.h"
#include "dataload_params.h"

using namespace PJ;

class DataLoadMCAP : public DataLoader
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "facontidavide.PlotJuggler3.DataLoader")
  Q_INTERFACES(PJ::DataLoader)

public:
  DataLoadMCAP();

  virtual const std::vector<const char*>& compatibleFileExtensions() const override;

  virtual bool readDataFromFile(PJ::FileLoadInfo* fileload_info,
                                PlotDataMapRef& destination) override;

  virtual ~DataLoadMCAP() override;

  virtual const char* name() const override
  {
    return "DataLoad MCAP";
  }

  bool xmlSaveState(QDomDocument& doc, QDomElement& parent_element) const override;

  bool xmlLoadState(const QDomElement& parent_element) override;

private:
  std::optional<mcap::LoadParams> _dialog_parameters;
};
