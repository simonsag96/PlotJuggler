#include "time_since_last_datapoint.h"
#include <QFormLayout>
#include <QDoubleValidator>

std::optional<PlotData::Point> TimeSinceLastDataPointTransform::calculateNextPoint(size_t index)
{
  if (index == 0)
  {
    return {};
  }

  const auto& prev = dataSource()->at(index - 1);
  const auto& p = dataSource()->at(index);

  double dt = (p.x - prev.x);

  PlotData::Point out = { p.x, dt };
  return out;
}
