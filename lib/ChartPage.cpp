/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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
#include "DataBase.h"
#include "../pics/addgroup.xpm"
#include "../pics/scanner.xpm"
#include "../pics/edit.xpm"
#include "../pics/sql.xpm"
#include "../pics/search.xpm"
#include <QMessageBox>
#include <QCursor>
#include <QToolTip>
#include <QLayout>
#include <QMenu>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QtDebug>
#include <QInputDialog>
#include <QGridLayout>


ChartPage::ChartPage (QWidget *w) : QWidget (w)
{
  activeSearch = 0;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(5);
  hbox->setSpacing(2);
  vbox->addLayout(hbox);

//  rules = new QComboBox;
//  hbox->addWidget(rules);

//  hbox = new QHBoxLayout;
//  hbox->setMargin(5);
//  hbox->setSpacing(2);
//  vbox->addLayout(hbox);

  allButton = new QToolButton;
  allButton->setToolTip(tr("All Symbols"));
  allButton->setText("*");
  connect(allButton, SIGNAL(clicked()), this, SLOT(updateList()));
  hbox->addWidget(allButton);
 
  symbolButton = new QToolButton;
  symbolButton->setToolTip(tr("Symbol Search"));
  symbolButton->setIcon(QIcon(search));
  connect(symbolButton, SIGNAL(clicked()), this, SLOT(symbolSearch()));
  hbox->addWidget(symbolButton);

/* 
  sqlButton = new QToolButton;
  sqlButton->setToolTip(tr("SQL Search"));
  sqlButton->setIcon(QIcon(sql_xpm));
  connect(sqlButton, SIGNAL(clicked()), this, SLOT(sqlSearch()));
  hbox->addWidget(sqlButton);
  sqlButton->setEnabled(FALSE);
*/

  hbox->addStretch(1);

/*
  searchGoButton = new QToolButton;
  searchGoButton->setToolTip(tr("Perform Rule Search"));
  searchGoButton->setIcon(QIcon(scanner));
  connect(searchGoButton, SIGNAL(clicked()), this, SLOT(ruleSearch()));
  hbox->addWidget(searchGoButton);
  
  searchEditButton = new QToolButton;
  searchEditButton->setToolTip(tr("Edit Search Rules"));
  searchEditButton->setIcon(QIcon(edit));
  connect(searchEditButton, SIGNAL(clicked()), this, SLOT(editSearch()));
  hbox->addWidget(searchEditButton);

//  hbox->addStretch(1);

*/
 
  nav = new QListWidget;
  nav->setContextMenuPolicy(Qt::CustomContextMenu);
  nav->setSelectionMode(QAbstractItemView::ExtendedSelection);
  nav->setSortingEnabled(TRUE);
//  connect(nav, SIGNAL(fileOpened(QString)), this, SLOT(chartOpened(QString)));
  connect(nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(nav, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(chartOpened(QListWidgetItem *)));
  vbox->addWidget(nav);

  menu = new QMenu(this);
  menu->addAction(QIcon(addgroup), tr("Add To &Group	Ctrl+G"), this, SLOT(addToGroup()), QKeySequence(Qt::CTRL+Qt::Key_G));
  menu->addAction(QIcon(), tr("&All Symbols"), this, SLOT(updateList()), QKeySequence(Qt::CTRL+Qt::Key_A));
  menu->addAction(QIcon(search), tr("&Symbol Search"), this, SLOT(symbolSearch()), QKeySequence(Qt::CTRL+Qt::Key_S));

  refreshList();
}

ChartPage::~ChartPage ()
{
}

void ChartPage::chartOpened (QListWidgetItem *item)
{
  if (! item)
    return;

  emit fileSelected(item->text());
  emit addRecentChart(item->text());
}

void ChartPage::rightClick (const QPoint &)
{
  menu->exec(QCursor::pos());
}

void ChartPage::refreshList ()
{
  switch (activeSearch)
  {
    case 1:
      symbolSearch();
      break;
    case 2:
      sqlSearch();
      break;
    default:
      updateList();
      break;
  }
}

void ChartPage::addToGroup ()
{
  QList<QListWidgetItem *> sl = nav->selectedItems();
  if (! sl.count())
    return;

  DataBase db;
  QStringList tl;
  db.getAllGroupsList(tl);
  bool ok;
  QString group = QInputDialog::getItem(this,
					QString(tr("Add To Group")),
					QString(tr("Select group to add selected charts")),
                                        tl,
					0,
					FALSE,
                                        &ok,
					0);
  if (! group.length())
    return;

  db.getGroupList(group, tl);

  int loop;
  for (loop = 0; loop < sl.count(); loop++)
    tl.append(sl.value(loop)->text());

  tl.removeDuplicates();

  db.setGroupList(group, tl, FALSE);
}

void ChartPage::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (key->modifiers() == Qt::ControlModifier)
  {
    switch(key->key())
    {
      case Qt::Key_G:
        addToGroup();
	break;
      default:
        break;
    }
  }
  else
  {
    switch(key->key())
    {
      default:
//        nav->doKeyPress(key);
        break;
    }
  }
}

void ChartPage::updateList ()
{
  activeSearch = 0;
  searchString.clear();
  nav->clear();
  DataBase db;
  QStringList l;
  db.getAllChartsList(l);
  nav->addItems(l);
}

void ChartPage::symbolSearch ()
{
  bool ok;
  QString s = QInputDialog::getText(this,
				    tr("Symbol Search"),
				    tr("Symbol contains:"),
				    QLineEdit::Normal,
				    QString(),
				    &ok,
				    0);
  if (! s.length())
    return;

  activeSearch = 1;
  searchString = s;

  DataBase db;
  QStringList l;
  db.getAllChartsList(l);

  nav->clear();

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    if (l[loop].contains(searchString))
      nav->addItem(l[loop]);
  }
}

void ChartPage::sqlSearch ()
{
  activeSearch = 3;
}

void ChartPage::ruleSearch ()
{
  QString s = rules->currentText();
  if (! s.length())
    return;

}

void ChartPage::editSearch ()
{
}


