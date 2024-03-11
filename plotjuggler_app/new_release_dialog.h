/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef NEW_RELEASE_DIALOG_H
#define NEW_RELEASE_DIALOG_H

#include <QDialog>

namespace Ui
{
class NewReleaseDialog;
}

class NewReleaseDialog : public QDialog
{
  Q_OBJECT

public:
  NewReleaseDialog(QWidget* parent, QString release, QString title, QString url);
  ~NewReleaseDialog();

  bool link_opened = false;

private:
  Ui::NewReleaseDialog* ui;
};

#endif  // NEW_RELEASE_DIALOG_H
