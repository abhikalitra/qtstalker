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
#include "../src/newchart.xpm"
#include "../src/delete.xpm"
#include "../src/edit.xpm"
#include <qlayout.h>

#define BUTTON_SIZE 24

FormulaEdit::FormulaEdit (QWidget *w) : QWidget(w)
{
  functionList = config.getPluginList(Config::IndicatorPluginPath);
  
  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setMargin(0);
  hbox->setSpacing(1);
  
  list = new QTable(0, 2, this);
  list->setSelectionMode(QTable::Single);
  list->setSorting(FALSE);
  list->horizontalHeader()->setLabel(0, tr("Function"));
  list->horizontalHeader()->setLabel(1, tr("Plot"));
  list->setColumnWidth(0, 275);
  list->setColumnWidth(1, 50);
  list->setColumnReadOnly(0, TRUE);
  hbox->addWidget(list);
  
  toolbar = new Toolbar(this, 30, 30, TRUE);
  hbox->addWidget(toolbar);

  toolbar->addButton("add", newchart, tr("Add"));
  QObject::connect(toolbar->getButton("add"), SIGNAL(clicked()), this, SLOT(addItem()));
  
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
  
  Plugin *plug = config.getPlugin(Config::IndicatorPluginPath, type);
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

  QString s = plug->getCustomSettings();
    
  config.closePlugin(type);
  
  list->setNumRows(list->numRows() + 1);
  
  list->setText(list->numRows() - 1, 0, s);
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  check->setChecked(FALSE);
  list->setItem(list->numRows() - 1, 1, check);
}

void FormulaEdit::editItem ()
{
  QStringList l = QStringList::split(",", list->text(list->currentRow(), 0), FALSE);
  
  Plugin *plug = config.getPlugin(Config::IndicatorPluginPath, l[0]);
  if (! plug)
  {
    config.closePlugin(l[0]);
    return;
  }
  
  plug->setCustomSettings(list->text(list->currentRow(), 0));
  
  if (! plug->indicatorPrefDialog(this))
  {
    config.closePlugin(l[0]);
    return;
  }

  QString s = plug->getCustomSettings();
    
  config.closePlugin(l[0]);

  list->setText(list->currentRow(), 0, s);
}

void FormulaEdit::deleteItem ()
{
  list->removeRow(list->currentRow());
}

void FormulaEdit::setLine (QString d)
{
  QStringList l = QStringList::split("|", d, FALSE);
  if (l.count() != 2)
  {
    qDebug("FormulaEdit::setLine: invalid field count");
    return;
  }

  list->setNumRows(list->numRows() + 1);
    
  list->setText(list->numRows() - 1, 0, l[0]);
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  if (! l[1].toInt())
    check->setChecked(FALSE);
  else
    check->setChecked(TRUE);
  list->setItem(list->numRows() - 1, 1, check);
}

QString FormulaEdit::getLine (int i)
{
  QString s = list->text(i, 0);
  s.append("|");
  QCheckTableItem *check = (QCheckTableItem *) list->item(i, 1);
  s.append(QString::number(check->isChecked()));
  return s;
}

int FormulaEdit::getLines ()
{
  return list->numRows();
}

QString FormulaEdit::getFormula (int i)
{
  return list->text(i, 0);
}

QString FormulaEdit::getPlot (int i)
{
  QCheckTableItem *check = (QCheckTableItem *) list->item(i, 1);
  return QString::number(check->isChecked());
}

