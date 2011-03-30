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

#include "ChartObjectVLineDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>

ChartObjectVLineDialog::ChartObjectVLineDialog (QWidget *p, ChartObject *set) : Dialog (p)
{
  _co = set;
  _keySize = "chart_object_vline_dialog_window_size";
  _keyPos = "chart_object_vline_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit VLine");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  loadObject();
}

void ChartObjectVLineDialog::createGUI ()
{
  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  _form->addRow(tr("Date"), _date);

  // color
  _color = new ColorButton(this, QColor(Qt::red));
  _form->addRow(tr("Color"), _color);
  
  // default
  _default = new QCheckBox;
  _form->addRow(tr("Set as default"), _default);
}

void ChartObjectVLineDialog::done ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_globalSettings);
    settings.setValue("default_chart_object_vline_color", _color->color().name());
    settings.sync();
  }

  Setting *set = _co->settings();
  set->setData("COLOR", _color->color());
  set->setData("DATE", _date->dateTime());

  _co->save();

  saveSettings();

  accept();
}

void ChartObjectVLineDialog::loadObject ()
{
  Setting *set = _co->settings();
  _date->setDateTime(set->dateTime("DATE"));
  _color->setColor(set->color("COLOR"));
}

void ChartObjectVLineDialog::loadSettings ()
{
  Dialog::loadSettings();

  QSettings settings(g_globalSettings);

  QColor c(settings.value("default_chart_object_vline_color", "red").toString());
  _color->setColor(c);
}
