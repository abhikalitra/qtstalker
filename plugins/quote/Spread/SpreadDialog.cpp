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

#include "SpreadDialog.h"
#include "ChartDb.h"
#include "SymbolDialog.h"
#include <qinputdialog.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include "../../../src/newchart.xpm"
#include "../../../src/openchart.xpm"
#include "../../../src/filesave.xpm"

SpreadDialog::SpreadDialog () : QTabDialog (0, "SpreadDialog", TRUE)
{
  dataPath = QDir::homeDirPath();
  QString tpath = dataPath;
  tpath.append("/Qtstalker/data");
  dataPath.append("/Qtstalker/data/Spread");

  saveFlag = FALSE;
  
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  toolbar = new Toolbar(w, 30, 30);
  vbox->addWidget(toolbar);
  
  toolbar->addButton("new", newchart, tr("New"));
  QObject::connect(toolbar->getButton("new"), SIGNAL(pressed()), this, SLOT(newSpread()));
  
  toolbar->addButton("open", openchart, tr("Open"));
  QObject::connect(toolbar->getButton("open"), SIGNAL(pressed()), this, SLOT(openSpread()));
  
  toolbar->addButton("save", filesave, tr("Save"));
  QObject::connect(toolbar->getButton("save"), SIGNAL(pressed()), this, SLOT(saveSpread()));
  
  vbox->addSpacing(10);
  
  QGridLayout *grid = new QGridLayout(vbox, 5, 2);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 0, 0);
  
  name = new QLineEdit(w);
  name->setReadOnly(TRUE);
  grid->addWidget(name, 0, 1);
  
  label = new QLabel(tr("Method"), w);
  grid->addWidget(label, 1, 0);
  
  method = new QComboBox(w);
  method->insertItem(tr("Subtract"), -1);
  method->insertItem(tr("Divide"), -1);
  QObject::connect(method, SIGNAL(activated(int)), this, SLOT(methodChanged(int)));
  grid->addWidget(method, 1, 1);
  
  label = new QLabel(tr("First Symbol"), w);
  grid->addWidget(label, 2, 0);

  firstButton = new SymbolButton(w, tpath, "");
  QObject::connect(firstButton, SIGNAL(symbolChanged()), this, SLOT(dataChanged()));
  grid->addWidget(firstButton, 2, 1);

  label = new QLabel(tr("Second Symbol"), w);
  grid->addWidget(label, 3, 0);

  secondButton = new SymbolButton(w, tpath, "");
  QObject::connect(secondButton, SIGNAL(symbolChanged()), this, SLOT(dataChanged()));
  grid->addWidget(secondButton, 3, 1);
  
  addTab(w, tr("Spread"));

  setOkButton();
  setCancelButton();
    
  buttonStatus();
  
  resize(325, 250);
}

SpreadDialog::~SpreadDialog ()
{
}

void SpreadDialog::buttonStatus ()
{
  if (saveFlag)
    toolbar->setButtonStatus("save", TRUE);
  else
    toolbar->setButtonStatus("save", FALSE);
    
  if (! name->text().length())
  {
    firstButton->setEnabled(FALSE);
    secondButton->setEnabled(FALSE);
    method->setEnabled(FALSE);
  }
  else
  {
    firstButton->setEnabled(TRUE);
    secondButton->setEnabled(TRUE);
    method->setEnabled(TRUE);
  }
}

void SpreadDialog::newSpread ()
{
  if (saveFlag)
  {
    int rc = QMessageBox::warning(this,
    			          tr("Qtstalker: Warning"),
			          tr("This Spread has been modified. Save changes?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::Yes)
      saveSpread();
  }
  
  bool ok = FALSE;
  QString spread = QInputDialog::getText(tr("New Spread"),
                                         tr("Enter symbol name for the new Spread"),
					 QLineEdit::Normal,
					 QString::null,
					 &ok,
					 this);
  if (! spread.length() || ok == FALSE)
    return;

  QDir dir;
  QString s = dataPath;
  s.append("/");
  s.append(spread);
  if (dir.exists(s))
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     tr("This Spread already exists."));
    return;
  }
  
  symbol = spread;
  name->setText(spread);
  file = s;  
  
  firstButton->setSymbol("");
  secondButton->setSymbol("");
  
  buttonStatus();
}

void SpreadDialog::openSpread ()
{
  if (saveFlag)
  {
    int rc = QMessageBox::warning(this,
    			          tr("Qtstalker: Warning"),
			          tr("This Spread has been modified. Save changes?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::Yes)
      saveSpread();
  }

  SymbolDialog *dialog = new SymbolDialog(this, dataPath, "*");
  dialog->setCaption(tr("Select Spread"));

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
      firstButton->setSymbol(details->getData("First Symbol"));
      secondButton->setSymbol(details->getData("Second Symbol"));
      method->setCurrentText(details->getData("Method"));
      symbol = details->getData("Symbol");
      name->setText(symbol);
      delete db;
    }
  }

  delete dialog;
  
  buttonStatus();
}

void SpreadDialog::saveSpread ()
{
  QDir dir;
  if (! dir.exists(dataPath))
  {
    if (! dir.mkdir(dataPath))
    {
      qDebug("Spread plugin: Unable to create directory");
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
    details->set("Chart Type", "Spread", Setting::None);
    details->set("Method", method->currentText(), Setting::None);
    details->set("Symbol", symbol, Setting::None);
    details->set("Title", symbol, Setting::Text);
    details->set("First Symbol", firstButton->getPath(), Setting::Symbol);
    details->set("Second Symbol", secondButton->getPath(), Setting::Symbol);
    db->saveDetails();
  }
  
  details->set("First Symbol", firstButton->getPath(), Setting::Symbol);
  details->set("Second Symbol", secondButton->getPath(), Setting::Symbol);
  details->set("Method", method->currentText(), Setting::None);
  db->saveDetails();
  delete db;

  saveFlag = FALSE;
  
  buttonStatus();
}

void SpreadDialog::dataChanged ()
{
  saveFlag = TRUE;
  buttonStatus();
}

void SpreadDialog::methodChanged (int)
{
  saveFlag = TRUE;
  buttonStatus();
}

