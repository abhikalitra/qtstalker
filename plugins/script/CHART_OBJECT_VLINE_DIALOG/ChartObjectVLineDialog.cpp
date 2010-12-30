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
#include "Doc.h"
#include "ChartObjectDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>

ChartObjectVLineDialog::ChartObjectVLineDialog (Command *c)
{
  _command = c;
  _helpFile = "main.html";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit VLine") << _command->parm(1);
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  loadObject();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void ChartObjectVLineDialog::createGUI ()
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
  _color = new ColorButton(this, QColor(Qt::red));
//  _color->setColorButton();
  form->addRow(tr("Color"), _color);
  
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

void ChartObjectVLineDialog::done ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_settingsFile);
    settings.setValue("default_chart_object_vline_color", _color->color().name());
    settings.sync();
  }

  _co.setData("Color", _color->color());
  _co.setData("Date", _date->dateTime());

  ChartObjectDataBase db;
  db.save(&_co);

  saveSettings();

  _command->setReturnData("0");

  accept();
}

void ChartObjectVLineDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ChartObjectVLineDialog::cancel ()
{
  saveSettings();
  reject();
}

void ChartObjectVLineDialog::loadObject ()
{
  _co.setData("ID", _command->parm(1));

  ChartObjectDataBase db;
  db.load(&_co);

  _date->setDateTime(_co.dateTime("Date"));
  _color->setColor(_co.color("Color"));
}

void ChartObjectVLineDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("chart_object_vline_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("chart_object_vline_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);

  QColor c(settings.value("default_chart_object_vline_color", "red").toString());
  _color->setColor(c);
}

void ChartObjectVLineDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("chart_object_vline_dialog_window_size", size());
  settings.setValue("chart_object_vline_dialog_window_position", pos());
  settings.sync();
}
