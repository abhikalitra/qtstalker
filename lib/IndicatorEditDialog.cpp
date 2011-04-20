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
#include "PluginFactory.h"
#include "DataDataBase.h"
#include "NewDialog.h"
#include "PluginWidget.h"

#include <QtDebug>
#include <QSettings>
#include <QInputDialog>

IndicatorEditDialog::IndicatorEditDialog (QWidget *p, QString n) : Dialog (p)
{
  _name = n;
  _keySize = "indicator_edit_dialog_window_size";
  _keyPos = "indicator_edit_dialog_window_position";
  _status = _NONE;
  _newFlag = 0;

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Indicator") << _name;
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  if (_name.isEmpty())
    createDefaults();
}

void IndicatorEditDialog::createGUI ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QStringList l;
  DataDataBase db("indicators");
  db.load(_name, l);
  
  int loop = 0;
  for (; loop < l.count(); loop++)
    addTab(l.at(loop), 0);

  // add button
  QPushButton *b = _buttonBox->addButton(QString("&Add"), QDialogButtonBox::ActionRole);
  b->setToolTip(tr("Add Indicator"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addIndicator()));

  // delete button
  b = _buttonBox->addButton(QString("&Delete"), QDialogButtonBox::ActionRole);
  b->setToolTip(tr("Delete Indicator"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(deleteIndicator()));

  // apply button
  b = _buttonBox->addButton(QString("A&pply"), QDialogButtonBox::ActionRole);
  b->setToolTip(tr("Apply Changes"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(apply()));
}

void IndicatorEditDialog::done ()
{
  _status = _DONE;
  if (applySave())
    return;
  saveSettings();
  accept();
}

void IndicatorEditDialog::addIndicator ()
{
  QStringList l;
  QSettings set(g_globalSettings);
  l = set.value("indicator_plugins").toStringList();
  l.sort();

  QSettings set2(g_localSettings);
  QString s = set2.value("indicator_edit_dialog_last_indicator", 0).toString();
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
  dialog->setTextValue(s);
  dialog->setLabelText(tr("Select indicator"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(addIndicator2(QString)));
  dialog->show();
}

void IndicatorEditDialog::addIndicator2 (QString d)
{
  PluginFactory fac;
  Plugin *plug = fac.plugin(d);
  if (! plug)
  {
    qDebug() << "IndicatorEditDialog::addIndicator2: no plugin" << d;
    return;
  }

  QSettings settings(g_localSettings);
  settings.setValue("indicator_edit_dialog_last_indicator", d);
  settings.sync();

  QString s;
  plug->defaults(s);
  delete plug;

  addTab(s, 1);
}

void IndicatorEditDialog::deleteIndicator ()
{
  int index = _tabs->currentIndex();
  if (index == -1)
    return;

  QWidget *w = _tabs->currentWidget();
  if (! w)
    return;

  _tabs->removeTab(index);

  delete w;
}

void IndicatorEditDialog::addTab (QString d, int insertFlag)
{
  Command command(d);

  int flag = 0;
  PluginWidget *w = 0;
  QString s = command.parm("TAB");
  if (s.isEmpty())
    flag++;
  else
  {
    w = (PluginWidget *) _tabs->widget(s.toInt());
    if (! w)
      flag++;
  }

  if (flag)
  {
    PluginFactory fac;
    Plugin *plug = fac.plugin(command.plugin());
    if (! plug)
      return;

    w = plug->dialog(this);
    if (! w)
    {
      delete plug;
      return;
    }
    delete plug;

    if (insertFlag)
    {
      _tabs->insertTab(_tabs->currentIndex() + 1, w, command.plugin());
      _tabs->setCurrentIndex(_tabs->currentIndex() + 1);
    }
    else
    {
      _tabs->addTab(w, command.plugin());
      _tabs->setCurrentIndex(_tabs->count() - 1);
    }
  }
  
  w->setCommand(d);
}

void IndicatorEditDialog::newDialog ()
{
  QStringList l;
  DataDataBase db("indicators");
  db.names(l);

  NewDialog *dialog = new NewDialog(this);
  dialog->setItems(l);
  dialog->setTitle(tr("Enter new indicator name"));
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(newDialog2(QString)));
  dialog->show();
}

void IndicatorEditDialog::newDialog2 (QString d)
{
  _name = d;

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Indicator") << _name;
  setWindowTitle(l.join(" "));

  _newFlag = 1;

  if (_status == _DONE)
    done();
  else if (_status == _APPLY)
    applySave();
}

void IndicatorEditDialog::apply ()
{
  _status = _APPLY;
  applySave();
}

int IndicatorEditDialog::applySave ()
{
  if (_name.isEmpty())
  {
    newDialog();
    return 1;
  }

  Indicator i;
  i.setName(_name);
  i.load();
  
  QStringList l;
  int loop = 0;
  for (; loop < _tabs->count(); loop++)
  {
    PluginWidget *w = (PluginWidget *) _tabs->widget(loop);
    w->commands(l, loop);
  }

  i.setCommands(l);
  i.save();

  i.add(_name);

  saveSettings();

  if (_newFlag)
    g_middleMan->indicatorNew(_name);

  emit signalDone();

  return 0;
}

void IndicatorEditDialog::createDefaults ()
{
  addIndicator2(QString("DOHLCVI"));
  addIndicator2(QString("IPLOT"));
}
