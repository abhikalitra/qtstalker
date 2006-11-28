/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#include "UpgradeMessage.h"
#include "Config.h"
#include "DbPlugin.h"
#include <qlayout.h>
#include <qwidget.h>
#include <qstring.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qpushbutton.h>


UpgradeMessage::UpgradeMessage (int type) : QTabDialog (0, "UpgradeMessage", TRUE)
{
  switch (type)
  {
    case V031:
      createPage031();
      break;
    case V034:
      createPage034();
      break;
    default:
      break;
  }
}

UpgradeMessage::~UpgradeMessage ()
{
}

void UpgradeMessage::createPage031 ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QString s = tr("This version of Qtstalker uses a new data format.");
  s.append(tr(" It can not read data files from previous versions."));
  s.append(tr(" This means that you will start with an empty workspace."));
  s.append(tr(" Your old data files have been preserved in $HOME/Qtstalker"));
  s.append(tr(" and can still be accessed by older Qtstalker versions."));
  s.append(tr(" If you don't intend to downgrade to a previous Qtstalker"));
  s.append(tr(" version, then you can remove that directory."));

  QTextEdit *message = new QTextEdit(w);
  message->setReadOnly(TRUE);
  message->setText(s);
  vbox->addWidget(message);

  check = new QCheckBox(tr("Do not show this message again."), w);
  vbox->addWidget(check);

  addTab(w, tr("Warning"));
  
  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
}

void UpgradeMessage::createPage034 ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QGridLayout *grid = new QGridLayout(vbox, 1, 2);
  grid->setColStretch(1, 1);

  QLabel *label = new QLabel(tr("Berkeley db_dump location"), w);
  grid->addWidget(label, 0, 0);

  QStringList l;
  QString s;
  fileButton = new FileButton(w, l, s);
  grid->addWidget(fileButton, 0, 1);

  label = new QLabel(tr("Progress"), w);
  grid->addWidget(label, 1, 0);

  progBar = new QProgressBar(w);
  grid->addWidget(progBar, 1, 1);

  addTab(w, tr("Chart Conversion"));

  QPushButton *button = new QPushButton(tr("Perform Conversion"), w);
  QObject::connect(button, SIGNAL(clicked()), this, SLOT(convert034()));
  vbox->addWidget(button);
  
//  setOkButton(tr("&OK"));

  setCancelButton(tr("&Cancel"));
}

bool UpgradeMessage::getStatus ()
{
  return check->isChecked();
}

void UpgradeMessage::convert034 ()
{
  QStringList l;
  fileButton->getFile(l);
  if (! l.count())
  {
    QMessageBox::information(this, tr("Chart Conversion Error"), tr("Berkeley db_dump executable not selected"));
    return;
  }

  QFileInfo fi(l[0]);
  if (fi.fileName().compare("db_dump"))
  {
    QMessageBox::information(this, tr("Chart Conversion Error"), tr("Berkeley db_dump executable not selected"));
    return;
  }

  // get all chart paths
  QStringList symbolList;
  Config config;
  QString dataPath;
  config.getData(Config::DataPath, dataPath);
  int t = dataPath.find("/data1/data", 0, TRUE);
  dataPath.replace(t + 5, 1, "0");
  traverse(dataPath, symbolList);
  
  int loop;
  for (loop = 0; loop < (int) symbolList.count(); loop++)
  {
    progBar->setProgress(loop, (int) symbolList.count());

    QString s = l[0] + " -p -f /tmp/qtstalker_db " + symbolList[loop];
    if (system(s.latin1()))
    {
      qDebug("UpgradeMessage::convert034: error from db_dump");
      continue;
    }

    QString path;
    getChartPathType(path);
    if (! path.length())
      continue;

    if (createDir(path))
      continue;

    qDebug("Converting %s", path.latin1());
    if (createChart(path))
      continue;
  }

  copyFiles();

  progBar->setProgress((int) symbolList.count(), (int) symbolList.count());

  qDebug("Conversion complete");
}

bool UpgradeMessage::createChart (QString &path)
{
  DbPlugin db;
  if (db.openChart(path))
    return TRUE;

  db.createTables();
  db.setHeaderField(DbPlugin::Path, path);

  QFile inFile("/tmp/qtstalker_db");
  if (! inFile.open(IO_ReadOnly))
  {
    qDebug("UpgradeMessage::loadFile: error opening input file");
    return TRUE;
  }
  QTextStream inStream(&inFile);
  
  // skip past header data
  while(inStream.atEnd() == 0)
  {
    QString s = inStream.readLine();
    s = s.stripWhiteSpace();
    if (s.contains("HEADER=END"))
      break;
  }

  // start the import
  while(inStream.atEnd() == 0)
  {
    QString s = inStream.readLine();
    s = s.stripWhiteSpace();
    s = s.left(s.length() - 3);

    QString s2 = inStream.readLine();
    s2 = s2.stripWhiteSpace();
    s2 = s2.left(s2.length() - 3);

    bool ok = FALSE;
    while (1)
    {
      // is this a quote?
      double t = s.toDouble(&ok);
      if (ok)
      {
        if (t < 10000)
        {
          // its a chart object
          Setting t;
          t.parse(s2);
          db.setChartObject(s, t);
          break;
        }
        else
        {
          // its a quote
          QStringList l = QStringList::split(",", s2, FALSE);
          QString q;
          if (l.count() == 5)
            q = "REPLACE INTO data (date,open,high,low,close,volume,oi) VALUES (" + s + "," + l[0] + "," + l[1] + "," + l[2] +
                "," + l[3] + "," + l[4] + ",0)";
          else
            q = "REPLACE INTO data (date,open,high,low,close,volume,oi) VALUES (" + s + "," + l[0] + "," + l[1] + "," + l[2] +
                "," + l[3] + "," + l[4] + "," + l[5] + ")";
          db.setData(q);
          break;
        }
      }

      if (! s.compare("Type"))
      {
        db.setHeaderField(DbPlugin::Type, s2);
        break;
      }

      if (! s.compare("FuturesType"))
      {
        db.setHeaderField(DbPlugin::FuturesType, s2);
        break;
      }

      if (! s.compare("FuturesMonth"))
      {
        db.setHeaderField(DbPlugin::FuturesMonth, s2);
        break;
      }

      if (! s.compare("BarType"))
      {
        db.setHeaderField(DbPlugin::BarType, s2);
        break;
      }

      if (! s.compare("Fundamentals"))
      {
        db.setHeaderField(DbPlugin::Fundamentals, s2);
        break;
      }

      if (! s.compare("LocalIndicators"))
      {
        db.setHeaderField(DbPlugin::LocalIndicators, s2);
        break;
      }

      if (! s.compare("QuotePlugin"))
      {
        db.setHeaderField(DbPlugin::QuotePlugin, s2);
        break;
      }

      if (! s.compare("Symbol"))
      {
        db.setHeaderField(DbPlugin::Symbol, s2);
        break;
      }

      if (! s.compare("Title"))
      {
        db.setHeaderField(DbPlugin::Title, s2);
        break;
      }

      if (! s.compare("SpreadFirstSymbol"))
      {
        int t = s2.find("/data0/data", 0, TRUE);
        s2.replace(t + 5, 1, "1");
        db.setHeaderField(DbPlugin::SpreadFirstSymbol, s2);
        break;
      }

      if (! s.compare("SpreadSecondSymbol"))
      {
        int t = s2.find("/data0/data", 0, TRUE);
        s2.replace(t + 5, 1, "1");
        db.setHeaderField(DbPlugin::SpreadSecondSymbol, s2);
        break;
      }

      if (! s.compare("IndexList"))
      {
        while (1)
        {
          int t = s2.find("/data0/data", 0, TRUE);
          if (t == -1)
            break;
          else
            s2.replace(t + 5, 1, "1");
        }
        db.setHeaderField(DbPlugin::IndexList, s2);
        break;
      }

      if (! s.compare("CCAdjustment"))
      {
        db.setHeaderField(DbPlugin::CCAdjustment, s2);
        break;
      }

      break;
    }
  }
  inFile.close();

  db.close();

  return FALSE;
}


bool UpgradeMessage::createDir (QString &path)
{
  int t = path.find("/data0/data", 0, TRUE);
  path.replace(t + 5, 1, "1");
  
  QStringList l = QStringList::split("/", path, FALSE);
  int loop = l.findIndex(".qtstalker");
  loop = loop + 2;
  for (; loop < (int) l.count() - 1; loop++)
  {
    QString s;
    int loop2;
    for (loop2 = 0; loop2 <= loop; loop2++)
      s.append("/" + l[loop2]);

    QDir dir(s);
    if (! dir.exists(s, TRUE))
    {
      if (! dir.mkdir(s, TRUE))
      {
        qDebug("UpgradeMessage::createDir: could not create directory structure for chart");
        return TRUE;
      }
    }
  }

  return FALSE;
}

void UpgradeMessage::getChartPathType (QString &path)
{
  QFile inFile("/tmp/qtstalker_db");
  if (! inFile.open(IO_ReadOnly))
  {
    qDebug("UpgradeMessage::loadFile: error opening input file");
    return;
  }
  QTextStream inStream(&inFile);
  
  // skip past header data
  while(inStream.atEnd() == 0)
  {
    QString s = inStream.readLine();
    s = s.stripWhiteSpace();
    s.truncate(s.length() - 3);

    if (! s.compare("Path"))
    {
      path = inStream.readLine();
      path = path.stripWhiteSpace();
      path.truncate(path.length() - 3);
    }
  }

  inFile.close();
}

void UpgradeMessage::traverse (QString dirname, QStringList &l)
{
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
      traverse(fi->absFilePath(), l);
    else
      l.append(fi->absFilePath());

    ++it;
  }
}

void UpgradeMessage::copyFiles ()
{
  QString oldPath, newPath;
  Config config;
  config.getData(Config::Home, oldPath);
  newPath = oldPath;
  oldPath.truncate(oldPath.length() - 1);
  oldPath.append("0");

  // copy group files
  qDebug("Converting group files...");
  QString s = "cp -R " + oldPath + "/group " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying files");

  QStringList l;
  s = newPath + "/group";
  traverse(s, l);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QFileInfo fi(l[loop]);
    QString s2 = fi.readLink();
    int t = s2.find("/data0/data", 0, TRUE);
    s2.replace(t + 5, 1, "1");
    s = "ln -s -f " + s2 + " " + l[loop];
    if (system(s.latin1()))
      qDebug("UpgradeMessage::copyFiles: error creating symlink for group file");
  }

  // copy indicators
  qDebug("Converting indicator files...");
  s = "cp -R " + oldPath + "/indicator " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying indicator files");
  l.clear();
  s = newPath + "/indicator";
  traverse(s, l);
  correctPathFiles(l);

  // copy plugin files
  qDebug("Converting plugin files...");
  s = "cp -R " + oldPath + "/plugin " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying indicator files");

  // copy portfolio files
  qDebug("Converting portfolio files...");
  s = "cp -R " + oldPath + "/portfolio " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying portfolio files");
  l.clear();
  s = newPath + "/portfolio";
  traverse(s, l);
  correctPathFiles(l);

  // copy scanner files
  qDebug("Converting scanner files...");
  s = "cp -R " + oldPath + "/scanner " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying scanner files");
  l.clear();
  s = newPath + "/scanner";
  traverse(s, l);
  correctPathFiles(l);

  // copy test files
  qDebug("Converting test files...");
  s = "cp -R " + oldPath + "/test " + newPath;
  if (system(s.latin1()))
    qDebug("UpgradeMessage::copyFiles: error copying test files");
  l.clear();
  s = newPath + "/test";
  traverse(s, l);
  correctPathFiles(l);
}

void UpgradeMessage::correctPathFiles (QStringList &list)
{
  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    QFile inFile(list[loop]);
    if (! inFile.open(IO_ReadOnly))
    {
      qDebug("UpgradeMessage::correctPathFiles: error opening input file");
      continue;
    }
    QTextStream inStream(&inFile);
  
    QStringList l;
    while(inStream.atEnd() == 0)
    {
      QString s = inStream.readLine();
      s = s.stripWhiteSpace();
      if (! s.length())
        continue;
      l.append(s);
    }
    inFile.close();

    QFile outFile(list[loop]);
    if (! outFile.open(IO_WriteOnly))
    {
      qDebug("UpgradeMessage::correctPathFiles: error opening output file");
      continue;
    }
    QTextStream outStream(&outFile);

    int loop2;
    for (loop2 = 0; loop2 < (int) l.count(); loop2++)
    {
      int t = l[loop2].find("/data0/data", 0, TRUE);
      if (t != -1)
        l[loop2].replace(t + 5, 1, "1");
      outStream << l[loop2] << "\n";
    }

    outFile.close();
  }
}


