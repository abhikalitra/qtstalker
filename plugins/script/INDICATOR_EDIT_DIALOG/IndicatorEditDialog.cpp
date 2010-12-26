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

#include "IndicatorEditDialog.h"
#include "Globals.h"
#include "Doc.h"

#include <QtDebug>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>
#include <QFormLayout>
#include <QFileInfo>
#include <QFileDialog>

IndicatorEditDialog::IndicatorEditDialog (Command *c)
{
  _command = c;
  _helpFile = "main.html";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("New Indicator");
  setWindowTitle(l.join(" "));

  createGUI();

  _name = _command->parm(1);

  if (_command->count() > 2)
  {
    _com->setText(_command->parm(2));
    fileButtonPressed2(_command->parm(3));
    _log->setChecked(_command->parm(5).toInt());
    _date->setChecked(_command->parm(6).toInt());
  }

  loadSettings();

  buttonStatus();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void IndicatorEditDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  vbox->addLayout(form);

  // command
  _com = new QLineEdit("perl");
  _com->setToolTip(tr("Script command"));
  connect(_com, SIGNAL(textChanged(const QString &)), this, SLOT(buttonStatus()));
  form->addRow(tr("Command"), _com);

  // file
  _fileButton = new QPushButton;
  connect(_fileButton, SIGNAL(clicked()), this, SLOT(fileButtonPressed()));
  form->addRow(tr("Script File"), _fileButton);

  // date check
  _date = new QCheckBox;
  _date->setChecked(TRUE);
  form->addRow(tr("Date Axis"), _date);

  // log check
  _log = new QCheckBox;
  _log->setChecked(FALSE);
  form->addRow(tr("Log Scaling"), _log);

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

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void IndicatorEditDialog::buttonStatus ()
{
  int status = 0;
  if (_fileButton->text().length())
    status++;

  if (_com->text().length())
    status++;

  if (status ==  2)
    _okButton->setEnabled(TRUE);
  else
    _okButton->setEnabled(FALSE);
}

void IndicatorEditDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void IndicatorEditDialog::done ()
{
  QString com = _com->text();
  com.remove(",");
  if (com.isEmpty())
  {
    _message->setText(tr("Invalid script command."));
    return;
  }

  QStringList l;
  l << _name << com << _file << "0";
  l << QString::number(_log->isChecked()) << QString::number(_date->isChecked());

  _command->setReturnData(l.join(","));

  saveSettings();

  accept();
}

void IndicatorEditDialog::cancel ()
{
  saveSettings();
  reject();
}

void IndicatorEditDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("indicator_new_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("indicator_new_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);

  _file = settings.value("indicator_new_dialog_last_file", QDir::homePath()).toString();
}

void IndicatorEditDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("indicator_new_dialog_window_size", size());
  settings.setValue("indicator_new_dialog_window_position", pos());
  settings.setValue("indicator_new_dialog_last_file", _file);
  settings.sync();
}

void IndicatorEditDialog::fileButtonPressed ()
{
  QFileDialog *dialog = new QFileDialog(this);
  dialog->setDirectory(_file);
  dialog->setWindowTitle(tr("Select Indicator Script File"));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(fileSelected(const QString &)), this, SLOT(fileButtonPressed2(QString)));
  dialog->show();
}

void IndicatorEditDialog::fileButtonPressed2 (QString d)
{
  _file = d;
  
  QFileInfo fi(_file);
  _fileButton->setText(fi.fileName());

  buttonStatus();
}
