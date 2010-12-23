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

#include "TabWidgetDialog.h"
#include "Globals.h"
#include "Doc.h"

#include <QtDebug>
#include <QFormLayout>
#include <QSettings>
#include <QDialogButtonBox>
#include <QLayout>
#include <QPushButton>

TabWidgetDialog::TabWidgetDialog (QString id)
{
  _id = id;
  _posList << tr("North") << tr("South") << tr("West") << tr("East");
  _helpFile = "main.html";

  setWindowTitle("Qtstalker" + g_session + ": " + tr("Chart Tab Settings"));

  createGUI();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void TabWidgetDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  vbox->addLayout(form);

  // position
  QSettings settings(g_settingsFile);

  _ttp = settings.value(_id).toInt();

  _position = new QComboBox;
  _position->addItems(_posList);
  _position->setCurrentIndex(_ttp);
  form->addRow(tr("Position"), _position);
  
  // north south height
  QString key = _id + "_NSH";
  _tnsh = settings.value(key).toInt();

  _nsh = new QSpinBox;
  _nsh->setRange(1, 100);
  _nsh->setValue(_tnsh);
  form->addRow(tr("North / South Height"), _nsh);

  // north south width
  key = _id + "_NSW";
  _tnsw = settings.value(key).toInt();

  _nsw = new QSpinBox;
  _nsw->setRange(1, 100);
  _nsw->setValue(_tnsw);
  form->addRow(tr("North / South Width"), _nsw);

  // east west height
  key = _id + "_EWH";
  _tewh = settings.value(key).toInt();

  _ewh = new QSpinBox;
  _ewh->setRange(1, 100);
  _ewh->setValue(_tewh);
  form->addRow(tr("East / West Height"), _ewh);

  // east west width
  key = _id + "_EWW";
  _teww = settings.value(key).toInt();

  _eww = new QSpinBox;
  _eww->setRange(1, 100);
  _eww->setValue(_teww);
  form->addRow(tr("East / West Width"), _eww);

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

void TabWidgetDialog::done ()
{
  QSettings settings(g_settingsFile);

  int flag = 0;
  QString d, key;
  
  int index = _position->currentIndex();
  if (index != _ttp)
  {
    settings.setValue(_id, index);
    flag = 1;
  }

  if (_tnsh != _nsh->value())
  {
    key = _id + "_NSH";
    settings.setValue(key, _nsh->value());
    flag = 1;
  }

  if (_tnsw != _nsw->value())
  {
    key = _id + "_NSW";
    settings.setValue(key, _nsw->value());
    flag = 1;
  }

  if (_tewh != _ewh->value())
  {
    key = _id + "_EWH";
    settings.setValue(key, _ewh->value());
    flag = 1;
  }

  if (_teww != _eww->value())
  {
    key = _id + "_EWW";
    settings.setValue(key, _eww->value());
    flag = 1;
  }

  settings.sync();

  if (flag)
    emit signalChanged();

  accept();
}

void TabWidgetDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void TabWidgetDialog::cancel ()
{
  saveSettings();
  reject();
}

void TabWidgetDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("tab_widget_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("tab_widget_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void TabWidgetDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("tab_widget_dialog_window_size", size());
  settings.setValue("tab_widget_dialog_window_position", pos());
  settings.sync();
}
