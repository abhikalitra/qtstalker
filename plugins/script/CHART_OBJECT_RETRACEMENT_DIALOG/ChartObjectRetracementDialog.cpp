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

#include "ChartObjectRetracementDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "ChartObjectDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>

ChartObjectRetracementDialog::ChartObjectRetracementDialog (Command *c)
{
  _command = c;
  _helpFile = "main.html";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit Retracement") << _command->parm(1);
  setWindowTitle(l.join(" "));

  createDialog();

  createMainPage();

  createLinePage();

  loadSettings();

  loadObject();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void ChartObjectRetracementDialog::createDialog ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

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

void ChartObjectRetracementDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  _color = new ColorButton(this, QColor(Qt::red));
//  _color->setColorButton();
  form->addRow(tr("Color"), _color);
  
  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  form->addRow(tr("Start Date"), _date);

  // date2
  _date2 = new QDateTimeEdit;
  _date2->setCalendarPopup(TRUE);
  form->addRow(tr("End Date"), _date2);
  
  // high
  _high = new QDoubleSpinBox;
  _high->setRange(0.0, 99999999.0);
  form->addRow(tr("High"), _high);

  // low
  _low = new QDoubleSpinBox;
  _low->setRange(0.0, 99999999.0);
  form->addRow(tr("Low"), _low);

  // extend
  _extend = new QCheckBox;
  form->addRow(tr("Extend"), _extend);

  // default
  _default = new QCheckBox;
  form->addRow(tr("Set as default"), _default);

  _tabs->addTab(w, tr("Settings"));
}

void ChartObjectRetracementDialog::createLinePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // line1
  _line1 = new QDoubleSpinBox;
  _line1->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 1"), _line1);

  // line2
  _line2 = new QDoubleSpinBox;
  _line2->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 2"), _line2);

  // line3
  _line3 = new QDoubleSpinBox;
  _line3->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 3"), _line3);

  // line4
  _line4 = new QDoubleSpinBox;
  _line4->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 4"), _line4);

  // line5
  _line5 = new QDoubleSpinBox;
  _line5->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 5"), _line5);

  // line6
  _line6 = new QDoubleSpinBox;
  _line6->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 6"), _line6);

  _tabs->addTab(w, tr("Levels"));
}

void ChartObjectRetracementDialog::done ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_settingsFile);
    settings.setValue("default_chart_object_retracement_color", _color->color().name());
    settings.setValue("default_chart_object_retracement_line1", _line1->value());
    settings.setValue("default_chart_object_retracement_line2", _line2->value());
    settings.setValue("default_chart_object_retracement_line3", _line3->value());
    settings.setValue("default_chart_object_retracement_line4", _line4->value());
    settings.setValue("default_chart_object_retracement_line5", _line5->value());
    settings.setValue("default_chart_object_retracement_line6", _line6->value());
    settings.sync();
  }

  _co.setData("Color", _color->color());
  _co.setData("Date", _date->dateTime());
  _co.setData("Date2", _date2->dateTime());
  _co.setData("High", _high->value());
  _co.setData("Low", _low->value());
  _co.setData("Extend", _extend->isChecked());
  _co.setData("Line1", _line1->value());
  _co.setData("Line2", _line2->value());
  _co.setData("Line3", _line3->value());
  _co.setData("Line4", _line4->value());
  _co.setData("Line5", _line5->value());
  _co.setData("Line6", _line6->value());

  ChartObjectDataBase db;
  db.save(&_co);

  saveSettings();

  _command->setReturnData("0");

  accept();
}

void ChartObjectRetracementDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ChartObjectRetracementDialog::cancel ()
{
  saveSettings();
  reject();
}

void ChartObjectRetracementDialog::loadObject ()
{
  _co.setData("ID", _command->parm(1));

  ChartObjectDataBase db;
  db.load(&_co);

  _color->setColor(_co.color("Color"));
  _date->setDateTime(_co.dateTime("Date"));
  _date2->setDateTime(_co.dateTime("Date2"));
  _high->setValue(_co.getDouble("High"));
  _low->setValue(_co.getDouble("Low"));
  _extend->setChecked(_co.getInt("Extend"));
  _line1->setValue(_co.getDouble("Line1"));
  _line2->setValue(_co.getDouble("Line2"));
  _line3->setValue(_co.getDouble("Line3"));
  _line4->setValue(_co.getDouble("Line4"));
  _line5->setValue(_co.getDouble("Line5"));
  _line6->setValue(_co.getDouble("Line6"));
}

void ChartObjectRetracementDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("chart_object_retracement_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("chart_object_retracement_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);

  QColor c(settings.value("default_chart_object_retracement_color", "red").toString());
  _color->setColor(c);

  _line1->setValue(settings.value("default_chart_object_retracement_line1").toDouble());
  _line2->setValue(settings.value("default_chart_object_retracement_line2").toDouble());
  _line3->setValue(settings.value("default_chart_object_retracement_line3").toDouble());
  _line4->setValue(settings.value("default_chart_object_retracement_line4").toDouble());
  _line5->setValue(settings.value("default_chart_object_retracement_line5").toDouble());
  _line6->setValue(settings.value("default_chart_object_retracement_line6").toDouble());
}

void ChartObjectRetracementDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("chart_object_retracement_dialog_window_size", size());
  settings.setValue("chart_object_retracement_dialog_window_position", pos());
  settings.sync();
}
