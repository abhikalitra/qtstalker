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

#include "WorkwithChartsDialog.h"
#include "ChartDb.h"
#include "EditDialog.h"
#include "CompositeDialog.h"
#include "Setting.h"
#include "open.xpm"
#include "newchart.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "export.xpm"
#include "done.xpm"
#include <qinputdialog.h>
#include <qtextstream.h>
#include <qdatetime.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qdir.h>
#include <qmessagebox.h>

WorkwithChartsDialog::WorkwithChartsDialog (Config *c) : QDialog (0, 0, FALSE)
{
  config = c;

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  QGridLayout *grid = new QGridLayout(vbox, 1, 8);
  grid->setSpacing(0);

  openButton = new QToolButton(this);
  QToolTip::add(openButton, tr("Open Chart"));
  openButton->setPixmap(QPixmap(open));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openSymbol()));
  openButton->setMaximumWidth(30);
  grid->addWidget(openButton, 0, 0);

  newButton = new QToolButton(this);
  QToolTip::add(newButton, tr("New Composite"));
  newButton->setPixmap(QPixmap(newchart));
  connect(newButton, SIGNAL(clicked()), this, SLOT(newComposite()));
  newButton->setMaximumWidth(30);
  grid->addWidget(newButton, 0, 1);

  editButton = new QToolButton(this);
  QToolTip::add(editButton, tr("Edit Chart"));
  editButton->setPixmap(QPixmap(edit));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editChart()));
  editButton->setMaximumWidth(30);
  grid->addWidget(editButton, 0, 2);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete Chart"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteChart()));
  deleteButton->setMaximumWidth(30);
  grid->addWidget(deleteButton, 0, 3);

  exportButton = new QToolButton(this);
  QToolTip::add(exportButton, tr("Export Chart"));
  exportButton->setPixmap(QPixmap(exportfile));
  connect(exportButton, SIGNAL(clicked()), this, SLOT(exportSymbol()));
  exportButton->setMaximumWidth(30);
  grid->addWidget(exportButton, 0, 4);

  exportAllButton = new QToolButton(this);
  QToolTip::add(exportAllButton, tr("Export All Charts"));
  exportAllButton->setPixmap(QPixmap(exportfile));
  connect(exportAllButton, SIGNAL(clicked()), this, SLOT(exportAll()));
  exportAllButton->setMaximumWidth(30);
  grid->addWidget(exportAllButton, 0, 5);
  
  QToolButton *button = new QToolButton(this);
  QToolTip::add(button, tr("Done"));
  button->setPixmap(QPixmap(finished));
  connect(button, SIGNAL(clicked()), this, SLOT(reject()));
  button->setMaximumWidth(30);
  grid->addWidget(button, 0, 6);

  QHBoxLayout *hbox = new QHBoxLayout(vbox);

  list = new QListView(this);
  list->addColumn(0, 200);
  list->setSelectionMode(QListView::Single);
  connect(list, SIGNAL(selectionChanged()), this, SLOT(buttonStatus()));
  connect(list, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(openSymbol()));
  hbox->addWidget(list);

  QGroupBox *gbox = new QGroupBox(this);
  gbox->setColumnLayout(2, Horizontal);
  hbox->addWidget(gbox);

  QLabel *label = new QLabel(tr("Symbol"), gbox);
  symbol = new QLabel(0, gbox);

  label = new QLabel(tr("Title"), gbox);
  title = new QLabel(0, gbox);

  label = new QLabel(tr("Type"), gbox);
  type = new QLabel(0, gbox);

  label = new QLabel(tr("First Date"), gbox);
  firstDate = new QLabel(0, gbox);

  label = new QLabel(tr("Last Date"), gbox);
  lastDate = new QLabel(0, gbox);

  buttonStatus();
}

WorkwithChartsDialog::~WorkwithChartsDialog ()
{
}

void WorkwithChartsDialog::updateList ()
{
  list->clear();

  QDir dir(config->getData(Config::DataPath));
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    item = new QListViewItem(list, dir[loop]);

}

void WorkwithChartsDialog::openSymbol ()
{
  item = list->selectedItem();
  if (item)
  {
    QString s = config->getData(Config::DataPath);
    s.append("/");
    s.append(item->text(0));
    emit chartOpened (s);
    reject();
  }
}

void WorkwithChartsDialog::deleteChart ()
{
  QString selection = config->getData(Config::DataPath);
  selection.append("/");
  selection.append(item->text(0));

  QDir dir(selection);
  dir.remove(selection, TRUE);
  
  delete item;

  buttonStatus();
}

void WorkwithChartsDialog::editChart ()
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

void WorkwithChartsDialog::editComposite ()
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

void WorkwithChartsDialog::newComposite()
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

void WorkwithChartsDialog::exportSymbol ()
{
  item = list->selectedItem();
  if (! item)
    return;

  QString s = config->getData(Config::Home);
  s.append("/export");
  QDir dir(s);
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
    {
      qDebug("Unable to create export directory.");
      return;
    }
  }

  exportChart(item->text(0));
}

void WorkwithChartsDialog::exportAll ()
{
  QString s = config->getData(Config::Home);
  s.append("/export");
  QDir dir(s);
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
    {
      qDebug("Unable to create export directory.");
      return;
    }
  }

  dir.setPath(config->getData(Config::DataPath));
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    exportChart(dir[loop]);
}

void WorkwithChartsDialog::exportChart (QString symbol)
{
  QString s = config->getData(Config::DataPath);
  s.append("/");
  s.append(symbol);

  ChartDb *db = new ChartDb();
  db->setPath(s);
  if(db->openChart())
  {
    delete db;
    return;
  }

  s = config->getData(Config::Home);
  s.append("/export/");
  s.append(symbol);

  QFile outFile(s);
  if (! outFile.open(IO_WriteOnly))
  {
    QString mess = tr("Unable to create");
    mess.append(" ");
    mess.append(s);
    qDebug(mess.latin1());
    delete db;
    return;
  }
  QTextStream outStream(&outFile);

  db->openCursor();

  while (1)
  {
    if (! db->getCursor())
      outStream << db->getCursorKey() << "=" << db->getCursorData() << "\n";
    else
      break;
  }

  db->closeCursor();
  delete db;

  outFile.close();
}

void WorkwithChartsDialog::buttonStatus ()
{
  item = list->selectedItem();
  if (item)
  {
    QString s = config->getData(Config::DataPath);
    s.append("/");
    s.append(item->text(0));

    ChartDb *db = new ChartDb;
    db->setPath(s);
    db->openChart();
    Setting *set = db->getDetails();

    symbol->setText(set->getData("Symbol"));

    title->setText(set->getData("Title"));

    type->setText(set->getData("Chart Type"));

    s = set->getData("First Date");
    s.truncate(s.length() - 6);
    firstDate->setText(s);

    s = set->getData("Last Date");
    s.truncate(s.length() - 6);
    lastDate->setText(s);

    delete set;

    openButton->setEnabled(TRUE);
    deleteButton->setEnabled(TRUE);
    editButton->setEnabled(TRUE);
    exportButton->setEnabled(TRUE);
  }
  else
  {
    symbol->setText("");
    title->setText("");
    type->setText("");
    firstDate->setText("");
    lastDate->setText("");

    openButton->setEnabled(FALSE);
    deleteButton->setEnabled(FALSE);
    editButton->setEnabled(FALSE);
    exportButton->setEnabled(FALSE);
  }
}



