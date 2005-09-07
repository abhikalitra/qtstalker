/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include "HelpWindow.h"
#include <qlayout.h>
#include <qdir.h>
#include "../../../pics/insert.xpm"
#include "../../../pics/edit.xpm"
#include "../../../pics/delete.xpm"


IndexDialog::IndexDialog (QString &d) : QTabDialog (0, "IndexDialog", TRUE)
{
  helpFile = d;

  createDetailsPage();
  createParmsPage();
  
  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(accept()));

  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
    
  buttonStatus();
  
  resize(325, 250);
}

IndexDialog::~IndexDialog ()
{
}

void IndexDialog::createDetailsPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);
    
  QGridLayout *grid = new QGridLayout(vbox);
  grid->setMargin(0);
  grid->setSpacing(5);
  
  QLabel *label = new QLabel(tr("Symbol"), w);
  grid->addWidget(label, 0, 0);

  symbol = new QLabel(w);
  symbol->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid->addWidget(symbol, 0, 1);

  label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 1, 0);
  
  name = new QLineEdit(w);
  grid->addWidget(name, 1, 1);
  
  label = new QLabel(tr("Type"), w);
  grid->addWidget(label, 2, 0);
  
  type = new QLabel(w);
  type->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid->addWidget(type, 2, 1);
  
  label = new QLabel(tr("First Date"), w);
  grid->addWidget(label, 3, 0);
  
  fdate = new QLabel(w);
  fdate->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid->addWidget(fdate, 3, 1);
  
  label = new QLabel(tr("Last Date"), w);
  grid->addWidget(label, 4, 0);
  
  ldate = new QLabel(w);
  ldate->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid->addWidget(ldate, 4, 1);
  
  grid->setColStretch(1, 1);
  
  vbox->insertStretch(-1, 1);
  
  addTab(w, tr("Details"));  
}

void IndexDialog::createParmsPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  rebuild = new QCheckBox(tr("Full Rebuild"), w);
  vbox->addWidget(rebuild);
  
  vbox->addSpacing(10);
  
  QHBoxLayout *hbox = new QHBoxLayout(vbox);
  hbox->setSpacing(2);
  
  list = new QListView(w);
  list->addColumn(tr("Symbol"), 200);
  list->addColumn(tr("Weight"), -1);
  QObject::connect(list, SIGNAL(selectionChanged()), this, SLOT(buttonStatus()));
  hbox->addWidget(list);
  
  toolbar = new Toolbar(w, 30, 30, TRUE);
  hbox->addWidget(toolbar);

  QString s = "add";
  QString s2 = tr("Add Item");
  toolbar->addButton(s, insert, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(addItem()));
  
  s = "edit";
  s2 = tr("Edit");
  toolbar->addButton(s, edit, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(editItem()));
  
  s = "delete";
  s2 = tr("Delete");
  toolbar->addButton(s, deleteitem, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(deleteItem()));
  
  addTab(w, tr("Index"));
}

void IndexDialog::buttonStatus ()
{
  QString s = "edit";
  QString s2 = "delete";
  
  QListViewItem *item = list->selectedItem();
  if (! item)
  {
    toolbar->setButtonStatus(s, FALSE);
    toolbar->setButtonStatus(s2, FALSE);
  }
  else
  {
    toolbar->setButtonStatus(s, TRUE);
    toolbar->setButtonStatus(s2, TRUE);
  }
}

void IndexDialog::addItem ()
{
  double weight = 1;
  QString s;
  
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Add Index Item"));
  QString pl = tr("Details");
  dialog->createPage (pl);
  QString t = tr("Symbol");
  QString t2 = config.getData(Config::DataPath);
  dialog->addSymbolItem(t, pl, t2, s);
  t = tr("Weight");
  dialog->addDoubleItem(t, pl, weight);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    t = tr("Symbol");
    s = dialog->getSymbol(t);
    if (! s.length())
    {
      delete dialog;
      return;
    }
    
    t = tr("Weight");
    weight = dialog->getDouble(t);
    
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
  QString pl = tr("Details");
  dialog->createPage (pl);
  QString t = tr("Symbol");
  QString t2 = config.getData(Config::DataPath);
  dialog->addSymbolItem(t, pl, t2, s);
  t = tr("Weight");
  dialog->addDoubleItem(t, pl, weight);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    t = tr("Symbol");
    s = dialog->getSymbol(t);
    if (! s.length())
    {
      delete dialog;
      return;
    }
    
    t = tr("Weight");
    weight = dialog->getDouble(t);
    
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

void IndexDialog::setList (QString &d)
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

void IndexDialog::setName (QString &d)
{
  name->setText(d);
}

QString IndexDialog::getName ()
{
  return name->text();
}

void IndexDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

void IndexDialog::setSymbol (QString &d)
{
  symbol->setText(d);
}

void IndexDialog::setType (QString &d)
{
  type->setText(d);
}

void IndexDialog::setFirstDate (QString &d)
{
  fdate->setText(d);
}

void IndexDialog::setLastDate (QString &d)
{
  ldate->setText(d);
}


