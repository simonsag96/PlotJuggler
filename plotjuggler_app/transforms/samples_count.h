#pragma once

#include <QWidget>
#include "PlotJuggler/transform_function.h"

using namespace PJ;

namespace Ui
{
class SamplesCount;
}

class SamplesCountFilter : public TransformFunction_SISO
{
public:
  explicit SamplesCountFilter();

  ~SamplesCountFilter() override;

  static const char* transformName()
  {
    return "Samples Counter";
  }

  const char* name() const override
  {
    return transformName();
  }

  QWidget* optionsWidget() override;

  bool xmlSaveState(QDomDocument& doc, QDomElement& parent_element) const override;

  bool xmlLoadState(const QDomElement& parent_element) override;

private:
  Ui::SamplesCount* ui;
  QWidget* _widget;

  int count_ = 0;
  double interval_end_ = 0;

  std::optional<PlotData::Point> calculateNextPoint(size_t index) override;
};
