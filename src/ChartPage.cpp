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
  keyFlag = FALSE;
  macroFlag = FALSE;
  macro = 0;
  
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  search = new MyLineEdit(this, Macro::ChartPage);
  search->setText("*");
  connect(search, SIGNAL(textChanged(const QString &)), this, SLOT(searchChanged(const QString &)));
  connect(search, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));
  QToolTip::add(search, tr("List Filter, e.g. s* or sb*"));
  vbox->addWidget(search);

  nav = new Navigator(this, config.getData(Config::DataPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(chartSelected(QString)));
  connect(nav, SIGNAL(fileOpened(QString)), this, SLOT(chartOpened(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(chartNoSelection()));
  connect(nav, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(QListBoxItem *)));
  connect(nav, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));
  nav->updateList();
  nav->setId(Macro::ChartPage);
  vbox->addWidget(nav);

  newMenu = new QPopupMenu(this);
  int id = newMenu->insertItem(QPixmap(newchart), tr("&CC"), this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  id = newMenu->insertItem(QPixmap(newchart), tr("&Index"), this, SLOT(newChart(int)));
  newMenu->setItemParameter(id, id);
  id = newMenu->insertItem(QPixmap(newchart), tr("&Spread"), this, SLOT(newChart(int)));
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

  QString plugin = config.parseDbPlugin(symbol);
  DbPlugin *db = config.getDbPlugin(plugin);
  if (! db)
  {
    config.closePlugin(plugin);
    return;
  }

  db->openChart(symbol);    
  db->dbPrefDialog();
  
  config.closePlugin(plugin);
}

void ChartPage::exportSymbol ()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  					  nav->getCurrentPath(),
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
      exportChart(l[loop], TRUE);
  }

  delete dialog;
}

void ChartPage::dumpSymbol ()
{
  SymbolDialog *dialog = new SymbolDialog(this,
  					  nav->getCurrentPath(),
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
      exportChart(l[loop], FALSE);
  }

  delete dialog;
}

void ChartPage::exportChart (QString path, bool f)
{
  QString plugin = config.parseDbPlugin(path);
  DbPlugin *db = config.getDbPlugin(plugin);
  if (! db)
  {
    config.closePlugin(plugin);
    return;
  }

  db->openChart(path);

  QString s = config.getData(Config::Home);
  s.append("/export/");
  
  QString s2 = db->getHeaderField(DbPlugin::Symbol);
  if (! s2.length())
  {
    QStringList l = QStringList::split("/", path, FALSE);
    s.append(l[l.count() - 1]);
  }
  else
    s.append(s2);

  db->dump(s, f);

  config.closePlugin(plugin);
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
  nav->setFilter(d);
}

void ChartPage::newChart (int id)
{
  QString dbPlugin = newMenu->text(id);
  dbPlugin.remove(dbPlugin.find("&", 0, TRUE), 1);

  DbPlugin *db = config.getDbPlugin(dbPlugin);
  if (! db)
  {
    config.closePlugin(dbPlugin);
    return;
  }
  
  db->createNew();
  
  config.closePlugin(dbPlugin);
  
  refreshList();
}

void ChartPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithcharts.html");
  hw->show();
}

void ChartPage::setFocus ()
{
  nav->setFocus();
}

void ChartPage::setKeyFlag (bool d)
{
  keyFlag = d;
  nav->setKeyFlag(d);
  search->setKeyFlag(d);
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
    if (search->hasFocus())
      search->doKeyPress(key);
    else
      nav->doKeyPress(key);
  }
}

void ChartPage::slotAccel (int id)
{
  switch (id)
  {
    case DeleteChart:
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartPage, ControlButton, Key_D, 0, QString());
      deleteChart();
      break;  
    case EditChart:
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartPage, ControlButton, Key_E, 0, QString());
      editChart();
      break;  
    case ExportSymbol:
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartPage, ControlButton, Key_X, 0, QString());
      exportSymbol();
      break;  
    case DumpSymbol:
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartPage, ControlButton, Key_U, 0, QString());
      dumpSymbol();
      break;  
    case Help:
      slotHelp();
      break;  
    case Tab:
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartPage, ControlButton, Key_Tab, 0, QString());
      if (search->hasFocus())
        nav->setFocus();
      else
        search->setFocus();
      break;  
    default:
      break;
  }
}

void ChartPage::runMacro (Macro *d)
{
  macro = d;
  macroFlag = TRUE;
  
  while (macro->getZone(macro->getIndex()) == Macro::ChartPage)
  {
    doKeyPress(macro->getKey(macro->getIndex()));
    
    macro->incIndex();
    if (macro->getIndex() >= macro->getCount())
      break;
  }
  
  macroFlag = FALSE;
}

