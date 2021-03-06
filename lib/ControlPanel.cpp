/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "ControlPanel.h"
#include "Global.h"

#include <QDebug>
#include <QLayout>
#include <QSettings>
#include <QToolBar>
#include <QShortcut>

ControlPanel::ControlPanel ()
{
  _lockStatus = TRUE;
  _startFlag = FALSE;
  createGUI();
  setMaximumHeight(75);
  setMaximumWidth(200);
}

void ControlPanel::createGUI ()
{
  int length = 16;
  int width = 16;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(0);
  vbox->setMargin(0);
  setLayout(vbox);

  // chart settings toolbar
  QToolBar *tb = new QToolBar;
  tb->setIconSize(QSize(length, width));
  vbox->addWidget(tb);

  // grid button
  _gridButton = new GridButton;
  tb->addWidget(_gridButton);

  // toggle crosshairs button
  _crossHairsButton = new CrossHairsButton;
  tb->addWidget(_crossHairsButton);

  // data window button
  _dataWindowButton = new DataWindowButton;
  tb->addWidget(_dataWindowButton);

  // configure button
  _configureButton = new ConfigureButton;
  tb->addWidget(_configureButton);

  // chart toggles toolbar
  tb = new QToolBar;
  tb->setIconSize(QSize(length, width));
  vbox->addWidget(tb);

  // create bar length button
  _barLengthButton = new BarLengthButton;
  tb->addWidget(_barLengthButton);

  // date range controls
  _dateRangeControl = new DateRangeControl;
  tb->addWidget(_dateRangeControl);

  // create the bar space button
  _barSpaceButton = new BarSpaceButton;
  tb->addWidget(_barSpaceButton);

  // create recent charts combobox
  _recentCharts = new RecentCharts;
  tb->addWidget(_recentCharts);

  // slider
  _slider = new QSlider;
  _slider->setRange(0, 0);
  _slider->setValue(0);
  _slider->setOrientation(Qt::Horizontal);
  _slider->setToolTip(tr("Scroll Chart"));
  _slider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
  connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
  vbox->addWidget(_slider);

  // create shortcut key for the slider
  QShortcut *sc = new QShortcut(this);
  sc->setKey(QKeySequence(Qt::CTRL + Qt::Key_L));
  connect(sc, SIGNAL(activated()), _slider, SLOT(setFocus()));
}

BarSpaceButton * ControlPanel::barSpaceButton ()
{
  return _barSpaceButton;
}

BarLengthButton * ControlPanel::barLengthButton ()
{
  return _barLengthButton;
}

DateRangeControl * ControlPanel::dateRangeControl ()
{
  return _dateRangeControl;
}

RecentCharts * ControlPanel::recentCharts ()
{
  return _recentCharts;
}

ConfigureButton * ControlPanel::configureButton ()
{
  return _configureButton;
}

QSlider * ControlPanel::slider ()
{
  return _slider;
}

void ControlPanel::setStart (int count, int, int)
{
  _slider->blockSignals(TRUE);
  _slider->setRange(0, count - 1);
  _slider->blockSignals(FALSE);
  _startFlag = FALSE;
}

int ControlPanel::getValue ()
{
  return _slider->value();
}

void ControlPanel::setValue (int d)
{
  _slider->setValue(d);
}

void ControlPanel::setStartValue (int d)
{
  if (_startFlag)
    return;

  _slider->blockSignals(TRUE);
  _slider->setValue(d);
  _slider->blockSignals(FALSE);

  _startFlag = TRUE;
//qDebug() << "ControlPanel::setStartValue" << d;
}

void ControlPanel::sliderChanged (int d)
{
  emit signalValueChanged (d);
}

void ControlPanel::loadSettings ()
{
  QSettings settings(g_localSettings);
  _lockStatus = settings.value("plot_slider_lock_status", TRUE).toBool();
  emit signalLockStatus(_lockStatus);
}

void ControlPanel::saveSettings ()
{
  QSettings settings(g_localSettings);
  settings.setValue("plot_slider_lock_status", _lockStatus);
}

void ControlPanel::setLockStatus (bool status)
{
  _lockStatus = status;
}

void ControlPanel::pageUp ()
{
  int d = _slider->value() + 50;
  if (d > _slider->maximum())
    d = _slider->maximum();
  _slider->setValue(d);
}

void ControlPanel::pageDown ()
{
  int d = _slider->value() - 50;
  if (d < 0)
    d = 0;
  _slider->setValue(d);
}
