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
#include "SymbolDialog.h"
#include "Indicator.h"
#include "IndicatorPlugin.h"
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
#include <qinputdialog.h>
#include <qmessagebox.h>


IndicatorPage::IndicatorPage (QWidget *w) : QWidget (w)
{
  keyFlag = FALSE;
  macroFlag = FALSE;
  macro = 0;
  updateEnableFlag = FALSE;
  
  Config config;  
  baseDir = config.getData(Config::IndicatorPath);
  currentGroup = config.getData(Config::IndicatorGroup);
  
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  group = new MyComboBox(this, Macro::IndicatorPage);
  connect(group, SIGNAL(activated(int)), this, SLOT(slotGroupChanged(int)));
  connect(group, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));
  vbox->addWidget(group);

  list = new MyListBox(this, Macro::IndicatorPage);
  connect(list, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  connect(list, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(QListBoxItem *)));
  connect(list, SIGNAL(highlighted(const QString &)), this, SLOT(itemSelected(const QString &)));
  connect(list, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));
  vbox->addWidget(list);
    
  menu = new QPopupMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Indicator Group		Ctrl+N"), this, SLOT(newIndicatorGroup()));
  menu->insertItem(QPixmap(newchart), tr("Delete Indicator Group	Ctrl+X"), this, SLOT(deleteIndicatorGroup()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(newchart), tr("&Add Indicator		Ctrl+A"), this, SLOT(newIndicator()));
  menu->insertItem(QPixmap(edit), tr("&Edit Indicator			Ctrl+E"), this, SLOT(editIndicator()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Indicator		Ctrl+D"), this, SLOT(deleteIndicator()));
  menu->insertItem(QPixmap(moveitem), tr("Mo&ve Indicator		Ctrl+V"), this, SLOT(moveIndicator()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help			Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_N, NewIndicatorGroup);
  a->insertItem(CTRL+Key_X, DeleteIndicatorGroup);
  a->insertItem(CTRL+Key_A, AddIndicator);
  a->insertItem(CTRL+Key_D, DeleteIndicator);
  a->insertItem(CTRL+Key_E, EditIndicator);
  a->insertItem(CTRL+Key_V, MoveIndicator);
  a->insertItem(CTRL+Key_H, Help);
  a->insertItem(CTRL+Key_Tab, Tab);
  
  updateGroups();
  itemSelected(QString());
}

IndicatorPage::~IndicatorPage ()
{
}

void IndicatorPage::itemSelected (const QString &d)
{
  if (d.length())
  {
    menu->setItemEnabled(menu->idAt(4), TRUE);
    menu->setItemEnabled(menu->idAt(5), TRUE);
    menu->setItemEnabled(menu->idAt(6), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(4), FALSE);
    menu->setItemEnabled(menu->idAt(5), FALSE);
    menu->setItemEnabled(menu->idAt(6), FALSE);
  }
}

void IndicatorPage::newIndicatorGroup ()
{
  bool ok;
  QString s = QInputDialog::getText(tr("New Indicator Group"),
  				    tr("Enter new group symbol."),
				    QLineEdit::Normal,
				    tr("NewGroup"),
				    &ok,
				    this);
  if ((! ok) || (s.isNull()))
    return;

  int loop;
  QString selection;
  for (loop = 0; loop < (int) s.length(); loop++)
  {
    QChar c = s.at(loop);
    if (c.isLetterOrNumber())
      selection.append(c);
  }
  
  s = baseDir + "/" + selection;
  QDir dir(s);
  if (dir.exists(s, TRUE))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This group already exists."));
    return;
  }

  dir.mkdir(s, TRUE);
  updateList();
  updateGroups();
}

void IndicatorPage::deleteIndicatorGroup ()
{
  Config config;
  SymbolDialog *dialog = new SymbolDialog(this,
  					  config.getData(Config::IndicatorPath),
					  "*",
					  QFileDialog::DirectoryOnly);
  dialog->setCaption(tr("Select Indicator Group(s) To Delete"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    rc = QMessageBox::warning(this,
  			      tr("Qtstalker: Warning"),
			      tr("Are you sure you want to delete indicator group items?"),
			      QMessageBox::Yes,
			      QMessageBox::No,
			      QMessageBox::NoButton);

    if (rc == QMessageBox::No)
    {
      delete dialog;
      return;
    }

    QStringList l = dialog->selectedFile();
    int loop;
    QDir dir;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      dir.setPath(l[loop]);
      if (! dir.dirName().compare("Indicators")) // dont delete the default group
        continue;
	
      int loop2;
      for (loop2 = 2; loop2 < (int) dir.count(); loop2++)
      {
        QString s = dir.absPath() + "/" + dir[loop2];
        if (! dir.remove(s, TRUE))
          qDebug("IndicatorPage::deleteGroupItem:failed to delete file");
      }
      
      if (! dir.rmdir(l[loop], TRUE))
        qDebug("IndicatorPage::deleteGroupItem:failed to delete dir");
    }

    updateList();
    updateGroups();
    itemSelected(QString());
  }
  
  delete dialog;
}

void IndicatorPage::newIndicator ()
{
  Indicator *ti = new Indicator;
  QStringList l = ti->getPlotTypes();
  delete ti;

  Config config;  
  PrefDialog *idialog = new PrefDialog;
  idialog->setCaption(tr("New Indicator"));
  idialog->createPage (tr("Details"));
  idialog->setHelpFile("newindicator.html");
  idialog->addComboItem(tr("Indicator"), tr("Details"), config.getIndicatorList(), 0);
  idialog->addTextItem(tr("Name"), tr("Details"), tr("NewIndicator"));
  idialog->addComboItem(tr("Plot Type"), tr("Details"), l, 1);
  idialog->addComboItem(tr("Indicator Group"), tr("Details"), getIndicatorGroups(), 0);
  
  int rc = idialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete idialog;
    return;
  }
  
  QString name = idialog->getText(tr("Name"));
  QString indicator = idialog->getCombo(tr("Indicator"));
  Indicator::PlotType plotType = (Indicator::PlotType) idialog->getComboIndex(tr("Plot Type"));
  QString group = idialog->getCombo(tr("Indicator Group"));
  delete idialog;
  
  if (! name.length())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Indicator name missing."));
    return;
  }
  
  int loop;
  QString s;
  for (loop = 0; loop < (int) name.length(); loop++)
  {
    QChar c = name.at(loop);
    if (c.isLetterOrNumber())
      s.append(c);
  }
  name = s;

  QDir dir;
  s = baseDir + "/" + group + "/" + name;
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate indicator name."));
    return;
  }

  IndicatorPlugin *plug = config.getIndicatorPlugin(indicator);
  if (! plug)
  {
    qDebug("IndicatorPage::newIndicator - could not open plugin");
    config.closePlugin(indicator);
    return;
  }

  rc = plug->indicatorPrefDialog(this);
  
  if (rc)
  {
    plug->setPlotType((int) plotType);
    plug->saveIndicatorSettings(s);
    config.closePlugin(indicator);
    
    Setting *set = new Setting;
    set->setData("Name", name);
    set->setData("PlotType", QString::number(plotType));
    set->setData("File", s);
    set->setData("Indicator", indicator);
    emit signalNewIndicator(set);
  }
  else
    config.closePlugin(indicator);
}

void IndicatorPage::editIndicator ()
{
  editIndicator(list->currentText());
}

void IndicatorPage::editIndicator (QString d)
{
  Config config;
  QString s = baseDir + "/" + group->currentText() + "/" + d;
  Setting *set = config.getIndicator(s);
  if (! set->count())
  {
    delete set;
    qDebug("IndicatorPage::editIndicator:indicator settings empty");
  }
  
  QString type = set->getData("plugin");
  
  IndicatorPlugin *plug = config.getIndicatorPlugin(type);
  if (! plug)
  {
    qDebug("IndicatorPage::editIndicator - could not open plugin");
    config.closePlugin(type);
    delete set;
    return;
  }

  plug->loadIndicatorSettings(s);
  int rc = plug->indicatorPrefDialog(this);

  if (rc)
  {
    plug->saveIndicatorSettings(s);
    config.closePlugin(type);
    
    if (set->getInt("enable"))
    {
      set->setData("Name", d);
      set->setData("File", s);
      emit signalEditIndicator(set);
    }
  }
  else
    config.closePlugin(type);
}

void IndicatorPage::deleteIndicator ()
{
  emit signalDeleteIndicator(list->currentText());
}

void IndicatorPage::moveIndicator ()
{
  Indicator *i = new Indicator;
  QStringList pl = i->getPlotTypes();
  delete i;

  Config config;
  QString s =  baseDir + "/" + group->currentText() + "/" + list->currentText();
  Setting *set = config.getIndicator(s);
  if (! set->count())
  {
    delete set;
    qDebug("IndicatorPage::moveIndicator:indicator settings empty");
  }
  
  PrefDialog *dialog = new PrefDialog;
  dialog->setCaption(tr("Move Indicator"));
  dialog->createPage (tr("Details"));
  dialog->setHelpFile("workwithmainindicators.html");
  dialog->addComboItem(tr("Plot Type"), tr("Details"), pl, set->getInt("plotType"));
  
  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    delete set;
    return;
  }
  
  set->setData("plotType", QString::number(dialog->getComboIndex(tr("Plot Type"))));

  bool status = set->getInt("enable");
  if (status)
    emit signalDisableIndicator(s);
    
  config.setIndicator(s, set);
  
  if (status)
    emit signalEnableIndicator(s);
}

void IndicatorPage::updateList ()
{
  list->clear();
  
  Config config;
  QStringList l = config.getIndicators(getIndicatorGroup());
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QFileInfo fi(l[loop]);
    Setting *set = config.getIndicator(l[loop]);
    if (! set->getInt("enable"))
      list->insertItem(disable, fi.fileName(), -1);
    else
    {
      list->insertItem(ok, fi.fileName(), -1);
      if (updateEnableFlag)
        emit signalEnableIndicator(l[loop]);
    }
      
    delete set;
  }
  
  itemSelected(QString());
}

void IndicatorPage::doubleClick (QListBoxItem *item)
{
  if (! item)
    return;
    
  changeIndicator(item->text());
}

void IndicatorPage::changeIndicator (QString d)
{
  if (! d.length())
    return;
    
  Config config;
  QString s = baseDir + "/" + group->currentText() + "/" + d;
  Setting *set = config.getIndicator(s);
  int t = set->getInt("enable");
  if (t)
  {
    set->setData("enable", "0");
    config.setIndicator(s, set);
    list->changeItem(disable, d, list->currentItem());
    emit signalDisableIndicator(s);
  }
  else
  {
    set->setData("enable", "1");
    config.setIndicator(s, set);
    list->changeItem(ok, d, list->currentItem());
    emit signalEnableIndicator(s);
  }
  
  delete set;
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

void IndicatorPage::setKeyFlag (bool d)
{
  keyFlag = d;
  list->setKeyFlag(d);
  group->setKeyFlag(d);
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
        slotAccel(NewIndicatorGroup);
	break;
      case Qt::Key_X:
        slotAccel(DeleteIndicatorGroup);
	break;
      case Qt::Key_A:
        slotAccel(AddIndicator);
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
      case Qt::Key_Tab:
        slotAccel(Tab);
	break;
      default:
        break;
    }
  }
  else
  {
    if (list->hasFocus())
      list->doKeyPress(key);
    else
      group->doKeyPress(key);
  }
}

void IndicatorPage::slotAccel (int id)
{
  switch (id)
  {
    case NewIndicatorGroup:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_N, 0, QString());
      newIndicatorGroup();
      break;  
    case DeleteIndicatorGroup:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_X, 0, QString());
      deleteIndicatorGroup();
      break;  
    case AddIndicator:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_A, 0, QString());
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
    case Tab:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_Tab, 0, QString());
      if (list->hasFocus())
        list->setFocus();
      else
        group->setFocus();
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

void IndicatorPage::slotGroupChanged (int)
{
  if (group->count() == 1)
    return;

  int loop;
  for (loop = 0; loop < (int) list->count(); loop++)
  {
    QString s = baseDir + "/" + currentGroup + "/" + list->text(loop);    
    emit signalDisableIndicator(s);
  }
   
  currentGroup = group->currentText();
  
  updateEnableFlag = TRUE;
  updateList();
  updateEnableFlag = FALSE;
  
  emit signalReloadChart();
}

void IndicatorPage::updateGroups ()
{
//  group->blockSignals(TRUE);
  
  group->clear();
  group->insertStringList(getIndicatorGroups(), -1);
  if (currentGroup.length())  
    group->setCurrentText(currentGroup);
  
//  group->blockSignals(FALSE);
}

QStringList IndicatorPage::getIndicatorGroups ()
{
  QStringList l;
  QDir dir(baseDir);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = dir.absPath() + "/" + dir[loop];
    QFileInfo fi(s);
    if (fi.isDir())
      l.append(dir[loop]);
  }
  return l;
}

QString IndicatorPage::getIndicatorGroup ()
{
  return group->currentText();
}

void IndicatorPage::setFocus ()
{
  list->setFocus();
}
