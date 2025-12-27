/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "toast_notification.h"

#include <QHBoxLayout>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>

ToastNotification::ToastNotification(const QString& message, const QPixmap& icon, int timeout_ms,
                                     QWidget* parent)
  : QFrame(parent)
  , _icon_label(nullptr)
  , _message_label(nullptr)
  , _close_button(nullptr)
  , _slide_animation(nullptr)
  , _timeout_timer(nullptr)
  , _timeout_ms(timeout_ms)
  , _is_closing(false)
{
  setObjectName("ToastNotification");
  setupUI();
  setupAnimation();

  setMessage(message);
  if (!icon.isNull())
  {
    setIcon(icon);
  }
}

ToastNotification::~ToastNotification()
{
  if (_slide_animation)
  {
    _slide_animation->stop();
  }
  if (_timeout_timer)
  {
    _timeout_timer->stop();
  }
}

void ToastNotification::setupUI()
{
  // Frame styling
  setFrameShape(QFrame::StyledPanel);
  setFrameShadow(QFrame::Raised);
  setMinimumHeight(ICON_SIZE + 16);  // icon + padding
  setMaximumWidth(400);

  // Main horizontal layout
  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);
  layout->setSpacing(12);

  // Icon label (optional, hidden by default)
  _icon_label = new QLabel(this);
  _icon_label->setObjectName("toastIcon");
  _icon_label->setFixedSize(ICON_SIZE, ICON_SIZE);
  _icon_label->setScaledContents(true);
  _icon_label->setVisible(false);
  layout->addWidget(_icon_label);

  // Message label (supports HTML/rich text with clickable links)
  _message_label = new QLabel(this);
  _message_label->setObjectName("toastMessage");
  _message_label->setWordWrap(true);
  _message_label->setTextFormat(Qt::RichText);
  _message_label->setOpenExternalLinks(true);
  _message_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  layout->addWidget(_message_label, 1);

  // Close button
  _close_button = new QPushButton(this);
  _close_button->setObjectName("toastCloseButton");
  _close_button->setText("\u00D7");  // Unicode multiplication sign (×)
  _close_button->setFixedSize(24, 24);
  _close_button->setFlat(true);
  _close_button->setCursor(Qt::PointingHandCursor);
  _close_button->setFocusPolicy(Qt::NoFocus);
  layout->addWidget(_close_button, 0, Qt::AlignTop);

  connect(_close_button, &QPushButton::clicked, this, &ToastNotification::onCloseClicked);

  // Timeout timer
  if (_timeout_ms > 0)
  {
    _timeout_timer = new QTimer(this);
    _timeout_timer->setSingleShot(true);
    connect(_timeout_timer, &QTimer::timeout, this, &ToastNotification::onTimeout);
  }
}

void ToastNotification::setupAnimation()
{
  _slide_animation = new QPropertyAnimation(this, "geometry", this);
  _slide_animation->setDuration(ANIMATION_DURATION_MS);
  _slide_animation->setEasingCurve(QEasingCurve::OutCubic);

  connect(_slide_animation, &QPropertyAnimation::finished, this,
          &ToastNotification::onAnimationFinished);
}

void ToastNotification::showAnimated()
{
  // Ensure widget is visible but positioned off-screen
  show();

  // Calculate animation positions
  QRect endRect = geometry();
  QRect startRect = endRect;
  startRect.moveLeft(endRect.right());  // Start from off-screen right

  _slide_animation->setStartValue(startRect);
  _slide_animation->setEndValue(endRect);
  _slide_animation->setDirection(QAbstractAnimation::Forward);
  _slide_animation->start();

  // Start timeout timer after animation completes
  if (_timeout_timer && _timeout_ms > 0)
  {
    QTimer::singleShot(ANIMATION_DURATION_MS, this, [this]() {
      if (_timeout_timer && !_is_closing)
      {
        _timeout_timer->start(_timeout_ms);
      }
    });
  }
}

void ToastNotification::hideAnimated()
{
  if (_is_closing)
  {
    return;
  }
  _is_closing = true;

  // Stop timeout timer
  if (_timeout_timer)
  {
    _timeout_timer->stop();
  }

  // Animate sliding out to the right
  QRect startRect = geometry();
  QRect endRect = startRect;
  endRect.moveLeft(startRect.right());  // Move off-screen right

  _slide_animation->setStartValue(startRect);
  _slide_animation->setEndValue(endRect);
  _slide_animation->setDirection(QAbstractAnimation::Forward);
  _slide_animation->start();
}

QString ToastNotification::message() const
{
  return _message_label->text();
}

void ToastNotification::setMessage(const QString& message)
{
  _message_label->setText(message);
}

void ToastNotification::setIcon(const QPixmap& icon)
{
  if (icon.isNull())
  {
    _icon_label->setVisible(false);
  }
  else
  {
    _icon_label->setPixmap(
        icon.scaled(ICON_SIZE, ICON_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    _icon_label->setVisible(true);
  }
}

void ToastNotification::showEvent(QShowEvent* event)
{
  QFrame::showEvent(event);
}

void ToastNotification::onCloseClicked()
{
  hideAnimated();
}

void ToastNotification::onAnimationFinished()
{
  if (_is_closing)
  {
    emit closed();
    deleteLater();
  }
}

void ToastNotification::onTimeout()
{
  hideAnimated();
}
