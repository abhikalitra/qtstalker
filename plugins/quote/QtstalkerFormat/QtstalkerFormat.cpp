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

#include "QtstalkerFormat.h"
#include "ChartDb.h"
#include "PrefDialog.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstring.h>
#include <qdir.h>

QtstalkerFormat::QtstalkerFormat ()
{
  pluginName = "QtstalkerFormat";
  helpFile = "qtstalker.html";
}

QtstalkerFormat::~QtstalkerFormat ()
{
}

void QtstalkerFormat::update ()
{
  QTimer::singleShot(250, this, SLOT(parse()));
}

void QtstalkerFormat::parse ()
{
  QString path = createDirectory("Import");
  if (! path.length())
  {
    emit statusLogMessage(tr("Unable to create directory"));
    return;
  }
  path.append("/");

  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    QStringList l = QStringList::split("/", list[loop], FALSE);
    QString symbol = l[l.count() - 1];
    
    QString newPath = path;    
    newPath.append(symbol);

    QFile f(list[loop]);
    if (! f.open(IO_ReadOnly))
      continue;
    QTextStream stream(&f);
    
    while(stream.atEnd() == 0)
    {
      QString s = stream.readLine();
      s = s.stripWhiteSpace();
      if (! s.length())
        continue;

      QStringList l = QStringList::split("=", s, FALSE);
      if (l.count() < 2)
        continue;
	
      if (! l[0].compare("ChartPath"))
      {
        newPath = l[1];
	break;
      }
    }
    f.reset();
      
    // delete the db if it exists
    QDir dir;
    dir.remove(newPath, TRUE);
    
    ChartDb *db = new ChartDb;
    db->setPlugin("Stocks");
    if (db->openChart(newPath))
    {
      emit statusLogMessage("Could not open db.");
      delete db;
      continue;
    }
    
    QString s = tr("Updating ");
    s.append(symbol);
    emit statusLogMessage(s);
    
    while(stream.atEnd() == 0)
    {
      s = stream.readLine();
      s = s.stripWhiteSpace();

      if (! s.length())
        continue;

      QStringList l = QStringList::split("=", s, FALSE);
      if (l.count() < 2)
        continue;
	
      QString key = l[0];
      
      // remove this old relic from prev versions
      if (! key.compare("DETAILS"))
        continue;

      // temp variable so don't save it
      if (! l[0].compare("ChartPath"))
        continue;
		
      s = s.remove(0, key.length() + 1);
      
      db->setData(key, s);
    }

    f.close();
    delete db;
  }

  emit done();
  emit statusLogMessage(tr("Done"));
}

void QtstalkerFormat::prefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("QtstalkerFormat Prefs"));
  dialog->createPage (tr("Details"));
  dialog->setHelpFile(helpFile);
  dialog->addFileItem(tr("File Input"), tr("Details"));
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    list = dialog->getFile(tr("File Input"));
  }
  
  delete dialog;
}

QuotePlugin * createQuotePlugin ()
{
  QtstalkerFormat *o = new QtstalkerFormat;
  return ((QuotePlugin *) o);
}





