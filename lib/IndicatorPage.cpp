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

#include "IndicatorPage.h"
#include "DataBase.h"
#include "IndicatorDialog.h"
#include "Indicator.h"

#include "../pics/ok.xpm"
#include "../pics/disable.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/newchart.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"
#include "../pics/move.xpm"

#include <QCursor>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolTip>
#include <QPixmap>
#include <QVBoxLayout>
#include <QIcon>
#include <QtDebug>
#include <QLabel>




IndicatorPage::IndicatorPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(5);
  hbox->setSpacing(2);
  vbox->addLayout(hbox);

  activeButton = new QToolButton;
  activeButton->setToolTip(tr("Show active indicators"));
  activeButton->setIcon(QIcon(ok));
  connect(activeButton, SIGNAL(clicked()), this, SLOT(showActive()));
  activeButton->setMaximumSize(25, 25);
  hbox->addWidget(activeButton);

  allButton = new QToolButton;
  allButton->setToolTip(tr("Show all indicators"));
  allButton->setIcon(QIcon(asterisk_xpm));
  connect(allButton, SIGNAL(clicked()), this, SLOT(showAll()));
  allButton->setMaximumSize(25, 25);
  hbox->addWidget(allButton);

  searchButton = new QToolButton;
  searchButton->setToolTip(tr("Search"));
  searchButton->setIcon(QIcon(search));
  connect(searchButton, SIGNAL(clicked()), this, SLOT(indicatorSearch()));
  searchButton->setMaximumSize(25, 25);
  hbox->addWidget(searchButton);

  hbox->addStretch(1);

  list = new QListWidget;
  list->setContextMenuPolicy(Qt::CustomContextMenu);
  list->setSortingEnabled(TRUE);
  connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(doubleClick(QListWidgetItem *)));
  connect(list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  vbox->addWidget(list);
    
  menu = new QMenu(this);
  QAction *action = menu->addAction(QIcon(newchart), tr("&New Indicator"), this, SLOT(newIndicator()), QKeySequence(Qt::CTRL+Qt::Key_N));
  actions.append(action);
  action = menu->addAction(QIcon(edit), tr("&Edit Indicator"), this, SLOT(editIndicator()), QKeySequence(Qt::CTRL+Qt::Key_E));
  actions.append(action);
  action = menu->addAction(QIcon(deleteitem), tr("&Delete Indicator"), this, SLOT(deleteIndicator()), QKeySequence(Qt::CTRL+Qt::Key_D));
  actions.append(action);
  action = menu->addAction(QIcon(moveitem), tr("&Move Indicator Tab"), this, SLOT(moveIndicator()), QKeySequence(Qt::CTRL+Qt::Key_M));
  actions.append(action);

  listFlag = 0;
  showActive();
}

void IndicatorPage::newIndicator ()
{
  bool ok;
  QString selection = QInputDialog::getText(this,
				    	    tr("New Indicator"),
  				    	    tr("Enter new indicator name."),
				    	    QLineEdit::Normal,
				    	    tr("New Indicator"),
				    	    &ok);
  if (! ok)
    return;

  DataBase db;
  QStringList l;
  db.getIndicatorList(l);
  if (l.contains(selection))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This indicator already exists."));
    return;
  }

  int tabRow = QInputDialog::getInt (this, tr("Indicator Tab"), tr("Row"), 1, 1, 3, 1, &ok, 0);
  if (! ok)
    return;

  IndicatorDialog *dialog = new IndicatorDialog(this, selection);
  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  Indicator i;
  i.setName(selection);
  db.getIndicator(i);

  i.setEnable(1);
  i.setTabRow(tabRow);

  db.setIndicator(i);

  delete dialog;

  emit signalNewIndicator(selection);

  if (listFlag == 0)
    showActive();

  if (listFlag == 1)
    showAll();
}

void IndicatorPage::editIndicator ()
{
  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  QString s = item->text();
  editIndicator(s);
}

void IndicatorPage::editIndicator (QString &s)
{
  IndicatorDialog *dialog = new IndicatorDialog(this, s);
  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  delete dialog;

  emit signalEditIndicator(s);
}

void IndicatorPage::deleteIndicator ()
{
  int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        tr("Are you sure you want to permanently delete this indicator?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
  if (rc == QMessageBox::No)
    return;

  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  QString s = item->text();

  DataBase db;
  db.deleteIndicator(s);

  emit signalDeleteIndicator(s);
  
  delete item;
}

void IndicatorPage::doubleClick (QListWidgetItem *item)
{
  if (! item)
    return;

  QString s = item->text();

  DataBase db;
  Indicator i;
  i.setName(s);
  db.getIndicator(i);

  if (i.getEnable())
  {
    i.setEnable(0);
    db.setIndicator(i);
    item->setIcon(QIcon(disable));
    emit signalDisableIndicator(s);
  }
  else
  {
    i.setEnable(1);
    db.setIndicator(i);
    item->setIcon(QIcon(ok));
    emit signalEnableIndicator(s);
  }
}

void IndicatorPage::rightClick (const QPoint &)
{
  bool b = FALSE;
  if (list->currentItem())
    b = TRUE;

  int loop;
  // menu items 1,2 3 only
  for (loop = 1; loop < 4; loop++)
    actions.at(loop)->setEnabled(b);

  menu->exec(QCursor::pos());
}

void IndicatorPage::keyPressEvent (QKeyEvent *key)
{
  doKeyPress(key);  
}

void IndicatorPage::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (key->modifiers() == Qt::ControlModifier)
  {
    switch(key->key())
    {
      case Qt::Key_W:
        newIndicator();
	break;
      case Qt::Key_D:
//        deleteIndicator();
	break;
      case Qt::Key_E:
//        editIndicator();
	break;
      default:
        break;
    }
  }
}

void IndicatorPage::indicatorSearch ()
{
  bool aok;
  QString s = QInputDialog::getText(this, tr("Indicator Search"), tr("Search pattern:"), QLineEdit::Normal, QString(), &aok, 0);
  if (! aok)
    return;

  listFlag = 2; // set to search mode

  DataBase db;
  QStringList l, al;
  db.getSearchIndicatorList(s, l);
  db.getActiveIndicatorList(al);

  list->clear();

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    if (al.contains(l[loop]))
      new QListWidgetItem(QIcon(ok), l[loop], list, 0);
    else
      new QListWidgetItem(QIcon(disable), l[loop], list, 0);
  }
}

void IndicatorPage::showActive ()
{
  listFlag = 0;

  DataBase db;
  QStringList l;
  db.getActiveIndicatorList(l);

  list->clear();

  int loop;
  for (loop = 0; loop < l.count(); loop++)
    new QListWidgetItem(QIcon(ok), l[loop], list, 0);
}

void IndicatorPage::showAll ()
{
  listFlag = 1;

  list->clear();

  DataBase db;
  QStringList l, al;
  db.getIndicatorList(l);
  db.getActiveIndicatorList(al);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    if (al.contains(l[loop]))
      new QListWidgetItem(QIcon(ok), l[loop], list, 0);
    else
      new QListWidgetItem(QIcon(disable), l[loop], list, 0);
  }
}

void IndicatorPage::moveIndicator ()
{
  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  DataBase db;
  Indicator i;
  QString s = item->text();
  i.setName(s);
  db.getIndicator(i);
  int cr = i.getTabRow();

  bool aok;
  int row = QInputDialog::getInt(this, tr("Move Indicator"), tr("Tab Row"), cr, 1, 3, 1, &aok, 0);
  if (! aok)
    return;

  i.setTabRow(row);
  db.setIndicator(i);

  emit signalDisableIndicator(s);
  emit signalEnableIndicator(s);
}

