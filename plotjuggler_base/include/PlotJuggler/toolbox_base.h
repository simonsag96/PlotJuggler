/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef PJ_TOOLBOX_BASE_H
#define PJ_TOOLBOX_BASE_H

#include <QAction>

#include <functional>
#include "PlotJuggler/plotdata.h"
#include "PlotJuggler/pj_plugin.h"
#include "PlotJuggler/transform_function.h"
#include "PlotJuggler/messageparser_base.h"

namespace PJ
{
class ToolboxPlugin : public PlotJugglerPlugin
{
  Q_OBJECT

public:
  ToolboxPlugin() = default;

  virtual void init(PlotDataMapRef& src_data, TransformsMap& transform_map) = 0;

  virtual ~ToolboxPlugin() = default;

  enum WidgetType
  {
    FLOATING,
    FIXED
  };

  virtual std::pair<QWidget*, WidgetType> providedWidget() const = 0;

  void setParserFactories(ParserFactories* parsers)
  {
    _parser_factories = parsers;
  }

  const ParserFactories* parserFactories() const
  {
    return _parser_factories;
  }

public slots:

  virtual bool onShowWidget() = 0;

signals:

  void plotCreated(std::string plot_name, bool is_custom = true);

  void importData(PlotDataMapRef& new_data, bool remove_old);

  void closed();

private:
  ParserFactories* _parser_factories = nullptr;
};

using ToolboxPluginPtr = std::shared_ptr<ToolboxPlugin>;

}  // namespace PJ

QT_BEGIN_NAMESPACE
#define Toolbox_iid "facontidavide.PlotJuggler3.Toolbox"
Q_DECLARE_INTERFACE(PJ::ToolboxPlugin, Toolbox_iid)
QT_END_NAMESPACE

#endif
