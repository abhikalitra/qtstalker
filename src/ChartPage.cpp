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
#include "Setting.h"
#include "SymbolDialog.h"
#include "ChartDb.h"
#include "HelpWindow.h"
#include "edit.xpm"
#include "delete.xpm"
#include "export.xpm"
#include "newchart.xpm"
#include "help.xpm"
#include <qmessagebox.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qlayout.h>

ChartPage::ChartPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  search = new QLineEdit("*", this);
  connect(search, SIGNAL(textChanged(const QString &)), this, SLOT(searchChanged(const QString &)));
  QToolTip::add(search, tr("List Filter, e.g. s* or sb*"));
  vbox->addWidget(search);

  nav = new Navigator(this, config.getData(Config::DataPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(chartSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(chartNoSelection()));
  connect(nav, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(QListBoxItem *)));
  nav->updateList();
  vbox->addWidget(nav);

  newMenu = new QPopupMenu;
  int id = newMenu->insertItem(QPixmap(newchart), tr("CC"), this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  id = newMenu->insertItem(QPixmap(newchart), tr("Index"), this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  id = newMenu->insertItem(QPixmap(newchart), tr("Spread"), this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  
  menu = new QPopupMenu();
  menu->insertItem(QPixmap(newchart), tr("&New..."), newMenu);
  menu->insertItem(QPixmap(edit), tr("&Edit Chart"), this, SLOT(editChart()), CTRL+Key_E);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Chart"), this, SLOT(deleteChart()), CTRL+Key_D);
  menu->insertItem(QPixmap(exportfile), tr("E&xport Chart"), this, SLOT(exportSymbol()), CTRL+Key_X);
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help"), this, SLOT(slotHelp()), CTRL+Key_H);

  chartNoSelection();
}

ChartPage::~ChartPage ()
{
  delete newMenu;
  delete menu;
}

void ChartPage::deleteChart ()
{
  SymbolDialog *dialog = new SymbolDialog(this, 
  					  nav->getCurrentPath(),
					  "*",
					  QFileDialog::ExistingFiles);
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

  ChartDb *db = new ChartDb;
  db->dbPrefDialog(symbol);
  delete db;
}

void ChartPage::exportSymbol ()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  					  config.getData(Config::DataPath),
					  "*",
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Charts"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QString s = config.getData(Config::Home);
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
  ChartDb *db = new ChartDb;
  db->openChart(path);

  QString s = config.getData(Config::Home);
  s.append("/export/");
  
  QString s2 = db->getData(tr("Symbol"));
  if (! s2.length())
  {
    QStringList l = QStringList::split("/", path, FALSE);
    s.append(l[l.count() - 1]);
  }
  else
    s.append(s2);

  db->dump(s, path);

  delete db;
}

void ChartPage::chartSelected (QString d)
{
  menu->setItemEnabled(menu->idAt(1), TRUE);
  emit fileSelected(d);
}


void ChartPage::chartNoSelection ()
{
  menu->setItemEnabled(menu->idAt(1), FALSE);
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

void ChartPage::newChart (int id)
{
  QString dbPlugin = newMenu->text(id);

  ChartDb *db = new ChartDb;
  db->setPlugin(dbPlugin);
  db->createNew(dbPlugin);
  delete db;
  refreshList();
}

void ChartPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithcharts.html");
  hw->show();
}

