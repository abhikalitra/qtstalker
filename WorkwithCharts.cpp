/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "WorkwithCharts.h"
#include "ChartDb.h"
#include "EditDialog.h"
#include "CompositeDialog.h"
#include "GroupDialog.h"
#include "PortfolioDialog.h"
#include "Setting.h"
#include "dirclosed.xpm"
#include "plainitem.xpm"
#include "portfolio.xpm"
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qinputdialog.h>
#include <qtextstream.h>
#include <qfiledialog.h>
#include <qdatetime.h>

WorkwithCharts::WorkwithCharts (Config *c, QString ty) : QDialog (0, "WorkwithDialog", FALSE)
{
  config = c;
  type = ty;

  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setSpacing(5);
  hbox->setMargin(5);

  QVBoxLayout *vbox = new QVBoxLayout(hbox);
  vbox->setMargin(5);

  list = new QListView(this);
  list->setSelectionMode(QListView::Single);
  connect(list, SIGNAL(selectionChanged()), this, SLOT(buttonStatus()));
  connect(list, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(openSymbol(QListViewItem *)));
  vbox->addWidget(list);

  QGridLayout *grid = new QGridLayout(hbox, 8, 1);
  grid->setSpacing(2);
  grid->setMargin(5);

  openButton = new QPushButton(tr("Open"), this);
  grid->addWidget(openButton, 0, 0);
  connect (openButton, SIGNAL (clicked()), this, SLOT (openSymbol()));

  newButton = new QPushButton(tr("New"), this);
  grid->addWidget(newButton, 1, 0);
  connect (newButton, SIGNAL (clicked()), this, SLOT (newSymbol()));

  editButton = new QPushButton(tr("Edit"), this);
  grid->addWidget(editButton, 2, 0);
  connect (editButton, SIGNAL (clicked()), this, SLOT (editSymbol()));

  renameButton = new QPushButton(tr("Rename"), this);
  grid->addWidget(renameButton, 3, 0);
  connect(renameButton, SIGNAL(clicked()), this, SLOT(renameSymbol()));

  deleteButton = new QPushButton(tr("Delete"), this);
  grid->addWidget(deleteButton, 4, 0);
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteSymbol()));

  exportButton = new QPushButton(tr("Export"), this);
  grid->addWidget(exportButton, 5, 0);
  connect(exportButton, SIGNAL(clicked()), this, SLOT(exportSymbol()));
  exportButton->setEnabled(FALSE);

  QPushButton *button = new QPushButton(tr("Done"), this);
  grid->addWidget(button, 6, 0);
  connect (button, SIGNAL (clicked()), this, SLOT (reject()));

  if (! type.compare("Chart"))
  {
    setCaption(tr("Qtstalker: Chart List"));
    list->addColumn(tr("Symbol"), 150);
    list->addColumn(tr("Type"), 80);
    list->addColumn(tr("First Date"), 80);
    list->addColumn(tr("Last Date"), 80);
    exportButton->setEnabled(TRUE);
  }
  else
  {
    if (! type.compare("Group"))
    {
      setCaption(tr("Qtstalker: Group List"));
      list->addColumn(tr("Group"), 200);
    }
    else
    {
      setCaption(tr("Qtstalker: Portfolio List"));
      list->addColumn(tr("Portfolio"), 200);
    }
  }
}

WorkwithCharts::~WorkwithCharts ()
{
}

void WorkwithCharts::updateList ()
{
  list->clear();

  QDir dir;
  QPixmap pix;
  QStringList l;

  if (! type.compare("Chart"))
  {
    updateChartList();
    return;
  }
  else
  {
    if (! type.compare("Group"))
    {
      pix = dirclosed;
      l = config->getGroupList();
    }
    else
    {
      pix = portfolio;
      l = config->getPortfolioList();
    }
  }

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    item = new QListViewItem(list, l[loop]);
    item->setPixmap(0, pix);
  }

  buttonStatus();
}

void WorkwithCharts::updateChartList ()
{
  list->clear();
  
  QPixmap pix = plainitem;

  ChartDb *index = new ChartDb();
  index->setPath(config->getData(Config::IndexPath));
  index->openChart();
  QStringList l = index->getKeyList();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *details = new Setting;
    details->parse(index->getData(l[loop]));

    QDateTime dt = index->getDateTime(details->getData("First Date"));
    QDateTime dt2 = index->getDateTime(details->getData("Last Date"));

    item = new QListViewItem(list, l[loop], details->getData("Chart Type"), dt.toString("yyyyMMdd"), dt2.toString("yyyyMMdd"));

    item->setPixmap(0, pix);

    delete details;
  }

  delete index;
}

void WorkwithCharts::openSymbol (QListViewItem *)
{
  openSymbol();
}

void WorkwithCharts::openSymbol ()
{
  item = list->selectedItem();
  if (item)
  {
    if (! type.compare("Chart"))
    {
      QString s = config->getData(Config::DataPath);
      s.append("/");
      s.append(item->text(0));
      emit chartOpened (s);
    }
    else
    {
      if (! type.compare("Group"))
        emit groupOpened (item->text(0));
      else
        openPortfolio();
    }

    reject();
  }
}

void WorkwithCharts::deleteSymbol ()
{
  item = list->selectedItem();
  if (item)
  {
    if (! type.compare("Chart"))
      deleteChart();
    else
    {
      if (! type.compare("Group"))
        deleteGroup();
      else
        deletePortfolio();
    }
  }
}

void WorkwithCharts::editSymbol ()
{
  item = list->selectedItem();
  if (item)
  {
    if (! type.compare("Chart"))
      editChart();
    else
    {
      if (! type.compare("Group"))
        editGroup();
      else
        editPortfolio();
    }
  }
}

void WorkwithCharts::newSymbol ()
{
  if (! type.compare("Chart"))
    newComposite();
  else
  {
    if (! type.compare("Group"))
      newGroup();
    else
      newPortfolio();
  }
}

void WorkwithCharts::renameSymbol ()
{
  item = list->selectedItem();
  if (item)
  {
    if (! type.compare("Group"))
      renameGroup();
    else
    {
      if (! type.compare("Portfolio"))
        renamePortfolio();
    }
  }
}

void WorkwithCharts::buttonStatus ()
{
  item = list->selectedItem();
  if (item)
  {
    openButton->setEnabled(TRUE);
    deleteButton->setEnabled(TRUE);
    editButton->setEnabled(TRUE);

    if (! type.compare("Chart"))
      renameButton->setEnabled(FALSE);
    else
      renameButton->setEnabled(TRUE);
  }
  else
  {
    openButton->setEnabled(FALSE);
    deleteButton->setEnabled(FALSE);
    editButton->setEnabled(FALSE);
    renameButton->setEnabled(FALSE);
  }
}

void WorkwithCharts::deleteChart ()
{
  QString selection = config->getData(Config::DataPath);
  selection.append("/");
  selection.append(item->text(0));

  QDir dir(selection);
  dir.remove(selection, TRUE);

  delete item;

  buttonStatus();
}

void WorkwithCharts::editChart ()
{
  QString selection = config->getData(Config::DataPath);
  selection.append("/");
  selection.append(item->text(0));

  ChartDb *db = new ChartDb();
  db->setPath(selection);
  if (db->openChart())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Can't open chart."));
    delete db;
    return;
  }

  Setting *set = db->getDetails();

  QString chartType = set->getData("Chart Type");
  if (! chartType.compare(tr("Spread")) || ! chartType.compare(tr("Ratio")) || ! chartType.compare(tr("Index")))
  {
    delete set;
    delete db;
    editComposite();
    return;
  }

  EditDialog *dialog = new EditDialog(config);

  delete db;

  QString s = tr("Qtstalker: Edit ");
  s.append(chartType);
  dialog->setCaption(s);

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    ChartDb *db = new ChartDb();
    db->setPath(selection);
    db->openChart();
    db->setDetails(set);
    delete db;
  }

  delete set;
}

void WorkwithCharts::editComposite ()
{
  QString file = config->getData(Config::DataPath);
  file.append("/");
  file.append(item->text(0));

  CompositeDialog *dialog = new CompositeDialog(config);

  ChartDb *db = new ChartDb();
  db->setPath(file);
  db->openChart();

  Setting *set = db->getComposite();
  Setting *details = db->getDetails();

  QString s = tr("Qtstalker: Edit ");
  s.append(details->getData("Chart Type"));
  dialog->setCaption(s);

  delete details;
  delete db;

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    db = new ChartDb();
    db->setPath(file);
    db->openChart();
    db->setComposite(set);
    delete db;
  }

  delete set;
  delete dialog;
}

void WorkwithCharts::newComposite()
{
  bool ok;
  QString name = QInputDialog::getText(tr("New Composite"), tr("Enter new composite symbol."),
                                       QLineEdit::Normal, tr("New Composite"), &ok, this);
  if ((! ok) || (name.isNull()))
    return;

  if (name.contains("/"))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Symbol cannot contain / character."));
    return;
  }

  QString path = config->getData(Config::DataPath);
  path.append("/");
  path.append(name);
  QDir dir(path);
  if (dir.exists(path, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This chart already exists."));
    return;
  }

  QStringList l;
  l.append(tr("Index"));
  l.append(tr("Spread"));
  l.append(tr("Ratio"));

  QString type = QInputDialog::getItem(tr("New Composite Type"), tr("Choose composite type to create."),
 	                               l, 0, FALSE, &ok, this);
  if ((! ok) || (type.isNull()))
    return;

  CompositeDialog *dialog = new CompositeDialog(config);

  Setting *set = new Setting();

  if (type.compare(tr("Index")))
  {
    dialog->setInsertButton(FALSE);
    dialog->setDeleteButton(FALSE);
    set->set("1", "Symbol 1", Setting::Composite);
    set->set("2", "Symbol 1", Setting::Composite);
  }

  dialog->setItems(set);

  QString s = tr("Qtstalker: New ");
  s.append(type);
  dialog->setCaption(s);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    ChartDb *db = new ChartDb();
    db->setPath(path);
    db->openChart();

    Setting *details = new Setting();
    details->set(tr("Title"), name, Setting::Text);
    details->set("Symbol", name, Setting::None);
    details->set("Chart Type", type, Setting::None);
    db->setDetails(details);
    delete details;

    db->setComposite(set);

    delete db;

    updateList();
    buttonStatus();
  }

  delete set;
  delete dialog;
}

void WorkwithCharts::newGroup()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Group"), tr("Enter new group symbol."),
  					    QLineEdit::Normal, tr("New Group"), &ok, this);
  if ((! ok) || (selection.isNull()))
    return;

  Setting *group = config->getGroup(selection);
  int t = group->count();
  delete group;
  if (t != 0)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This group already exists."));
    return;
  }

  GroupDialog *dialog = new GroupDialog(config);

  Setting *set = new Setting;
  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setGroup(selection, set);
    updateList();
  }

  delete set;
  delete dialog;
}

void WorkwithCharts::editGroup()
{
  GroupDialog *dialog = new GroupDialog(config);

  Setting *group = config->getGroup(item->text(0));
  dialog->setItems(group);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setGroup(item->text(0), group);
  }

  delete group;
  delete dialog;
}

void WorkwithCharts::deleteGroup()
{
  config->deleteGroup(item->text(0));
  delete item;
  buttonStatus();
}

void WorkwithCharts::renameGroup ()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Group"), tr("Enter new group symbol."),
  					    QLineEdit::Normal, item->text(0), &ok, this);
  if ((ok) && (! selection.isNull()))
  {
    Setting *group = config->getGroup(selection);
    int t = group->count();
    delete group;
    if (t != 0)
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This chart group exists."));
      return;
    }

    group = config->getGroup(item->text(0));
    config->deleteGroup(item->text(0));
    config->setGroup(selection, group);
    delete group;

    updateList();
    buttonStatus();
  }
}

void WorkwithCharts::renamePortfolio ()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("Rename Portfolio"), tr("Enter new portfolio name."),
  					    QLineEdit::Normal, item->text(0), &ok, this);
  if ((ok) && (! selection.isNull()))
  {
    QStringList l = config->getPortfolio(selection);
    if (l.count())
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This portfolio already exists."));
      return;
    }

    l = config->getPortfolio(item->text(0));
    config->deletePortfolio(item->text(0));
    config->setPortfolio(selection, l);

    updateList();
    buttonStatus();
  }
}

void WorkwithCharts::openPortfolio ()
{
  PortfolioDialog *dialog = new PortfolioDialog(config, item->text(0));
  dialog->show();
  reject();
}

void WorkwithCharts::editPortfolio ()
{
  PortfolioDialog *dialog = new PortfolioDialog(config, item->text(0));
  dialog->show();
  reject();
}

void WorkwithCharts::deletePortfolio ()
{
  config->deletePortfolio(item->text(0));
  delete item;
  buttonStatus();
}

void WorkwithCharts::newPortfolio ()
{
  bool ok;
  QString selection = QInputDialog::getText(tr("New Portfolio"), tr("Enter new portfolio name."),
  					    QLineEdit::Normal, tr("New Portfolio"), &ok, this);
  if ((ok) && (! selection.isNull()))
  {
    QStringList l = config->getPortfolio(selection);

    if (l.count())
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("This portfolio already exists."));
      return;
    }

    PortfolioDialog *dialog = new PortfolioDialog(config, selection);
    dialog->show();
    reject();
  }
}

void WorkwithCharts::exportSymbol ()
{
  QStringList files = QFileDialog::getOpenFileNames("*", config->getData(Config::DataPath), this, "file dialog",
  						    tr("Select Charts For Export"));
  if (files.count() == 0)
    return;

  QString path = config->getData(Config::Home);
  path.append("/export");
  QDir dir(path);
  if (! dir.exists(path, TRUE))
  {
    if (! dir.mkdir(path, TRUE))
    {
      qDebug("Unable to create export directory.");
      return;
    }
  }

  int loop;
  for (loop = 0; loop < (int) files.count(); loop++)
  {
    ChartDb *db = new ChartDb();
    db->setPath(files[loop]);
    if(db->openChart())
    {
      delete db;
      continue;
    }
    
    Setting *set = db->getDetails();
    QString s = path;
    s.append("/");
    s.append(set->getData("Symbol"));
    delete set;

    QFile outFile(s);
    if (! outFile.open(IO_WriteOnly))
    {
      QString mess = tr("Unable to create");
      mess.append(" ");
      mess.append(s);
      QMessageBox::information(this, tr("Qtstalker: Error"), mess, 0);
      return;
    }
    QTextStream outStream(&outFile);

    QStringList key = db->getKeyList();

    int loop2;
    for (loop2 = 0; loop2 < (int) key.count(); loop2++)
      outStream << key[loop2] << "=" << db->getData(key[loop2]) << "\n";

    delete db;

    outFile.close();
  }
}



