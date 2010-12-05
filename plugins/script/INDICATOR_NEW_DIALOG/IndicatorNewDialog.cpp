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

#include "IndicatorNewDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "Command.h"
#include "ScriptPluginFactory.h"

#include <QtDebug>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>
#include <QFormLayout>
#include <QFileInfo>
#include <QFileDialog>

IndicatorNewDialog::IndicatorNewDialog ()
{
  _helpFile = "main.html";
  setWindowTitle("QtStalker" + g_session + ": " + tr("New Indicator"));

  createGUI();

  loadSettings();

  buttonStatus();
}

void IndicatorNewDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  vbox->addLayout(form);

  _name = new QLineEdit;
  _name->setToolTip(tr("Indicator name"));
  connect(_name, SIGNAL(textChanged(const QString &)), this, SLOT(buttonStatus()));
  form->addRow(tr("Name"), _name);

  // indicator
  _fileButton = new QPushButton;
  connect(_fileButton, SIGNAL(clicked()), this, SLOT(fileButtonPressed()));
  form->addRow(tr("Indicator Script"), _fileButton);

  _command = new QLineEdit("perl");
  _command->setToolTip(tr("Script command"));
  connect(_command, SIGNAL(textChanged(const QString &)), this, SLOT(buttonStatus()));
  form->addRow(tr("Command"), _command);

  // tab row
  _row = new QSpinBox;
  _row->setRange(1, 3);
  form->addRow(tr("Tab Row"), _row);

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

void IndicatorNewDialog::buttonStatus ()
{
  int status = 0;
  if (_name->text().length())
    status++;

  if (_fileButton->text().length())
    status++;

  if (_command->text().length())
    status++;

  if (status ==  3)
    _okButton->setEnabled(TRUE);
  else
    _okButton->setEnabled(FALSE);
}

void IndicatorNewDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void IndicatorNewDialog::done ()
{
  QString name = _name->text();
  name.remove(",");
  if (name.isEmpty())
  {
    _message->setText(tr("Invalid name."));
    return;
  }

  QString com = _command->text();
  com.remove(",");
  if (com.isEmpty())
  {
    _message->setText(tr("Invalid script command."));
    return;
  }

  QStringList l;
  l << "INDICATOR_DATABASE" << "INDICATORS";

  Command command(l.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "IndicatorNewDialog::done: no plugin" << command.plugin();
    return;
  }

  if (plug->command(command))
    cancel();

  l = command.stringData().split(",", QString::SkipEmptyParts);
  if (l.contains(name))
  {
    _message->setText(tr("Duplicate indicator name entered."));
    return;
  }

  l.clear();
  l << "INDICATOR_DATABASE" << "SAVE" << name << _file << com << _row->text();
  l << QString::number(_log->isChecked()) << QString::number(_date->isChecked());

  command.parse(l.join(","));

  if (plug->command(command))
    cancel();

  l.removeFirst();
  l.removeFirst();
  emit signalDone(l.join(","));

  saveSettings();

  accept();
}

void IndicatorNewDialog::cancel ()
{
  saveSettings();
  reject();
}

QString & IndicatorNewDialog::stringData ()
{
  return _stringData;
}

void IndicatorNewDialog::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("main"); // global setting

  QSize sz = settings.value("indicator_new_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("indicator_new_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);

  _file = settings.value("indicator_new_dialog_last_file", QDir::homePath()).toString();
}

void IndicatorNewDialog::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("indicator_new_dialog_window_size", size());
  settings.setValue("indicator_new_dialog_window_position", pos());
  settings.setValue("indicator_new_dialog_last_file", _file);
  settings.sync();
}

void IndicatorNewDialog::fileButtonPressed ()
{
  QFileDialog *dialog = new QFileDialog(this);
  dialog->setDirectory(_file);
  dialog->setWindowTitle(tr("Select Indicator Script File"));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(fileSelected(const QString &)), this, SLOT(fileButtonPressed2(QString)));
  dialog->show();
}

void IndicatorNewDialog::fileButtonPressed2 (QString d)
{
  _file = d;
  
  QFileInfo fi(_file);
  _fileButton->setText(fi.fileName());

  buttonStatus();
}
