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

#include "ChartPage.h"
#include "EditChartDialog.h"
#include "ChartDb.h"
#include "Setting.h"
#include "SymbolDialog.h"
#include "edit.xpm"
#include "delete.xpm"
#include "export.xpm"
#include <qmessagebox.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qlayout.h>

ChartPage::ChartPage (QWidget *w, Config *c) : QWidget (w)
{
  config = c;

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  search = new QLineEdit("*", this);
  connect(search, SIGNAL(textChanged(const QString &)), this, SLOT(searchChanged(const QString &)));
  QToolTip::add(search, tr("List Filter, e.g. s* or sb*"));
  vbox->addWidget(search);

  nav = new Navigator(this, config->getData(Config::DataPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(chartSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(chartNoSelection()));
  connect(nav, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(QListBoxItem *)));
  nav->updateList();
  vbox->addWidget(nav);

  menu = new QPopupMenu();
  menu->insertItem(QPixmap(edit), tr("&Edit Chart"), this, SLOT(editChart()), CTRL+Key_E);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Chart"), this, SLOT(deleteChart()), CTRL+Key_D);
  menu->insertItem(QPixmap(exportfile), tr("E&xport Chart"), this, SLOT(exportSymbol()), CTRL+Key_X);

  chartNoSelection();
}

ChartPage::~ChartPage ()
{
  delete menu;
}

void ChartPage::deleteChart ()
{
  SymbolDialog *dialog = new SymbolDialog(this,nav->getCurrentPath(), "*");
  dialog->setCaption(tr("Select Charts To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
    			      tr("Qtstalker: Warning"),
			      tr("Are you sure you want to delete selected charts?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    QStringList l = dialog->selectedFiles();
    int loop;
    QDir dir;
    for (loop = 0; loop < (int) l.count(); loop++)
      dir.remove(l[loop], TRUE);
      
    nav->updateList();
  }

  delete dialog;
}

void ChartPage::editChart ()
{
  QString symbol = nav->getFileSelection();
  if (! symbol.length())
    return;

  EditChartDialog *dialog = new EditChartDialog(symbol);

  dialog->exec();

  delete dialog;
}

void ChartPage::exportSymbol ()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  							   config->getData(Config::DataPath),
							   "*");
  dialog->setCaption(tr("Select Charts"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QString s = config->getData(Config::Home);
    s.append("/export");
    QDir dir(s);
    if (! dir.exists(s, TRUE))
    {
      if (! dir.mkdir(s, TRUE))
      {
        qDebug("Unable to create export directory.");
	delete dialog;
        return;
      }
    }

    QStringList l = dialog->selectedFiles();
    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
      exportChart(l[loop]);
  }

  delete dialog;
}

void ChartPage::exportChart (QString path)
{
  ChartDb *db = new ChartDb();
  if(db->openChart(path))
  {
    delete db;
    return;
  }

  QString s = config->getData(Config::Home);
  s.append("/export/");
  
  QString s2 = db->getDetail(ChartDb::Symbol);
  if (! s2.length())
  {
    QStringList l = QStringList::split("/", path, FALSE);
    s.append(l[l.count() - 1]);
  }
  else
    s.append(s2);

  db->dump(s);

  delete db;
}

void ChartPage::chartSelected (QString d)
{
  menu->setItemEnabled(menu->idAt(0), TRUE);
  emit fileSelected(d);
}


void ChartPage::chartNoSelection ()
{
  menu->setItemEnabled(menu->idAt(0), FALSE);
}

void ChartPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void ChartPage::refreshList ()
{
  nav->updateList();
}

void ChartPage::searchChanged (const QString &d)
{
  nav->setFilter(d);
}


