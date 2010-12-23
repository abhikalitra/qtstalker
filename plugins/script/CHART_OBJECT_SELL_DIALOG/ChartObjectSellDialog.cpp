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

#include "ChartObjectSellDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "Command.h"
#include "ScriptPluginFactory.h"

#include <QtDebug>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>

ChartObjectSellDialog::ChartObjectSellDialog (QString id)
{
  _id = id;
  _helpFile = "main.html";
  
  setWindowTitle("Qtstalker" + g_session + ": " + tr("Edit Sell"));

  createGUI();

  loadObject();

  loadSettings();
}

void ChartObjectSellDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  vbox->addLayout(form);

  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  form->addRow(tr("Date"), _date);

  // color
  QColor c(Qt::red);
  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);
  
  // price
  _price = new QDoubleSpinBox;
  _price->setRange(0.0, 99999999.0);
  form->addRow(tr("Price"), _price);

  // default
  _default = new QCheckBox;
  form->addRow(tr("Set as default"), _default);

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setDefault(TRUE);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void ChartObjectSellDialog::done ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_settingsFile);
    settings.setValue("default_chart_object_sell_color", _color->color().name());
    settings.sync();
  }

//  _settings.color = _color->color();
//  _settings.date = _date->dateTime();
//  _settings.price = _price->value();
  
  emit signalDone(_id);

  saveSettings();

  accept();
}

void ChartObjectSellDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ChartObjectSellDialog::cancel ()
{
  saveSettings();
  reject();
}

void ChartObjectSellDialog::loadObject ()
{
  QStringList l;
  l << "CHART_OBJECT_DATABASE" << "LOAD" << _id;

  Command command(l.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ChartObjectSellDialog::loadGroups: no plugin" << command.plugin();
    return;
  }

  plug->command(command);

  l = command.stringData().split(",", QString::SkipEmptyParts);

//  _color->setColor(set.color("Color"));
//  _date->setDateTime(set.dateTime("Date"));
//  _price->setValue(set.getDouble("Price"));
}

void ChartObjectSellDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("chart_object_sell_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("chart_object_sell_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);

  QColor c(settings.value("default_chart_object_sell_color", "green").toString());
  _color->setColor(c);
}

void ChartObjectSellDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("chart_object_sell_dialog_window_size", size());
  settings.setValue("chart_object_sell_dialog_window_position", pos());
  settings.sync();
}
