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
#include "Indicator.h"
#include "Config.h"
#include "IndicatorBase.h"
#include "IndicatorFactory.h"
#include "PrefDialog.h"

#include "../pics/ok.xpm"
#include "../pics/disable.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/newchart.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"

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

  listFlag = 0;
  showActive();
}

void IndicatorPage::newIndicator ()
{
  IndicatorBase ib;
  QStringList l;
  ib.getIndicatorList(l, 1);

  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(tr("New Indicator"));
  QString s = tr("Main");
  dialog->addPage(0, s);

  s = tr("Name");
  QString name;
  dialog->addTextItem(0, s, name);

  s = tr("Indicator");
  QString indicator;
  dialog->addComboItem(0, s, l, 0);

  s = tr("Tab Row");
  dialog->addIntItem(0, s, 2, 1, 3);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  s = tr("Name");
  dialog->getItem(s, name);
  if (name.isEmpty())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Name missing."));
    delete dialog;
    return;
  }

  s = tr("Indicator");
  dialog->getItem(s, indicator);

  s = tr("Tab Row");
  int tabRow = dialog->getInt(s);

  delete dialog;

  // check is name already exists
  l.clear();
  DataBase db;
  db.getIndicatorList(l);
  if (l.contains(name))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This indicator already exists."));
    return;
  }

  Indicator i;
  i.setEnable(1);
  i.setName(name);
  i.setTabRow(tabRow);

  IndicatorFactory fac;
  IndicatorBase *ib2 = fac.getFunction(indicator);
  if (! ib2)
    return;

  rc = ib2->dialog();
  if (rc == QDialog::Rejected)
  {
    delete ib2;
    return;
  }

  ib2->getSettings(i);

  db.setIndicator(i);

  delete ib2;

  emit signalNewIndicator(name);

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

void IndicatorPage::editIndicator (QString &name)
{
  Indicator i;
  i.setName(name);

  DataBase db;
  db.getIndicator(i);
  QString indicator;
  i.getIndicator(indicator);

  IndicatorFactory fac;
  IndicatorBase *ib2 = fac.getFunction(indicator);
  if (! ib2)
    return;

  ib2->setSettings(i);

  int rc = ib2->dialog();
  if (rc == QDialog::Rejected)
  {
    delete ib2;
    return;
  }

  ib2->getSettings(i);

  db.setIndicator(i);

  delete ib2;

  emit signalDisableIndicator(name);
  emit signalEnableIndicator(name);
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
    db.setIndicatorEnable(i);
    item->setIcon(QIcon(disable));
    emit signalDisableIndicator(s);
  }
  else
  {
    i.setEnable(1);
    db.setIndicatorEnable(i);
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
  // menu items 1,2 only
  for (loop = 1; loop < 3; loop++)
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
        deleteIndicator();
	break;
      case Qt::Key_E:
        editIndicator();
	break;
      default:
        break;
    }
  }
}

void IndicatorPage::indicatorSearch ()
{
  bool aok;
  QString s = QInputDialog::getText(this,
                                    tr("Indicator Search"),
                                    tr("Enter either a specific name like RSI or\na partial match like %SI or %RSI%"),
                                    QLineEdit::Normal,
                                    QString(),
                                    &aok,
                                    0);
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


