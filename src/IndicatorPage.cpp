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

#include "IndicatorPage.h"
#include "HelpWindow.h"
#include "Config.h"
#include "PrefDialog.h"
#include "help.xpm"
#include "ok.xpm"
#include "disable.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "newchart.xpm"
#include "move.xpm"
#include <qcursor.h>
#include <qdir.h>
#include <qaccel.h>
#include <qfile.h>
#include <qtextstream.h>

IndicatorPage::IndicatorPage (QWidget *w) : QListBox (w)
{
  keyFlag = FALSE;
  macroFlag = FALSE;
  macro = 0;
  
  connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  connect(this, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(QListBoxItem *)));
  connect(this, SIGNAL(highlighted(const QString &)), this, SLOT(itemSelected(const QString &)));

  menu = new QPopupMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Indicator	Ctrl+N"), this, SLOT(newIndicator()));
  menu->insertItem(QPixmap(edit), tr("&Edit Indicator		Ctrl+E"), this, SLOT(editIndicator()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Indicator	Ctrl+D"), this, SLOT(deleteIndicator()));
  menu->insertItem(QPixmap(moveitem), tr("Mo&ve Indicator	Ctrl+V"), this, SLOT(moveIndicator()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_N, NewIndicator);
  a->insertItem(CTRL+Key_D, DeleteIndicator);
  a->insertItem(CTRL+Key_E, EditIndicator);
  a->insertItem(CTRL+Key_V, MoveIndicator);
  a->insertItem(CTRL+Key_H, Help);
}

IndicatorPage::~IndicatorPage ()
{
}

void IndicatorPage::itemSelected (const QString &d)
{
  if (d.length())
  {
    menu->setItemEnabled(menu->idAt(1), TRUE);
    menu->setItemEnabled(menu->idAt(2), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(1), FALSE);
    menu->setItemEnabled(menu->idAt(2), FALSE);
  }
}

void IndicatorPage::newIndicator ()
{
  emit signalNewIndicator();
}

void IndicatorPage::editIndicator ()
{
  emit signalEditIndicator(currentText());
}

void IndicatorPage::deleteIndicator ()
{
  emit signalDeleteIndicator(currentText());
}

void IndicatorPage::moveIndicator ()
{
  Indicator *i = new Indicator;
  QStringList pl = i->getPlotTypes();
  delete i;

  Config config;
  QStringList il;
  int type = 1;
  int line = -1;
  QString s =  config.getData(Config::IndicatorPath) + "/" + currentText();
  
  QFile f(s);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;
      
    il.append(s);
    
    QStringList l = QStringList::split("=", s, FALSE);
    if (! l[0].compare("plotType"))
    {
      type = l[1].toInt();
      line = il.count() - 1;
    }
  }
  f.close();
  
  if (line == -1)
    return;
  
  PrefDialog *dialog = new PrefDialog;
  dialog->setCaption(tr("Move Indicator"));
  dialog->createPage (tr("Details"));
  dialog->setHelpFile("workwithmainindicators.html");
  dialog->addComboItem(tr("Plot Type"), tr("Details"), pl, type);
  
  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }
  
  il[line] = "plotType=" + QString::number(dialog->getComboIndex(tr("Plot Type")));

  emit signalDisableIndicator(currentText());
  
  QFile f2(s);
  if (! f2.open(IO_WriteOnly))
    return;
  QTextStream stream2(&f2);

  int loop;
  for (loop = 0; loop < (int) il.count(); loop++)
    stream2 << il[loop] << "\n";
    
  f2.close();
  
  emit signalEnableIndicator(currentText());
}

void IndicatorPage::refreshList ()
{
  clear();
  statusList.clear();

  Config config;
  QDir dir(config.getData(Config::IndicatorPath));
  QStringList l = QStringList::split(",", config.getData(Config::IndicatorPageStatus), FALSE);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    if (l.findIndex(dir[loop]) == -1)
    {
      insertItem(ok, dir[loop], -1);
      statusList.setData(dir[loop], "1");
    }
    else
    {
      insertItem(disable, dir[loop], -1);
      statusList.setData(dir[loop], "0");
    }
  }
}

void IndicatorPage::doubleClick (QListBoxItem *item)
{
  if (! item)
    return;
    
  changeIndicator(item->text(), index(item));
}

void IndicatorPage::changeIndicator (QString d, int i)
{
  if (! d.length())
    return;
    
  if (statusList.getInt(d))
  {
    changeItem(disable, d, i);
    statusList.setData(d, "0");
    emit signalDisableIndicator(d);
  }
  else
  {
    changeItem(ok, d, i);
    statusList.setData(d, "1");
    emit signalEnableIndicator(d);
  }
}

void IndicatorPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithmainindicators.html");
  hw->show();
}

void IndicatorPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void IndicatorPage::saveStatus ()
{
  QStringList l = statusList.getKeyList();
  QStringList l2;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    if (! statusList.getInt(l[loop]))
      l2.append(l[loop]);
  }

  Config config;
  config.setData(Config::IndicatorPageStatus, l2.join(","));
}

QStringList IndicatorPage::getDisabledIndicators ()
{
  QStringList l = statusList.getKeyList();
  QStringList l2;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    if (! statusList.getInt(l[loop]))
      l2.append(l[loop]);
  }

  return l2;
}

bool IndicatorPage::getIndicatorStatus (QString d)
{
  return (bool) statusList.getInt(d);
}

void IndicatorPage::setKeyFlag (bool d)
{
  keyFlag = d;
}

void IndicatorPage::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (Macro::IndicatorPage,
    			   key->state(),
			   key->key(),
			   key->ascii(),
			   key->text());
    
  doKeyPress(key);  
}

void IndicatorPage::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (key->state() == Qt::ControlButton)
  {
    switch(key->key())
    {
      case Qt::Key_N:
        slotAccel(NewIndicator);
	break;
      case Qt::Key_D:
        slotAccel(DeleteIndicator);
	break;
      case Qt::Key_E:
        slotAccel(EditIndicator);
	break;
      case Qt::Key_V:
        slotAccel(MoveIndicator);
	break;
      default:
        break;
    }
  }
  else
  {
    switch (key->key())
    {
      case Qt::Key_Left: // segfaults if we dont trap this
      case Qt::Key_Right: // segfaults if we dont trap this
        break;      
      case Qt::Key_Enter:
      case Qt::Key_Return:
        changeIndicator(currentText(), currentItem());
        break;
      default:
        QListBox::keyPressEvent(key);
        break;
    }
  }
}

void IndicatorPage::slotAccel (int id)
{
  switch (id)
  {
    case NewIndicator:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_N, 0, QString());
      newIndicator();
      break;  
    case DeleteIndicator:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_D, 0, QString());
      deleteIndicator();
      break;  
    case EditIndicator:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_E, 0, QString());
      editIndicator();
      break;  
    case MoveIndicator:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_V, 0, QString());
      moveIndicator();
      break;  
    case Help:
      slotHelp();
      break;  
    default:
      break;
  }
}

void IndicatorPage::runMacro (Macro *d)
{
  macro = d;
  macroFlag = TRUE;
  
  while (macro->getZone(macro->getIndex()) == Macro::IndicatorPage)
  {
    doKeyPress(macro->getKey(macro->getIndex()));
    
    macro->incIndex();
    if (macro->getIndex() >= macro->getCount())
      break;
  }
  
  macroFlag = FALSE;
}

