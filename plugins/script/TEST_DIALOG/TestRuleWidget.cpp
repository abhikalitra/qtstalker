/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "TestRuleWidget.h"
#include "Globals.h"
#include "PluginFactory.h"
#include "DataDataBase.h"

#include "../../../pics/add.xpm"
#include "../../../pics/delete.xpm"
#include "../../../pics/edit.xpm"

#include <QDebug>
#include <QToolBar>
#include <QLayout>
#include <QInputDialog>

TestRuleWidget::TestRuleWidget ()
{
  createGUI();
}

TestRuleWidget::~TestRuleWidget ()
{
  qDeleteAll(_iList);
  qDeleteAll(_ruleList);
}

void TestRuleWidget::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QToolBar *tb = new QToolBar(this);
  vbox->addWidget(tb);

  // add button
  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(add_xpm));
  b->setToolTip(tr("Add Indicator"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addIndicator()));
  tb->addWidget(b);

  // edit button
  _editButton = new QToolButton;
  _editButton->setIcon(QIcon(edit_xpm));
  _editButton->setToolTip(tr("Edit Indicator"));
  connect(_editButton, SIGNAL(clicked(bool)), this, SLOT(editIndicator()));
  _editButton->setEnabled(FALSE);
  tb->addWidget(_editButton);

  // edit rule button
  _editRuleButton = new QToolButton;
  _editRuleButton->setIcon(QIcon(edit_xpm));
  _editRuleButton->setToolTip(tr("Edit Indicator Rules"));
  connect(_editRuleButton, SIGNAL(clicked(bool)), this, SLOT(editIndicatorRules()));
  _editRuleButton->setEnabled(FALSE);
  tb->addWidget(_editRuleButton);

  // delete button
  _deleteButton = new QToolButton;
  _deleteButton->setIcon(QIcon(delete_xpm));
  _deleteButton->setToolTip(tr("Delete Indicator"));
  connect(_deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteIndicator()));
  _deleteButton->setEnabled(FALSE);
  tb->addWidget(_deleteButton);
 
  // list
  _list = new QListWidget;
  _list->setSortingEnabled(FALSE);
  connect(_list, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(listSelected()));
  vbox->addWidget(_list);
}

void TestRuleWidget::clear ()
{
  qDeleteAll(_iList);
  _iList.clear();
  _list->clear();

  qDeleteAll(_ruleList);
  _ruleList.clear();

  listSelected();
}

void TestRuleWidget::addIndicator ()
{
  QStringList l;
  QSettings db(g_globalSettings);
  l = db.value("indicator_plugins").toStringList();
  l.sort();
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
  dialog->setLabelText(tr("Select indicator"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(addIndicator2(QString)));
  dialog->show();
}

void TestRuleWidget::addIndicator2 (QString d)
{
  PluginFactory fac;
  Plugin *plug = fac.plugin(d);
  if (! plug)
  {
    qDebug() << "TestRuleWidget::addIndicator2: no plugin" << d;
    return;
  }

  Indicator *i = new Indicator(0);
  plug->defaults(i->settings());
  _iList.append(i);

  _list->addItem(d);

  Setting *rule = new Setting;
  _ruleList.append(rule);

//  plug->dialog(this, i);
  
  delete plug;
}

void TestRuleWidget::deleteIndicator ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void TestRuleWidget::editIndicator ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;

  PluginFactory fac;
  Plugin *plug = fac.plugin(l.at(0)->text());
  if (! plug)
  {
    qDebug() << "TestRuleWidget::editIndicator: no plugin" << l.at(0)->text();
    return;
  }

  Indicator *i = _iList.at(_list->currentRow());
  if (! i)
    return;
  
//  plug->dialog(this, i);

  delete plug;
}

void TestRuleWidget::editIndicatorRules ()
{
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;

  PluginFactory fac;
  Plugin *plug = fac.plugin(l.at(0)->text());
  if (! plug)
  {
    qDebug() << "TestRuleWidget::addIndicator2: no plugin" << l.at(0)->text();
    return;
  }

  Setting *rule = _ruleList.at(_list->currentRow());
  if (! rule)
    return;
  
//  plug->testRuleDialog(this, rule);
  
  delete plug;
}

void TestRuleWidget::listSelected ()
{
  bool status = TRUE;
  QList<QListWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    status = FALSE;

  _editButton->setEnabled(status);
  _editRuleButton->setEnabled(status);
  _deleteButton->setEnabled(status);
}

int TestRuleWidget::save (QString name, QString prefix)
{
  DataDataBase db("testRules");
  db.transaction();

  QString key = prefix + "_INDICATOR_COUNT";
  if (db.save(name, key, QString::number(_iList.count())))
  {
    qDebug() << "TestRuleWidget::save: error saving indicator count" << name << key;
    return 1;
  }
  
  int loop = 0;
  for (; loop < _iList.count(); loop++)
  {
    QString s;
    _iList.at(loop)->settings()->string(s);
    key = prefix + "_INDICATOR_" + QString::number(loop);
    if (db.save(name, key, s))
      qDebug() << "TestRuleWidget::save: error saving indicator" << name << key;
  }

  key = prefix + "_RULE_COUNT";
  if (db.save(name, key, QString::number(_ruleList.count())))
  {
    qDebug() << "TestRuleWidget::save: error saving rule count" << name << key;
    return 1;
  }

  loop = 0;
  for (; loop < _ruleList.count(); loop++)
  {
    QString s;
    _ruleList.at(loop)->string(s);
    key = prefix + "_RULE_" + QString::number(loop);
    if (db.save(name, key, s))
      qDebug() << "TestRuleWidget::save: error saving rule" << name << key;
  }

  db.commit();
  
  return 0;
}

int TestRuleWidget::load (QString name, QString prefix)
{
  clear();
  
  DataDataBase db("testRules");

  QString key = prefix + "_INDICATOR_COUNT";
  QString data;
  if (db.load(name, key, data))
  {
    qDebug() << "TestRuleWidget::load: error loading indicator count" << name << key;
    return 1;
  }

  int count = data.toInt();
  int loop = 0;
  for (; loop < count; loop++)
  {
    key = prefix + "_INDICATOR_" + QString::number(loop);
    if (db.load(name, key, data))
    {
      qDebug() << "TestRuleWidget::load: error loading indicator" << name << key;
      continue;
    }
    
    Indicator *i = new Indicator(0);

    Setting *set = i->settings();
    set->parse(data);
    _iList.append(i);

    _list->addItem(set->data("PLUGIN"));
  }

  key = prefix + "_RULE_COUNT";
  if (db.load(name, key, data))
  {
    qDebug() << "TestRuleWidget::load: error loading rule count" << name << key;
    return 1;
  }
  
  count = data.toInt();
  loop = 0;
  for (; loop < count; loop++)
  {
    key = prefix + "_RULE_" + QString::number(loop);
    if (db.load(name, key, data))
    {
      qDebug() << "TestRuleWidget::load: error loading rule" << name << key;
      continue;
    }

    Setting *set = new Setting;
    set->parse(data);
    _ruleList.append(set);
  }

  return 0;
}
