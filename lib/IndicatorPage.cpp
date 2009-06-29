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
#include "FormulaEdit.h"
#include "IndicatorIndex.h"
#include "../pics/ok.xpm"
#include "../pics/disable.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/newchart.xpm"
#include <QCursor>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolTip>
#include <QPixmap>
#include <QVBoxLayout>
#include <QIcon>
#include <QtDebug>


IndicatorPage::IndicatorPage (QWidget *w) : QWidget (w)
{
  updateEnableFlag = FALSE;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(5);
  setLayout(vbox);
  
  list = new QListWidget;
  list->setContextMenuPolicy(Qt::CustomContextMenu);
  list->setSortingEnabled(TRUE);
  connect(list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(doubleClick(QListWidgetItem *)));
  connect(list, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(list, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelected()));
  vbox->addWidget(list);
    
  menu = new QMenu(this);
  QAction *action = menu->addAction(QIcon(newchart), tr("Ne&w Indicator		Ctrl+W"), this, SLOT(newIndicator()), QKeySequence(Qt::CTRL+Qt::Key_W));
  actionList.append(action);
  action = menu->addAction(QIcon(edit), tr("&Edit Indicator			Ctrl+E"), this, SLOT(editIndicator()), QKeySequence(Qt::CTRL+Qt::Key_E));
  actionList.append(action);
  action = menu->addAction(QIcon(deleteitem), tr("&Delete Indicator		Ctrl+D"), this, SLOT(deleteIndicator()), QKeySequence(Qt::CTRL+Qt::Key_D));
  actionList.append(action);

  updateList();

  itemSelected();
}

IndicatorPage::~IndicatorPage ()
{
}

void IndicatorPage::itemSelected ()
{
  bool b = FALSE;
  if (list->currentItem())
    b = TRUE;

  int loop;
  // menu items 1,2 only
  for (loop = 1; loop < 3; loop++)
    actionList.at(loop)->setEnabled(b);
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

  int tabRow = QInputDialog::getInt (this,
				     tr("Indicator Tab"),
				     tr("Row"),
				     1,
				     1,
				     3,
				     1,
				     &ok,
				     0);
  if (! ok)
    return;

  FormulaEdit *dialog = new FormulaEdit(this, selection);
  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  IndicatorIndex index;
  index.setEnable(1);
  index.setTabRow(tabRow);
  db.setIndicatorIndex(selection, index);

  delete dialog;

  emit signalNewIndicator(selection);
  
  updateList();
}

void IndicatorPage::editIndicator ()
{
  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  QString s = item->text();
  editIndicator(s);
}

void IndicatorPage::editIndicator (QString d)
{
  FormulaEdit *dialog = new FormulaEdit(this, d);
  int rc = dialog->exec();
  if (! rc)
  {
    delete dialog;
    return;
  }

  delete dialog;

  emit signalEditIndicator(d);

  updateList();
}

void IndicatorPage::deleteIndicator ()
{
  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        tr("Are you sure you want to permanently delete this indicator?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
  if (rc == QMessageBox::No)
    return;

  QString s = item->text();

  DataBase db;
  db.deleteChartObjectsIndicator(s);
  db.deleteIndicator(s);

  itemSelected();
  emit signalDeleteIndicator(s);
  updateList();
}

void IndicatorPage::updateList ()
{
  list->clear();

  DataBase db;
  QStringList l;
  db.getIndicatorList(l);
  
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    IndicatorIndex index;
    db.getIndicatorIndex(l[loop], index);

    if (! index.getEnable())
    {
      QListWidgetItem *item = new QListWidgetItem(QIcon(disable), l[loop]);
      list->addItem(item);
    }
    else
    {
      QListWidgetItem *item = new QListWidgetItem(QIcon(ok), l[loop]);
      list->addItem(item);
    }
  }

  itemSelected();
}

void IndicatorPage::doubleClick (QListWidgetItem *item)
{
  if (! item)
    return;

  QString s = item->text();
  changeIndicator(s);
}

void IndicatorPage::changeIndicator (QString &d)
{
  if (! d.length())
    return;

  QListWidgetItem *item = list->currentItem();
  if (! item)
    return;

  QString s = item->text();
  DataBase db;
  IndicatorIndex index;
  db.getIndicatorIndex(s, index);

  if (index.getEnable())
  {
    item->setIcon(QIcon(disable));
    index.setEnable(0);
    db.setIndicatorIndex(s, index);
    emit signalDisableIndicator(s);
  }
  else
  {
    item->setIcon(QIcon(ok));
    index.setEnable(1);
    db.setIndicatorIndex(s, index);
    emit signalEnableIndicator(s);
  }
}

void IndicatorPage::rightClick (const QPoint &)
{
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


