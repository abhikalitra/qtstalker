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

#include <qlayout.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qhgroupbox.h>
#include <qprogressdialog.h>
#include <stdlib.h>
#include "Scanner.h"
#include "BarData.h"
#include "SymbolDialog.h"
#include "DbPlugin.h"
#include "IndicatorPlugin.h"
#include "HelpWindow.h"

Scanner::Scanner (QString n) : QTabDialog (0, 0, FALSE)
{
  scannerName = n;
  
  QString s = "Qtstalker Scanner";
  s.append(": ");
  s.append(scannerName);
  setCaption (s);

  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);
  
  QHGroupBox *gbox = new QHGroupBox(tr("Symbol Selection"), w);  
  vbox->addWidget(gbox);

  allSymbols = new QCheckBox(tr("All symbols"), gbox);
  connect(allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsToggled(bool)));

  fileButton = new QPushButton(tr("0 Symbols"), gbox);
  connect(fileButton, SIGNAL(clicked()), this, SLOT(getSymbols()));

  gbox = new QHGroupBox(tr("Compression"), w);  
  vbox->addWidget(gbox);
  
  BarData bd;
  period = new QComboBox(gbox);
  bd.getBarCompressionList(compressionList);
  period->insertStringList(compressionList, -1);

  list = new FormulaEdit(w, FormulaEdit::Logic);
  vbox->addWidget(list);
  
  setDefaultButton(tr("&Scan"));
  connect(this, SIGNAL(defaultButtonPressed()), this, SLOT(scan()));
  
  setApplyButton(tr("&Apply"));  
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveRule()));
  
  setCancelButton(tr("&Cancel"));  
  connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(exitDialog()));
  
  setOkButton(QString::null);  
  
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(slotHelp()));
  
  addTab(w, tr("Parms"));

  loadRule();
}

Scanner::~Scanner ()
{
}

void Scanner::scan ()
{
  if (! fileList.count() && ! allSymbols->isChecked())
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Error"),
			     tr("No symbols selected."));
    return;
  }

  // open the CUS plugin
  QString iplugin("CUS");
  IndicatorPlugin *plug = config.getIndicatorPlugin(iplugin);
  if (! plug)
  {
    config.closePlugin(iplugin);
    return;
  }

  QStringList l = QStringList::split("\n", list->getText(), FALSE);
  plug->setCustomFunction(l);
  
  this->setEnabled(FALSE);
  
  // clear dir for scan symbols
  QDir dir;
  QString s = config.getData(Config::GroupPath);
  s.append("/Scanner");
  if (! dir.exists(s, TRUE))
    dir.mkdir(s, TRUE);
  s.append("/");
  s.append(scannerName);
  if (! dir.exists(s, TRUE))
    dir.mkdir(s, TRUE);
  else
  {
    int loop;
    for (loop = 2; loop < (int) dir.count(); loop++)
    {
      QString s2 = dir.absPath(); + "/" + dir[loop];
      dir.remove(s2, TRUE);
    }
  }
  
  if (allSymbols->isChecked())
  {
    Traverse *trav = new Traverse();
    trav->traverse(config.getData(Config::DataPath) + "/Stocks");
    fileList = trav->getList();
    delete trav;
  }
  
  QProgressDialog prog(tr("Scanning..."),
                       tr("Cancel"),
		       fileList.count(),
		       this,
		       "progress",
		       TRUE);
  prog.show();
  
//  int minBars = plug->getMinBars();
  int minBars = 100; // FIXME gotta find a new way to get minbars before we calculate
  
  emit message(QString("Scanning..."));
  
  int loop;
  for (loop = 0; loop < (int) fileList.count(); loop++)
  {
    prog.setProgress(loop);
    emit message(QString());
    if (prog.wasCancelled())
    {
      emit message(QString("Scan cancelled"));
      break;
    }

    QString plugin = config.parseDbPlugin(fileList[loop]);
    DbPlugin *db = config.getDbPlugin(plugin);
    if (! db)
    {
      config.closePlugin(plugin);
      continue;
    }

    QDir dir;
    if (! dir.exists(fileList[loop]))
    {
      config.closePlugin(plugin);
      continue;
    }
    db->openChart(fileList[loop]);

    if (! period->currentText().compare(tr("Daily")))
      db->setBarCompression(BarData::DailyBar);
    else
    {
      if (! period->currentText().compare(tr("Weekly")))
        db->setBarCompression(BarData::WeeklyBar);
      else
        db->setBarCompression(BarData::MonthlyBar);
    }
    
    db->setBarRange(minBars);
    db->setBarCompression((BarData::BarCompression) compressionList.findIndex(period->currentText()));

    BarData *recordList = new BarData;
    db->getHistory(recordList);
    
    // load the CUS plugin and calculate
    plug->clearOutput();
    plug->setIndicatorInput(recordList);
    plug->calculate();
    Indicator *i = plug->getIndicator();
    if (! i->getLines())
    {
      delete recordList;
      config.closePlugin(plugin);
      continue;
    }
    
    PlotLine *line = i->getLine(0);
    if (line && line->getSize() > 0)
    {
      if (line->getData(line->getSize() - 1) > 0)
      {
        QString s = "ln -s ";
        s.append(fileList[loop]);
        s.append(" ");
        s.append(config.getData(Config::GroupPath));
        s.append("/Scanner/");
        s.append(scannerName);
        system (s);
      }
    }
    
    delete recordList;
    config.closePlugin(plugin);
    
    emit message(QString());
  }
  
  if (! prog.wasCancelled())
    emit message(QString("Scan complete"));
  
  config.closePlugin(iplugin);
  
  this->setEnabled(TRUE);
}

void Scanner::saveRule ()
{
  QString s = config.getData(Config::ScannerPath);
  s.append("/");
  s.append(scannerName);

  QFile f(s);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);
  
  stream << "allSymbols=" << QString::number(allSymbols->isChecked()) << "\n";
  stream << "compression=" << period->currentText() << "\n";
  
  int loop;
  for (loop = 0; loop < (int) fileList.count(); loop++)
    stream << "symbol=" << fileList[loop] << "\n";
  
  QStringList l = QStringList::split("\n", list->getText(), FALSE);
  stream << "script=" << l.join("|") << "\n";
  
  f.close();
}

void Scanner::loadRule ()
{
  QString s = config.getData(Config::ScannerPath);
  s.append("/");
  s.append(scannerName);

  QFile f(s);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  fileList.clear();
  
  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();

    if (! s.length())
      continue;
      
    QString key;
    QString dat;
    if (s.contains("="))
    {
      int t = s.find("=", 0, TRUE);
      key = s.left(t);
      dat = s.right(s.length() - t - 1);
    }
    else
      continue;

    if (! key.compare("allSymbols"))
    {
      allSymbols->setChecked(dat.toInt());
      continue;
    }
    
    if (! key.compare("compression"))
    {
      period->setCurrentText(dat);
      continue;
    }
    
    if (! key.compare("symbol"))
    {
      fileList.append(dat);
      continue;
    }

    if (! key.compare("script"))
    {
      QStringList l2 = QStringList::split("|", dat, FALSE);
      int loop;
      for (loop = 0; loop < (int) l2.count(); loop++)
        list->setLine(l2[loop]);
    }
  }

  fileButton->setText(QString::number(fileList.count()) + " Symbols");
  
  f.close();
}

void Scanner::exitDialog ()
{
  reject();
  emit exitScanner();
}

void Scanner::getSymbols ()
{
  QString s(config.getData(Config::DataPath));
  QString s2("*");
  SymbolDialog *dialog = new SymbolDialog(this,
  					  s,
					  s2,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select symbols to scan"));
  
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
    fileList = dialog->selectedFiles();
    
  fileButton->setText(QString::number(fileList.count()) + " Symbols");
    
  delete dialog;
}

void Scanner::allSymbolsToggled (bool d)
{
  if (d)
    fileButton->setEnabled(FALSE);
  else
    fileButton->setEnabled(TRUE);
}

void Scanner::slotHelp ()
{
  QString s = "workwithscanner.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}

//******************************************************
//**************** TRAVERSE ****************************
//******************************************************

Traverse::Traverse()
{
}

Traverse::~Traverse()
{
}

void Traverse::traverse(QString dirname)
{
  list.clear();
  
  QDir dir(dirname);
  dir.setFilter(QDir::Dirs|QDir::Files);

  const QFileInfoList *fileinfolist = dir.entryInfoList();
  QFileInfoListIterator it(*fileinfolist);
  QFileInfo *fi;
  while((fi = it.current()))
  {
    if(fi->fileName() == "." || fi->fileName() == "..")
    {
      ++it;
      continue;
    }

    if(fi->isDir() && fi->isReadable())
      traverse(fi->absFilePath());
    else
      list.append(fi->absFilePath());

    ++it;
  }
}

QStringList Traverse::getList()
{
  return list;
}


