/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "FormulaEdit.h"
#include "BarData.h"
#include "PrefDialog.h"
#include "Setting.h"
#include "IndicatorPlugin.h"
#include "../src/newchart.xpm"
#include "../src/delete.xpm"
#include "../src/edit.xpm"
#include "../src/insert.xpm"
#include <qlayout.h>

#define BUTTON_SIZE 24

FormulaEdit::FormulaEdit (QWidget *w) : QWidget(w)
{
  functionList = config.getPluginList(Config::IndicatorPluginPath);
  
  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setMargin(0);
  hbox->setSpacing(1);
  
  list = new QTable(0, 3, this);
  list->setSelectionMode(QTable::Single);
  list->setSorting(FALSE);
  list->horizontalHeader()->setLabel(0, tr("Function"));
  list->horizontalHeader()->setLabel(1, tr("Plot"));
  list->horizontalHeader()->setLabel(2, tr("Parms"));
  list->setColumnWidth(0, 75);
  list->setColumnWidth(1, 50);
  list->setColumnWidth(2, 50);
  list->setColumnReadOnly(0, TRUE);
  list->setColumnReadOnly(2, TRUE);
  list->hideColumn(2);
  hbox->addWidget(list);
  
  toolbar = new Toolbar(this, 30, 30, TRUE);
  hbox->addWidget(toolbar);

  toolbar->addButton("add", newchart, tr("Add"));
  QObject::connect(toolbar->getButton("add"), SIGNAL(clicked()), this, SLOT(addItem()));
  
  toolbar->addButton("insert", insert, tr("Insert"));
  QObject::connect(toolbar->getButton("insert"), SIGNAL(clicked()), this, SLOT(insertItem()));
  
  toolbar->addButton("edit", edit, tr("Edit"));
  QObject::connect(toolbar->getButton("edit"), SIGNAL(clicked()), this, SLOT(editItem()));
  
  toolbar->addButton("delete", deleteitem, tr("Delete"));
  QObject::connect(toolbar->getButton("delete"), SIGNAL(clicked()), this, SLOT(deleteItem()));
}

FormulaEdit::~FormulaEdit ()
{
}

void FormulaEdit::addItem ()
{
  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("ADD Function"));
  dialog->createPage (tr("Functions"));
  dialog->addComboItem(tr("Functions"), tr("Functions"), functionList, 0);
  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }
  
  QString type = functionList[dialog->getComboIndex(tr("Functions"))];
  delete dialog;
  
  IndicatorPlugin *plug = config.getIndicatorPlugin(type);
  if (! plug)
  {
    config.closePlugin(type);
    return;
  }
  
  plug->setCustomFlag(TRUE);
  
  if (! plug->indicatorPrefDialog(this))
  {
    config.closePlugin(type);
    return;
  }

  Setting set = plug->getIndicatorSettings();
    
  list->setNumRows(list->numRows() + 1);
  
  list->setText(list->numRows() - 1, 0, set.getData("label"));
  
  list->setText(list->numRows() - 1, 2, set.getString());
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  check->setChecked(FALSE);
  list->setItem(list->numRows() - 1, 1, check);
  
  config.closePlugin(type);
}

void FormulaEdit::insertItem ()
{
  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("Insert Function"));
  dialog->createPage (tr("Functions"));
  dialog->addComboItem(tr("Functions"), tr("Functions"), functionList, 0);
  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }
  
  QString type = functionList[dialog->getComboIndex(tr("Functions"))];
  delete dialog;
  
  IndicatorPlugin *plug = config.getIndicatorPlugin(type);
  if (! plug)
  {
    config.closePlugin(type);
    return;
  }
  
  plug->setCustomFlag(TRUE);
  
  if (! plug->indicatorPrefDialog(this))
  {
    config.closePlugin(type);
    return;
  }

  Setting set = plug->getIndicatorSettings();
    
  list->insertRows(list->currentRow(), 1);
  
  list->setText(list->currentRow(), 0, set.getData("label"));
  
  list->setText(list->currentRow(), 2, set.getString());
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  check->setChecked(FALSE);
  list->setItem(list->currentRow(), 1, check);
  
  config.closePlugin(type);
}

void FormulaEdit::editItem ()
{
  Setting set;
  set.parse(list->text(list->currentRow(), 2));
  
  IndicatorPlugin *plug = config.getIndicatorPlugin(set.getData("plugin"));
  if (! plug)
  {
    config.closePlugin(set.getData("plugin"));
    return;
  }

  plug->setCustomFlag(TRUE);
  
  plug->setIndicatorSettings(set);
  
  if (! plug->indicatorPrefDialog(this))
  {
    config.closePlugin(set.getData("plugin"));
    return;
  }

  Setting set2 = plug->getIndicatorSettings();
    
  list->setText(list->currentRow(), 2, set2.getString());
  list->setText(list->currentRow(), 0, set2.getData("label"));
  
  config.closePlugin(set2.getData("plugin"));
}

void FormulaEdit::deleteItem ()
{
  list->removeRow(list->currentRow());
}

void FormulaEdit::setLine (QString d)
{
  Setting set;
  set.parse(d);

  list->setNumRows(list->numRows() + 1);
    
  list->setText(list->numRows() - 1, 0, set.getData("label"));
  
  list->setText(list->numRows() - 1, 2, set.getString());
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  if (! set.getData("plot").toInt())
    check->setChecked(FALSE);
  else
    check->setChecked(TRUE);
  list->setItem(list->numRows() - 1, 1, check);
}

QString FormulaEdit::getLine (int i)
{
  Setting set;
  set.parse(list->text(i, 2));
  QCheckTableItem *check = (QCheckTableItem *) list->item(i, 1);
  set.setData("plot", QString::number(check->isChecked()));
  return set.getString();
}

int FormulaEdit::getLines ()
{
  return list->numRows();
}

