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
#include "SymbolDialog.h"
#include "../pics/newchart.xpm"
#include "../pics/delete.xpm"
#include "../pics/edit.xpm"
#include "../pics/insert.xpm"
#include "../pics/openchart.xpm"
#include "../pics/filesave.xpm"
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qinputdialog.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qpoint.h>

#define BUTTON_SIZE 24

FormulaEdit::FormulaEdit (QWidget *w, int t) : QWidget(w)
{
  type = (FormulaEditType) t;
  config.getPluginList(Config::IndicatorPluginPath, functionList);
  
  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setMargin(0);
  hbox->setSpacing(1);
  
  list = new QTable(0, 4, this);
  list->setSelectionMode(QTable::Single);
  list->setSorting(FALSE);
  list->horizontalHeader()->setLabel(0, tr("Function"));
  list->horizontalHeader()->setLabel(1, tr("Plot"));
  list->horizontalHeader()->setLabel(2, tr("Scale"));
  list->horizontalHeader()->setLabel(3, tr("Parms"));
  list->setColumnStretchable(0, TRUE);
  list->setColumnWidth(1, 35);
  list->setColumnWidth(2, 40);
  list->setColumnReadOnly(0, TRUE);
  list->setColumnReadOnly(3, TRUE);
  list->hideColumn(3);
  QObject::connect(list, SIGNAL(doubleClicked(int, int, int, const QPoint &)), this,
                   SLOT(slotDoubleClicked(int, int, int, const QPoint &)));
  hbox->addWidget(list);
  
  toolbar = new Toolbar(this, 30, 30, TRUE);
  hbox->addWidget(toolbar);

  QString s("add");
  QString s2(tr("Add"));
  toolbar->addButton(s, newchart, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(addItem()));
  
  s = "insert";
  s2 = tr("Insert");
  toolbar->addButton(s, insert, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(insertItem()));
  
  s = "edit";
  s2 = tr("Edit");
  toolbar->addButton(s, edit, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(editItem()));
  
  s = "delete";
  s2 = tr("Delete");
  toolbar->addButton(s, deleteitem, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(deleteItem()));

  s = "open";
  s2 = tr("Open Rule");
  toolbar->addButton(s, openchart, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(openRule()));

  s = "save";
  s2 = tr("Save Rule");
  toolbar->addButton(s, filesave, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(saveRule()));
}

FormulaEdit::~FormulaEdit ()
{
}

void FormulaEdit::addItem ()
{
  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("ADD Function"));
  QString s(tr("Functions")); 
  dialog->createPage (s);
  dialog->addComboItem(s, s, functionList, 0);
  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }
  
  QString type = functionList[dialog->getComboIndex(s)];
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

  Setting set;
  plug->getIndicatorSettings(set);
    
  list->setNumRows(list->numRows() + 1);
  
  list->setText(list->numRows() - 1, 0, set.getData("label"));
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  check->setChecked(FALSE);
  list->setItem(list->numRows() - 1, 1, check);
  
  check = new QCheckTableItem(list, QString::null);
  check->setChecked(FALSE);
  list->setItem(list->numRows() - 1, 2, check);

  set.getString(s);
  list->setText(list->numRows() - 1, 3, s);
  
  config.closePlugin(type);
}

void FormulaEdit::insertItem ()
{
  PrefDialog *dialog = new PrefDialog(this);
  dialog->setCaption(tr("Insert Function"));
  QString s(tr("Functions")); 
  dialog->createPage (s);
  dialog->addComboItem(s, s, functionList, 0);
  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }
  
  QString type = functionList[dialog->getComboIndex(s)];
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

  Setting set;
  plug->getIndicatorSettings(set);
    
  int row = list->currentRow();

  list->insertRows(row, 1);
  
  list->setText(row, 0, set.getData("label"));
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  check->setChecked(FALSE);
  list->setItem(row, 1, check);
  
  check = new QCheckTableItem(list, QString::null);
  check->setChecked(FALSE);
  list->setItem(row, 2, check);
  
  set.getString(s);
  list->setText(row, 3, s);
  
  config.closePlugin(type);
}

void FormulaEdit::editItem ()
{
  Setting set;
  QString s = list->text(list->currentRow(), 3);
  set.parse(s);

  QString plugin(set.getData("plugin"));  
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return;
  }

  plug->setCustomFlag(TRUE);
  
  plug->setIndicatorSettings(set);
  
  if (! plug->indicatorPrefDialog(this))
  {
    config.closePlugin(plugin);
    return;
  }

  Setting set2;
  plug->getIndicatorSettings(set2);
    
  list->setText(list->currentRow(), 0, set2.getData("label"));
  
  set2.getString(s);
  list->setText(list->currentRow(), 3, s);
  
  config.closePlugin(plugin);
}

void FormulaEdit::deleteItem ()
{
  list->removeRow(list->currentRow());
}

void FormulaEdit::setLine (QString &d)
{
  Setting set;
  set.parse(d);

  list->setNumRows(list->numRows() + 1);
    
  list->setText(list->numRows() - 1, 0, set.getData("label"));
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  if (! set.getData("plot").toInt())
    check->setChecked(FALSE);
  else
    check->setChecked(TRUE);
  list->setItem(list->numRows() - 1, 1, check);
  
  check = new QCheckTableItem(list, QString::null);
  if (! set.getData("scale").toInt())
    check->setChecked(FALSE);
  else
    check->setChecked(TRUE);
  list->setItem(list->numRows() - 1, 2, check);
  
  QString s;
  set.getString(s);
  list->setText(list->numRows() - 1, 3, s);
}

QString FormulaEdit::getLine (int i)
{
  Setting set;
  QString s = list->text(i, 3);
  set.parse(s);
  
  QCheckTableItem *check = (QCheckTableItem *) list->item(i, 1);
  set.setData("plot", QString::number(check->isChecked()));
  
  check = (QCheckTableItem *) list->item(i, 2);
  set.setData("scale", QString::number(check->isChecked()));
  
  set.getString(s);
  return s;
}

int FormulaEdit::getLines ()
{
  return list->numRows();
}

void FormulaEdit::openRule ()
{
  QString s("*");
  QString s2(config.getData(Config::CUSRulePath));
  SymbolDialog *dialog = new SymbolDialog(this, 
  					  s2,
					  s,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select rule to open."));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QFile f(dialog->selectedFile());
    if (! f.open(IO_ReadOnly))
    {
      qDebug("FormulaEdit::openItem:can't read file");
      delete dialog;
      return;
    }
    QTextStream stream(&f);
  
    list->setNumRows(0);
    
    while(stream.atEnd() == 0)
    {
      s = stream.readLine();
      s = s.stripWhiteSpace();
      if (! s.length())
        continue;
      
      QStringList l = QStringList::split("=", s, FALSE);
    
      if (l.count() < 2)
        continue;
      
      if (l.count() > 2)
      {
        s = s.remove(0, l[0].length() + 1);
        setLine(s);
      }
    }
  
    f.close();
  }

  delete dialog;
}

void FormulaEdit::saveRule ()
{
  bool ok = checkError();
  if (ok)
  {
    if (type == Logic)
    {
      QMessageBox::information(this,
                               tr("Qtstalker: Error"),
			       tr("Must have one COMP step checked."));
      return;
    }
    else
    {
      QMessageBox::information(this,
                               tr("Qtstalker: Error"),
			       tr("No step(s) have been checked to plot."));
      return;
    }
  }
  
  QString selection = QInputDialog::getText(tr("Save Rule"),
  					    tr("Enter name for rule."),
					    QLineEdit::Normal,
					    tr("NewRule"),
					    &ok,
					    this);
  if ((! ok) || (selection.isNull()))
    return;

  while (selection.contains(" "))
    selection = selection.remove(selection.find(" ", 0, TRUE), 1);
    
  QString s = config.getData(Config::CUSRulePath);
  s.append("/");
  s.append(selection);
  QDir dir(s);
  if (dir.exists(s, TRUE))
  {
    int rc = QMessageBox::warning(this,
  			          tr("Qtstalker: Warning"),
			          tr("Rule already exists. Do you want to replace it?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::No)
      return;
  }
  
  QFile f(s);
  if (! f.open(IO_WriteOnly))
  {
    qDebug("FormulaEdit::saveItem:can't open file");
    return;
  }
  QTextStream stream(&f);
  
  int loop;
  for (loop = 0; loop < list->numRows(); loop++)
    stream << QString::number(loop + 1) << "=" << getLine(loop) << "\n";

  f.close(); 
}

bool FormulaEdit::checkError ()
{
  bool rc = TRUE;
  int loop;
  int comp = 0;
  Setting set;
  QString s;
  for (loop = 0; loop < list->numRows(); loop++)
  {
    s = getLine(loop);
    set.parse(s);
    
    bool p = set.getInt("plot");
    
    if (type == Indicator)
    {
      if (p)
      {
        rc = FALSE;
	break;
      }
    }
    else
    {
      QString s = set.getData("plugin");
      if (! s.compare("COMP") && p == TRUE)
        comp++;
    }
  }
  
  if (type == Logic)
  {
    if (comp == 1)
      rc = FALSE;
  }
  
  return rc;
}

void FormulaEdit::slotDoubleClicked (int, int, int, const QPoint &)
{
  editItem();
}


