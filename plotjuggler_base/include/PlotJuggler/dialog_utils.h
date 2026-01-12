#ifndef PJ_DIALOG_UTILS_H
#define PJ_DIALOG_UTILS_H

#include <QDialog>
#include <set>

namespace PJ
{

/**
 * @brief Adjusts dialog size to fit contents after widget visibility changes
 *
 * This function resets dialog size constraints and forces the layout system
 * to recalculate the optimal size. Useful when showing/hiding option widgets
 * in plugin dialogs.
 *
 * @param dialog The dialog to resize
 */
inline void adjustDialogToContent(QDialog* dialog, const QString& selected_protocol)
{
  if (!dialog)
  {
    return;
  }
  // Reset minimum height and resize dialog
  dialog->setMinimumHeight(0);
  dialog->layout()->invalidate();
  dialog->layout()->activate();

  QSize hint;
  if (std::set<QString>{ "ros1msg", "ros2msg", "data_tamer", "Influx (Line protocol)" }.count(
          selected_protocol))
  {
    hint = QSize(316, 341);
  }
  else
  {
    hint = dialog->sizeHint();
  }
  // Force recalculation of size hint
  dialog->setMaximumHeight(hint.height());
  dialog->resize(hint);
}

}  // namespace PJ

#endif  // PJ_DIALOG_UTILS_H
