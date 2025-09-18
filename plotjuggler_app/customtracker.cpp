/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "customtracker.h"
#include "qwt_series_data.h"
#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "qwt_scale_map.h"
#include "qwt_symbol.h"
#include "qwt_text.h"
#include <qevent.h>
#include <QFontDatabase>
#include <QSettings>

struct compareX
{
  inline bool operator()(const double x, const QPointF& pos) const
  {
    return (x < pos.x());
  }
};

CurveTracker::CurveTracker(QwtPlot* plot) : QObject(plot), _plot(plot), _param(VALUE)
{
  _line_marker = new QwtPlotMarker();

  _line_marker->setLinePen(QPen(Qt::red));
  _line_marker->setLineStyle(QwtPlotMarker::VLine);
  _line_marker->setValue(0, 0);
  _line_marker->attach(plot);

  _text_marker = new QwtPlotMarker();
  _text_marker->attach(plot);

  _visible = true;
}

CurveTracker::~CurveTracker()
{
}

QPointF CurveTracker::actualPosition() const
{
  return _prev_trackerpoint;
}

void CurveTracker::setParameter(Parameter par)
{
  bool changed = _param != par;
  _param = par;

  if (changed)
  {
    setPosition(_prev_trackerpoint);
  }
}

void CurveTracker::setEnabled(bool enable)
{
  _visible = enable;
  _line_marker->setVisible(enable);
  _text_marker->setVisible(enable);

  for (int i = 0; i < _marker.size(); i++)
  {
    _marker[i]->setVisible(enable);
  }
}

bool CurveTracker::isEnabled() const
{
  return _visible;
}

void CurveTracker::setPosition(const QPointF& tracker_position)
{
  const QwtPlotItemList curves = _plot->itemList(QwtPlotItem::Rtti_PlotCurve);

  _line_marker->setValue(tracker_position);

  QRectF rect;
  rect.setBottom(_plot->canvasMap(QwtPlot::yLeft).s1());
  rect.setTop(_plot->canvasMap(QwtPlot::yLeft).s2());
  rect.setLeft(_plot->canvasMap(QwtPlot::xBottom).s1());
  rect.setRight(_plot->canvasMap(QwtPlot::xBottom).s2());

  double min_Y = std::numeric_limits<double>::max();
  double max_Y = -min_Y;
  int visible_points = 0;

  while (_marker.size() > curves.size())
  {
    _marker.back()->detach();
    _marker.pop_back();
  }

  for (int i = _marker.size(); i < curves.size(); i++)
  {
    _marker.push_back(new QwtPlotMarker);
    _marker[i]->attach(_plot);
  }

  double text_X_offset = 0;

  struct LineParts
  {
    QColor color;
    QString value;
    QString delta;
    QString name;
  };

  std::multimap<double, LineParts> text_lines;

  QSettings settings;
  const int prec = settings.value("Preferences::precision", 3).toInt();

  int values_char_count = 0;
  int delta_char_count = 0;

  for (int i = 0; i < curves.size(); i++)
  {
    QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(curves[i]);
    _marker[i]->setVisible(curve->isVisible());

    if (curve->isVisible() == false)
    {
      continue;
    }
    QColor color = curve->pen().color();
    text_X_offset = rect.width() * 0.02;
    if (!_marker[i]->symbol() || _marker[i]->symbol()->brush().color() != color)
    {
      QwtSymbol* sym = new QwtSymbol(QwtSymbol::Ellipse, color, QPen(Qt::black), QSize(5, 5));
      _marker[i]->setSymbol(sym);
    }
    const auto maybe_point = curvePointAt(curve, tracker_position.x());
    if (!maybe_point)
    {
      continue;
    }
    const std::optional<QPointF> maybe_reference =
        (_reference_pos) ? curvePointAt(curve, _reference_pos->x()) : std::nullopt;

    const QPointF point = maybe_point.value();
    _marker[i]->setValue(point);
    if (rect.contains(point) && _visible)
    {
      min_Y = std::min(min_Y, point.y());
      max_Y = std::max(max_Y, point.y());

      visible_points++;
      double value = point.y();
      LineParts parts;
      parts.value = QString::number(value, 'f', prec);
      if (maybe_reference)
      {
        auto delta_str = QString::number(value - maybe_reference->y(), 'f', prec);
        parts.delta = QString(" (Δ %1)").arg(delta_str);
      }
      parts.name = curve->title().text();
      parts.color = color;
      text_lines.insert(std::make_pair(value, parts));
      _marker[i]->setVisible(true);

      values_char_count = std::max(values_char_count, parts.value.length());
      delta_char_count = std::max(delta_char_count, parts.delta.length());
    }
    else
    {
      _marker[i]->setVisible(false);
    }
    _marker[i]->setValue(point);
  }

  // add indentation to align the columns
  for (auto& [_, parts] : text_lines)
  {
    while (parts.value.length() < values_char_count)
    {
      parts.value.prepend("&nbsp;");
    }
    while (parts.delta.length() < delta_char_count)
    {
      parts.delta.prepend("&nbsp;");
    }
  }

  QwtText mark_text;
  QString text_marker_info;

  const QString time_str = QString::number(tracker_position.x(), 'f', prec);
  if (_reference_pos)
  {
    auto delta_time = tracker_position.x() - _reference_pos->x();
    auto delta_str = QString::number(delta_time, 'f', prec);
    text_marker_info = QString("time : %1 (Δ %2)<br>").arg(time_str).arg(delta_str);
  }
  else
  {
    text_marker_info = QString("time : %1<br>").arg(time_str);
  }

  if (_param != LINE_ONLY)
  {
    int count = 0;
    for (auto it = text_lines.rbegin(); it != text_lines.rend(); it++)
    {
      LineParts parts = it->second;
      QString line_str;
      if (_param == VALUE)
      {
        line_str = QString("<font color=%1>%2%3</font>")
                       .arg(parts.color.name())
                       .arg(parts.value)
                       .arg(parts.delta);
      }
      else if (_param == VALUE_NAME)
      {
        line_str = QString("<font color=%1>%2%3 : %4</font>")
                       .arg(parts.color.name())
                       .arg(parts.value)
                       .arg(parts.delta)
                       .arg(parts.name);
      }

      text_marker_info += line_str;
      if (count++ < text_lines.size() - 1)
      {
        text_marker_info += "<br>";
      }
    }
    mark_text.setBorderPen(QColor(Qt::transparent));

    QColor background_color = _plot->palette().background().color();
    background_color.setAlpha(180);
    mark_text.setBackgroundBrush(background_color);
    mark_text.setText(text_marker_info);

    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(9);
    mark_text.setFont(font);
    mark_text.setRenderFlags(_param == VALUE ? Qt::AlignCenter : Qt::AlignLeft);
    _text_marker->setLabel(mark_text);
    _text_marker->setLabelAlignment(Qt::AlignRight);
    _text_marker->setXValue(tracker_position.x() + text_X_offset);
  }

  if (visible_points > 0)
  {
    _text_marker->setYValue(0.5 * (max_Y + min_Y));
  }

  double canvas_ratio = rect.width() / double(_plot->width());
  double text_width = mark_text.textSize().width() * canvas_ratio;
  bool exceed_right = (_text_marker->boundingRect().right() + text_width) > rect.right();

  if (exceed_right)
  {
    _text_marker->setXValue(tracker_position.x() - text_X_offset - text_width);
  }

  _text_marker->setVisible(visible_points > 0 && _visible && _param != LINE_ONLY);

  _prev_trackerpoint = tracker_position;
}

void CurveTracker::setReferencePosition(std::optional<QPointF> reference_pos)
{
  _reference_pos = reference_pos;
  redraw();
}

std::optional<QPointF> curvePointAt(const QwtPlotCurve* curve, double x)
{
  if (curve->dataSize() >= 2)
  {
    int index = qwtUpperSampleIndex<QPointF>(*curve->data(), x, compareX());

    if (index > 0)
    {
      auto p1 = (curve->sample(index - 1));
      auto p2 = (curve->sample(index));
      double middle_X = (p1.x() + p2.x()) / 2.0;
      return (x < middle_X) ? p1 : p2;
    }
  }
  return std::nullopt;
}
