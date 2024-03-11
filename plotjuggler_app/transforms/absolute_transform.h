#ifndef ABSOLUTE_TRANSFORM_H
#define ABSOLUTE_TRANSFORM_H

#include "PlotJuggler/transform_function.h"

using namespace PJ;

class AbsoluteTransform : public TransformFunction_SISO
{
public:
  AbsoluteTransform() = default;

  ~AbsoluteTransform() override = default;

  static const char* transformName()
  {
    return "Absolute";
  }

  const char* name() const override
  {
    return transformName();
  }

private:
  std::optional<PlotData::Point> calculateNextPoint(size_t index) override;
};

#endif  // ABSOLUTE_TRANSFORM_H
