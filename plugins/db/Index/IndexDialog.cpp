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

#include "IndexDialog.h"
#include "SymbolDialog.h"
#include "PrefDialog.h"
#include <qlayout.h>
#include <qlabel.h>
#include "../../../src/insert.xpm"
#include "../../../src/edit.xpm"
#include "../../../src/delete.xpm"

IndexDialog::IndexDialog () : QTabDialog (0, "IndexDialog", TRUE)
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  toolbar = new Toolbar(w, 30, 30, FALSE);
  vbox->addWidget(toolbar);
  
  toolbar->addButton("add", insert, tr("Add Item"));
  QObject::connect(toolbar->getButton("add"), SIGNAL(clicked()), this, SLOT(addItem()));
  
  toolbar->addButton("edit", edit, tr("Edit"));
  QObject::connect(toolbar->getButton("edit"), SIGNAL(clicked()), this, SLOT(editItem()));
  
  toolbar->addButton("delete", deleteitem, tr("Delete"));
  QObject::connect(toolbar->getButton("delete"), SIGNAL(clicked()), this, SLOT(deleteItem()));
  
  vbox->addSpacing(10);
  
  QGridLayout *grid = new QGridLayout(vbox, 5, 2);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 0, 0);
  
  name = new QLineEdit(w);
  name->setReadOnly(TRUE);
  grid->addWidget(name, 0, 1);
  
  label = new QLabel(tr("Full Rebuild"), w);
  grid->addWidget(label, 1, 0);
  
  rebuild = new QCheckBox(w);
  grid->addWidget(rebuild, 1, 1);
  
  vbox->addSpacing(10);
  
  list = new QListView(w);
  list->addColumn(tr("Symbol"), 200);
  list->addColumn(tr("Weight"), -1);
  QObject::connect(list, SIGNAL(selectionChanged()), this, SLOT(buttonStatus()));
  vbox->addWidget(list);
  
  addTab(w, tr("Index"));

  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(accept()));
  
  buttonStatus();
  
  resize(325, 250);
}

IndexDialog::~IndexDialog ()
{
}

void IndexDialog::buttonStatus ()
{
  QListViewItem *item = list->selectedItem();
  if (! item)
  {
    toolbar->setButtonStatus("edit", FALSE);
    toolbar->setButtonStatus("delete", FALSE);
  }
  else
  {
    toolbar->setButtonStatus("edit", TRUE);
    toolbar->setButtonStatus("delete", TRUE);
  }
}

void IndexDialog::addItem ()
{
  double weight = 1;
  QString s;
  
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Add Index Item"));
  dialog->createPage (tr("Details"));
  dialog->addSymbolItem(tr("Symbol"), tr("Details"), config.getData(Config::DataPath), s);
  dialog->addFloatItem(tr("Weight"), tr("Details"), weight);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    s = dialog->getSymbol(tr("Symbol"));
    if (! s.length())
    {
      delete dialog;
      return;
    }
    
    weight = dialog->getFloat(tr("Weight"));
    
    QStringList l = QStringList::split("/", s, FALSE);
    symbolDict.insert(l[l.count() - 1], new QString(s));
    
    new QListViewItem(list, l[l.count() - 1], QString::number(weight));
    
    buttonStatus();
  }
  
  delete dialog;
}

void IndexDialog::editItem ()
{
  QListViewItem *item = list->selectedItem();
  if (! item)
    return;

  QString s = item->text(0);
  double weight = item->text(1).toFloat();
  
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Index Item"));
  dialog->createPage (tr("Details"));
  dialog->addSymbolItem(tr("Symbol"), tr("Details"), config.getData(Config::DataPath), s);
  dialog->addFloatItem(tr("Weight"), tr("Details"), weight);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    s = dialog->getSymbol(tr("Symbol"));
    if (! s.length())
    {
      delete dialog;
      return;
    }
    
    weight = dialog->getFloat(tr("Weight"));
    
    symbolDict.remove(item->text(0));
    QStringList l = QStringList::split("/", s, FALSE);
    symbolDict.insert(l[l.count() - 1], new QString(s));
    
    item->setText(0, l[l.count() - 1]);
    item->setText(1, QString::number(weight));
   
    buttonStatus();
  }
  
  delete dialog;
}

void IndexDialog::deleteItem ()
{
  QListViewItem *item = list->selectedItem();
  if (! item)
    return;

  symbolDict.remove(item->text(0));
  delete item;
 
  buttonStatus();
}

void IndexDialog::setList (QString d)
{
  list->clear();
  symbolDict.clear();
  
  QStringList l = QStringList::split(":", d, FALSE);
  
  int loop;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
    QStringList l2 = QStringList::split("/", l[loop], FALSE);
    symbolDict.insert(l2[l2.count() - 1], new QString(l[loop]));
    new QListViewItem(list, l2[l2.count() - 1], l[loop + 1]);
  }
    
  buttonStatus();
}

QString IndexDialog::getList ()
{
  QString s;
  
  if (! list->childCount())
    return s;
  
  QListViewItem *item = list->firstChild();
  
  while (item)
  {
    QString *sp = symbolDict[item->text(0)];
    s.append(sp->left(sp->length()));
    s.append(":");
    s.append(item->text(1));
    s.append(":");
    
    item = item->nextSibling();
  }
  
  return s;
}

void IndexDialog::setRebuild (bool d)
{
  rebuild->setChecked(d);
}

bool IndexDialog::getRebuild ()
{
  return rebuild->isChecked();
}

void IndexDialog::setName (QString d)
{
  name->setText(d);
}

