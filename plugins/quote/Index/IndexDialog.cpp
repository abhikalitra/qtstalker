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
#include "ChartDb.h"
#include "PrefDialog.h"
#include "SymbolDialog.h"
#include <qinputdialog.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qlayout.h>

IndexDialog::IndexDialog () : QTabDialog (0, "IndexDialog", TRUE)
{
  symbolDict.setAutoDelete(TRUE);
  
  dataPath = QDir::homeDirPath();
  dataPath.append("/Qtstalker/data/Index");

  saveFlag = FALSE;
  
  QWidget *w = new QWidget(this);
  
  QHBoxLayout *hbox = new QHBoxLayout(w);
  hbox->setMargin(5);
  hbox->setSpacing(5);
  
  QVBoxLayout *vbox = new QVBoxLayout(hbox);
  
  QGridLayout *grid = new QGridLayout(vbox, 2, 1);
  
  QLabel *label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 0, 0);
  
  name = new QLabel(w);
  name->setFrameStyle(QFrame::WinPanel || QFrame::Sunken);
  grid->addWidget(name, 0, 1);
  
  list = new QListView(w);
  list->addColumn(tr("Symbol"), 200);
  list->addColumn(tr("Weight"), -1);
  QObject::connect(list, SIGNAL(selectionChanged()), this, SLOT(buttonStatus()));
  vbox->addWidget(list);
  
  grid = new QGridLayout(hbox, 7, 1);
  grid->setMargin(5);
  grid->setSpacing(5);
  
  newButton = new QPushButton(tr("New"), w);
  QObject::connect(newButton, SIGNAL(pressed()), this, SLOT(newIndex()));
  grid->addWidget(newButton, 0, 0);
  
  openButton = new QPushButton(tr("Open"), w);
  QObject::connect(openButton, SIGNAL(pressed()), this, SLOT(openIndex()));
  grid->addWidget(openButton, 1, 0);
  
  saveButton = new QPushButton(tr("Save"), w);
  QObject::connect(saveButton, SIGNAL(pressed()), this, SLOT(saveIndex()));
  grid->addWidget(saveButton, 2, 0);
  
  addButton = new QPushButton(tr("Add"), w);
  QObject::connect(addButton, SIGNAL(pressed()), this, SLOT(addItem()));
  grid->addWidget(addButton, 3, 0);
  
  editButton = new QPushButton(tr("Edit"), w);
  QObject::connect(editButton, SIGNAL(pressed()), this, SLOT(editItem()));
  grid->addWidget(editButton, 4, 0);

  deleteButton = new QPushButton(tr("Delete"), w);
  QObject::connect(deleteButton, SIGNAL(pressed()), this, SLOT(deleteItem()));
  grid->addWidget(deleteButton, 5, 0);
  
  addTab(w, tr("Index"));
  
  setOkButton();
  QObject::connect(this, SIGNAL(applyButtonPressed()), this, SLOT(okPressed()));
  setCancelButton();
  
  buttonStatus();
}

IndexDialog::~IndexDialog ()
{
}

void IndexDialog::addItem ()
{
  double weight = 1;
  QString s;
  
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Add Index Item"));
  dialog->createPage (tr("Details"));
  
  QString tpath = QDir::homeDirPath();
  tpath.append("/Qtstalker/data");
  dialog->addSymbolItem(tr("Symbol"), 1, tpath, s);
  
  dialog->addFloatItem(tr("Weight"), 1, weight);
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
    
    saveFlag = TRUE;
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
  
  QString tpath = QDir::homeDirPath();
  tpath.append("/Qtstalker/data");
  dialog->addSymbolItem(tr("Symbol"), 1, tpath, s);
  
  dialog->addFloatItem(tr("Weight"), 1, weight);
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
   
    saveFlag = TRUE;
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
 
  saveFlag = TRUE;
  buttonStatus();
}

void IndexDialog::buttonStatus ()
{
  if (! name->text().length())
  {
    saveButton->setEnabled(FALSE);
    editButton->setEnabled(FALSE);
    deleteButton->setEnabled(FALSE);
    addButton->setEnabled(FALSE);
    return;
  }
  else
    addButton->setEnabled(TRUE);

  QListViewItem *item = list->selectedItem();
  if (! item)
  {
    editButton->setEnabled(FALSE);
    deleteButton->setEnabled(FALSE);
  }
  else
  {
    editButton->setEnabled(TRUE);
    deleteButton->setEnabled(TRUE);
  }
  
  if (saveFlag)
    saveButton->setEnabled(TRUE);
  else
    saveButton->setEnabled(FALSE);
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

void IndexDialog::newIndex ()
{
  if (saveFlag)
  {
    int rc = QMessageBox::warning(this,
    			          tr("Qtstalker: Warning"),
			          tr("This Index has been modified. Save changes?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::Yes)
      saveIndex();
  }
  
  bool ok = FALSE;
  QString index = QInputDialog::getText(tr("New Index"),
                                        tr("Enter symbol name for the new Index"),
					QLineEdit::Normal,
					QString::null,
					&ok,
					this);
  if (! index.length() || ok == FALSE)
    return;

  QDir dir;
  QString s = dataPath;
  s.append("/");
  s.append(index);
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This index already exists."));
    return;
  }
  
  symbol = index;
  name->setText(index);
  file = s;
  
  setList(QString());
  
  buttonStatus();
}

void IndexDialog::openIndex ()
{
  if (saveFlag)
  {
    int rc = QMessageBox::warning(this,
    			          tr("Qtstalker: Warning"),
			          tr("This Index has been modified. Save changes?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::Yes)
      saveIndex();
  }

  SymbolDialog *dialog = new SymbolDialog(this, dataPath, "*");
  dialog->setCaption(tr("Select Index"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QStringList l = dialog->selectedFiles();
    if (l.count())
    {
      file = l[0];
      
      ChartDb *db = new ChartDb();
      if (db->openChart(file))
      {
        qDebug("could not open db");
        delete db;
	delete dialog;
        return;
      }
      
      Setting *details = db->getDetails();
      setList(details->getData("Index"));
      symbol = details->getData("Symbol");
      name->setText(symbol);
      delete db;
      
      buttonStatus();
    }
  }

  delete dialog;
}

void IndexDialog::saveIndex ()
{
  QDir dir;
  if (! dir.exists(dataPath))
  {
    if (! dir.mkdir(dataPath))
    {
      qDebug("Index plugin: Unable to create directory");
      return;
    }
  }

  ChartDb *db = new ChartDb();
  if (db->openChart(file))
  {
    qDebug("could not open db");
    delete db;
    return;
  }
  
  Setting *details = db->getDetails();
  if (! details->count())
  {
    details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
    details->set("Chart Type", "Index", Setting::None);
    details->set("Symbol", symbol, Setting::None);
    details->set("Title", symbol, Setting::Text);
    details->set("Index", getList(), Setting::Text);
    db->saveDetails();
  }
  
  details->setData("Index", getList());
  db->saveDetails();
  delete db;

  saveFlag = FALSE;
  buttonStatus();
}

void IndexDialog::okPressed ()
{
  if (saveFlag)
  {
    int rc = QMessageBox::warning(this,
    			          tr("Qtstalker: Warning"),
			          tr("This Index has been modified. Save changes?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::Yes)
      saveIndex();
  }

  accept();
}

