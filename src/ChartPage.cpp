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

#include "ChartPage.h"
#include "Setting.h"
#include "SymbolDialog.h"
#include "DbPlugin.h"
#include "HelpWindow.h"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/export.xpm"
#include "../pics/newchart.xpm"
#include "../pics/help.xpm"
#include <qmessagebox.h>
#include <qcursor.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qaccel.h>


ChartPage::ChartPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  search = new QLineEdit(this);
  search->setText("*");
  connect(search, SIGNAL(textChanged(const QString &)), this, SLOT(searchChanged(const QString &)));
  QToolTip::add(search, tr("List Filter, e.g. s* or sb*"));
  vbox->addWidget(search);

  QString s;
  config.getData(Config::DataPath, s);
  nav = new Navigator(this, s);
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(chartSelected(QString)));
  connect(nav, SIGNAL(fileOpened(QString)), this, SLOT(chartOpened(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(chartNoSelection()));
  connect(nav, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(QListBoxItem *)));
  nav->updateList();
  vbox->addWidget(nav);

  newMenu = new QPopupMenu(this);
  int id = newMenu->insertItem(QPixmap(newchart), "&CC", this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  id = newMenu->insertItem(QPixmap(newchart), "&Index", this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  id = newMenu->insertItem(QPixmap(newchart), "&Spread", this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  
  menu = new QPopupMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New..."), newMenu);
  menu->insertItem(QPixmap(edit), tr("&Edit Chart	Ctrl+E"), this, SLOT(editChart()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Chart	Ctrl+D"), this, SLOT(deleteChart()));
  menu->insertItem(QPixmap(exportfile), tr("E&xport Chart CSV	Ctrl+X"), this, SLOT(exportSymbol()));
  menu->insertItem(QPixmap(exportfile), tr("D&ump Chart		Ctrl+U"), this, SLOT(dumpSymbol()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help	Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_D, DeleteChart);
  a->insertItem(CTRL+Key_E, EditChart);
  a->insertItem(CTRL+Key_X, ExportSymbol);
  a->insertItem(CTRL+Key_U, DumpSymbol);
  a->insertItem(CTRL+Key_H, Help);
  a->insertItem(CTRL+Key_Tab, Tab);
  
  chartNoSelection();
}

ChartPage::~ChartPage ()
{
}

void ChartPage::deleteChart ()
{
  QString s("*");
  QString s2, bp;
  nav->getCurrentPath(s2);
  config.getData(Config::DataPath, bp);
  SymbolDialog *dialog = new SymbolDialog(this, 
                                          bp,
  					  s2,
					  s,
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
  QString symbol;
  nav->getFileSelection(symbol);
  if (! symbol.length())
    return;

  DbPlugin db;
  db.openChart(symbol);    
  db.dbPrefDialog();
  
  db.close();
}

void ChartPage::exportSymbol ()
{
  QString s("*");
  QString s2, bp;
  nav->getCurrentPath(s2);
  config.getData(Config::DataPath, bp);
  SymbolDialog *dialog = new SymbolDialog(this,
                                          bp,
  					  s2,
					  s,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Charts"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QString s;
    config.getData(Config::Home, s);
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
      exportChart(l[loop], TRUE);
  }

  delete dialog;
}

void ChartPage::dumpSymbol ()
{
  QString s("*");
  QString s2, bp;
  nav->getCurrentPath(s2);
  config.getData(Config::DataPath, bp);
  SymbolDialog *dialog = new SymbolDialog(this,
                                          bp,
  					  s2,
					  s,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Charts"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config.getData(Config::Home, s);
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
      exportChart(l[loop], FALSE);
  }

  delete dialog;
}

void ChartPage::exportChart (QString &path, bool f)
{
  DbPlugin db;
  db.openChart(path);

  QString s;
  config.getData(Config::Home, s);
  s.append("/export/");
  
  QString s2;
  db.getHeaderField(DbPlugin::Symbol, s2);
  if (! s2.length())
  {
    QStringList l = QStringList::split("/", path, FALSE);
    s.append(l[l.count() - 1]);
  }
  else
    s.append(s2);

  db.dump(s, f);

  db.close();
}

void ChartPage::chartSelected (QString)
{
  menu->setItemEnabled(menu->idAt(1), TRUE);
}

void ChartPage::chartOpened (QString d)
{
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
  QString s(d);
  nav->setFilter(s);
}

void ChartPage::newChart (int id)
{
  QString type = newMenu->text(id);
  type.remove(type.find("&", 0, TRUE), 1);

  DbPlugin db;
  db.createNew(type);
  db.close();
  
  refreshList();
}

void ChartPage::slotHelp ()
{
  QString s = "workwithcharts.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}

void ChartPage::setFocus ()
{
  nav->setFocus();
}

void ChartPage::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (key->state() == Qt::ControlButton)
  {
    switch(key->key())
    {
      case Qt::Key_D:
        slotAccel(DeleteChart);
	break;
      case Qt::Key_E:
        slotAccel(EditChart);
	break;
      case Qt::Key_X:
        slotAccel(ExportSymbol);
	break;
      case Qt::Key_U:
        slotAccel(DumpSymbol);
	break;
      case Qt::Key_Tab:
        slotAccel(Tab);
	break;
      default:
        break;
    }
  }
  else
  {
    if (! search->hasFocus())
      nav->doKeyPress(key);
  }
}

void ChartPage::slotAccel (int id)
{
  switch (id)
  {
    case DeleteChart:
      deleteChart();
      break;  
    case EditChart:
      editChart();
      break;  
    case ExportSymbol:
      exportSymbol();
      break;  
    case DumpSymbol:
      dumpSymbol();
      break;  
    case Help:
      slotHelp();
      break;  
    case Tab:
      if (search->hasFocus())
        nav->setFocus();
      else
        search->setFocus();
      break;  
    default:
      break;
  }
}

