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
#include "IndicatorPlugin.h"
#include "../pics/help.xpm"
#include "../pics/ok.xpm"
#include "../pics/ok_gray.xpm"
#include "../pics/disable.xpm"
#include "../pics/disable_gray.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/newchart.xpm"
#include "../pics/move.xpm"
#include "../pics/dirclosed.xpm"
#include <qcursor.h>
#include <qdir.h>
#include <qaccel.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qtooltip.h>


IndicatorPage::IndicatorPage (QWidget *w) : QWidget (w)
{
  keyFlag = FALSE;
  macroFlag = FALSE;
  macro = 0;
  updateEnableFlag = FALSE;
  
  Config config;  
  baseDir = config.getData(Config::IndicatorPath);
  cusRulePath = config.getData(Config::CUSRulePath);
  currentGroup = config.getData(Config::IndicatorGroup);
  
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  group = new MyComboBox(this, Macro::IndicatorPage);
  updateGroups();
  connect(group, SIGNAL(activated(int)), this, SLOT(slotGroupChanged(int)));
  connect(group, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));
  QToolTip::add(group, tr("Indicator Groups"));
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
  menu->insertItem(QPixmap(dirclosed), tr("&New Indicator Group		Ctrl+N"), this, SLOT(newIndicatorGroup()));
  menu->insertItem(QPixmap(deleteitem), tr("Delete Indicator Group	Ctrl+X"), this, SLOT(deleteIndicatorGroup()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(newchart), tr("Ne&w Indicator		Ctrl+W"), this, SLOT(newIndicator()));
  menu->insertItem(QPixmap(edit), tr("&Edit Indicator			Ctrl+E"), this, SLOT(editIndicator()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Indicator		Ctrl+D"), this, SLOT(deleteIndicator()));
  menu->insertItem(QPixmap(moveitem), tr("Mo&ve Indicator		Ctrl+V"), this, SLOT(moveIndicator()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help			Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_N, NewIndicatorGroup);
  a->insertItem(CTRL+Key_X, DeleteIndicatorGroup);
  a->insertItem(CTRL+Key_W, NewIndicator);
  a->insertItem(CTRL+Key_D, DeleteIndicator);
  a->insertItem(CTRL+Key_E, EditIndicator);
  a->insertItem(CTRL+Key_V, MoveIndicator);
  a->insertItem(CTRL+Key_H, Help);
  a->insertItem(CTRL+Key_Tab, Tab);
  
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
  updateGroups();
}

void IndicatorPage::deleteIndicatorGroup ()
{
  Config config;
  QString s("*");
  QString s2(config.getData(Config::IndicatorPath));
  SymbolDialog *dialog = new SymbolDialog(this,
  					  s2,
					  s,
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
    if (! l.count())
    {
      delete dialog;
      return;
    }
    
    QDir dir(l[0]);
    bool flag = FALSE;
    if (! dir.dirName().compare(currentGroup))
      flag = TRUE;
    
    if (! dir.dirName().compare("Indicators"))
    {
      delete dialog;
      QMessageBox::information(this,
                               tr("Qtstalker: Delete Indicator Group"),
                               tr("Cannot delete default group."));
      return;
    }
    else
    {
      int loop;
      for (loop = 2; loop < (int) dir.count(); loop++)
      {
        QString s = dir.absPath() + "/" + dir[loop];
        if (! dir.remove(s, TRUE))
          qDebug("IndicatorPage::deleteGroupItem:failed to delete file");
      }
      
      if (! dir.rmdir(dir.absPath(), TRUE))
        qDebug("IndicatorPage::deleteGroupItem:failed to delete dir");
    }

    updateGroups();
    
    if (flag)
      slotGroupChanged(0);
      
    itemSelected(QString());
  }
  
  delete dialog;
}

void IndicatorPage::newIndicator ()
{
  Indicator *i = new Indicator;
  QStringList l;
  i->getPlotTypes(l);
  delete i;

  Config config;  
  PrefDialog *idialog = new PrefDialog;
  idialog->setCaption(tr("New Indicator"));
  QString s("newindicator.html");
  idialog->setHelpFile(s);
  
  QString pl = tr("Details");
  QString il = tr("Indicator");
  QString nl = tr("Name");
  QString ptl = tr("Plot Type");
  QString igl = tr("Indicator Group");
  
  idialog->createPage (pl);
  
  QStringList l2;
  config.getIndicatorList(l2);
  idialog->addComboItem(il, pl, l2, 0);
  
  s = tr("NewIndicator");
  idialog->addTextItem(nl, pl, s);
  
  idialog->addComboItem(ptl, pl, l, 1);
  
  getIndicatorGroups(l);
  idialog->addComboItem(igl, pl, l, 0);
  
  int rc = idialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete idialog;
    return;
  }
  
  QString name = idialog->getText(nl);
  QString indicator = idialog->getCombo(il);
  Indicator::PlotType plotType = (Indicator::PlotType) idialog->getComboIndex(ptl);
  QString igroup = idialog->getCombo(igl);
  delete idialog;
  
  if (! name.length())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Indicator name missing."));
    return;
  }
  
  // correct any forbidden chars in name
  int loop;
  s.truncate(0);
  for (loop = 0; loop < (int) name.length(); loop++)
  {
    QChar c = name.at(loop);
    if (c.isLetterOrNumber())
      s.append(c);
  }
  name = s;

  // check if we can save this indicator in current group  
  QDir dir;  
  s = baseDir + "/" + group->currentText() + "/" + name;
  if (dir.exists(s))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate indicator name."));
    return;
  }

  // get the indicator plugin
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
    
    if (! igroup.compare(tr("Local")))
    {
      QString plugin = config.parseDbPlugin(chartPath);
      DbPlugin *db = config.getDbPlugin(plugin);
      if (! db)
      {
        qDebug("IndicatorPage::newIndicator: can't open db plugin");
        config.closePlugin(plugin);
        config.closePlugin(indicator);
        return;
      }
  
      if (db->openChart(chartPath))
      {
        qDebug("IndicatorPage::newIndicator: can't open chart");
        config.closePlugin(plugin);
        config.closePlugin(indicator);
        return;
      }
    
      // get the indicator settings
      Setting tset;
      plug->getIndicatorSettings(tset);
      tset.setData("Name", name);
      tset.setData("enable", "1");
      tset.setData("plotType", QString::number(plotType));
      
      // save the local indicator to the db
      QString s2;
      tset.getString(s2);
      db->addIndicator(s2);
      
      localIndicators.append(name);
      
      config.closePlugin(plugin);
    }
    
    plug->saveIndicatorSettings(s);
    
    config.closePlugin(indicator);
    
    i = new Indicator;
    i->setName(name);
    i->setPlotType((Indicator::PlotType) plotType);
    i->setFile(s);
    i->setType(indicator);
    emit signalNewIndicator(i);
  }
  else
    config.closePlugin(indicator);
}

void IndicatorPage::editIndicator ()
{
  QString s = list->currentText();
  editIndicator(s);
}

void IndicatorPage::editIndicator (QString d)
{
  Config config;
  QDir dir;
  QString s = baseDir + "/" + group->currentText() + "/" + d;
  if (! dir.exists(s, TRUE))
  {
    qDebug("IndicatorPage::editIndicator: indicator not found %s", s.latin1());
    return;
  }
  
  Setting set;
  config.getIndicator(s, set);
  if (! set.count())
  {
    qDebug("IndicatorPage::editIndicator:indicator settings empty");
    return;
  }
  
  QString type = set.getData("plugin");
  
  IndicatorPlugin *plug = config.getIndicatorPlugin(type);
  if (! plug)
  {
    qDebug("IndicatorPage::editIndicator - could not open plugin");
    config.closePlugin(type);
    return;
  }

  plug->loadIndicatorSettings(s);
  int rc = plug->indicatorPrefDialog(this);

  if (rc)
  {
    plug->saveIndicatorSettings(s);
    
    Setting tset;
    plug->getIndicatorSettings(tset);
    saveLocalIndicator(d, tset);
    
    config.closePlugin(type);
    
    if (set.getInt("enable"))
    {
      Indicator *i = new Indicator;
      i->setName(d);
      i->setPlotType((Indicator::PlotType) set.getInt("plotType"));
      i->setFile(s);
      i->setType(type);
      emit signalEditIndicator(i);
    }
  }
  else
    config.closePlugin(type);
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

  QDir dir;
  Config config;
  QString s = config.getData(Config::IndicatorPath) + "/" + group->currentText() + "/" + list->currentText();
  if (! dir.exists(s, TRUE))
  {
    qDebug("IndicatorPage::deleteIndicator: indicator not found %s", s.latin1());
    return;
  }
  
  if (chartPath.length())
  {
    QString plugin = config.parseDbPlugin(chartPath);
    DbPlugin *db = config.getDbPlugin(plugin);
    if (! db)
    {
      qDebug("IndicatorPage::slotDeleteIndicator:can't get db plugin");
      config.closePlugin(plugin);
      return;
    }
    
    if (db->openChart(chartPath))
    {
      qDebug("IndicatorPage::slotDeleteIndicator: can't open chart");
      config.closePlugin(plugin);
      return;
    }
    
    QStringList l;
    db->getChartObjects(l);
    int loop;
    Setting set;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      set.parse(l[loop]);
      if (! set.getData("Plot").compare(list->currentText()))
      {
        s = set.getData("Name");
        db->deleteChartObject(s);
      }
    }
  
    s = list->currentText();
    db->deleteIndicator(s);  
    
    config.closePlugin(plugin);
  }

  itemSelected(QString());
  localIndicators.remove(list->currentText());
  emit signalDeleteIndicator(list->currentText());
}

void IndicatorPage::moveIndicator ()
{
  Indicator *i = new Indicator;
  QStringList pl;
  i->getPlotTypes(pl);
  delete i;

  Config config;
  QDir dir;
  QString s = baseDir + "/" + group->currentText() + "/" + list->currentText();
  if (! dir.exists(s, TRUE))
  {
    qDebug("IndicatorPage::moveIndicator: indicator not found %s", s.latin1());
    return;
  }
  
  Setting set;
  config.getIndicator(s, set);
  if (! set.count())
  {
    qDebug("IndicatorPage::moveIndicator:indicator settings empty");
    return;
  }
  
  PrefDialog *dialog = new PrefDialog;
  dialog->setCaption(tr("Move Indicator"));
  QString t("workwithmainindicators.html");  
  dialog->setHelpFile(t);
  
  t = tr("Details");
  dialog->createPage(t);
  QString t2(tr("Plot Type"));
  dialog->addComboItem(t2, t, pl, set.getInt("plotType"));
  QString igroup = group->currentText();
  if (localIndicators.findIndex(list->currentText()) != -1)
    igroup = tr("Local");
  t2 = tr("Indicator Group");
  dialog->addComboItem(t2, t, pl, igroup);
  
  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  t = tr("Plot Type");
  set.setData("plotType", QString::number(dialog->getComboIndex(t)));
  
  t = tr("Indicator Group");
  QString s2 = dialog->getCombo(t);
  if (! igroup.compare(tr("Local")))
  {
    if (s2.compare(tr("Local")))
    {
      Config config;
      QString plugin = config.parseDbPlugin(chartPath);
      DbPlugin *db = config.getDbPlugin(plugin);
      if (! db)
      {
        qDebug("IndicatorPage::moveIndicator: can't open db plugin");
        config.closePlugin(plugin);
        return;
      }
  
      if (db->openChart(chartPath))
      {
        qDebug("IndicatorPage::moveIndicator: can't open chart");
        config.closePlugin(plugin);
        return;
      }

      QString t = list->currentText();
      db->deleteIndicator(t);
      
      config.closePlugin(plugin);
      
      localIndicators.remove(list->currentText());
    }
  }
  else
  {
    if (! s2.compare(tr("Local")))
    {
      Config config;
      QString plugin = config.parseDbPlugin(chartPath);
      DbPlugin *db = config.getDbPlugin(plugin);
      if (! db)
      {
        qDebug("IndicatorPage::moveIndicator: can't open db plugin");
        config.closePlugin(plugin);
        return;
      }
  
      if (db->openChart(chartPath))
      {
        qDebug("IndicatorPage::moveIndicator: can't open chart");
        config.closePlugin(plugin);
        return;
      }
      
      QString t;
      set.getString(t);
      db->addIndicator(t);
      localIndicators.append(list->currentText());
      
      config.closePlugin(plugin);
    }
  }

  bool status = set.getInt("enable");
  if (status)
    emit signalDisableIndicator(s);

  t = list->currentText();
  saveLocalIndicator(t, set);
  
  config.setIndicator(s, set);
  
  if (status)
    emit signalEnableIndicator(s);
    
  updateList();  
}

void IndicatorPage::updateList ()
{
  list->clear();
  
  Config config;
  QStringList l;
  QString s(getIndicatorGroup());
  config.getIndicators(s, l);

  int loop;  
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QFileInfo fi(l[loop]);
    Setting set;
    config.getIndicator(l[loop], set);
    if (! set.getInt("enable"))
    {
      if (localIndicators.findIndex(fi.fileName()) != -1)
        list->insertItem(disable_gray, fi.fileName(), -1);
      else
        list->insertItem(disable, fi.fileName(), -1);
    }
    else
    {
      if (localIndicators.findIndex(fi.fileName()) != -1)
        list->insertItem(ok_gray, fi.fileName(), -1);
      else
        list->insertItem(ok, fi.fileName(), -1);
	
      if (updateEnableFlag)
        emit signalEnableIndicator(l[loop]);
    }
  }

  itemSelected(QString());
}

void IndicatorPage::doubleClick (QListBoxItem *item)
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
    
  QDir dir;
  QString s = baseDir + "/" + group->currentText() + "/" + d;
  if (! dir.exists(s, TRUE))
  {
    qDebug("IndicatorPage::changeIndicator: indicator not found %s", s.latin1());
    return;
  }
  
  Config config;
  Setting set;
  config.getIndicator(s, set);
  int t = set.getInt("enable");
  if (t)
  {
    set.setData("enable", "0");
    config.setIndicator(s, set);
    if (localIndicators.findIndex(list->currentText()) != -1)
      list->changeItem(disable_gray, d, list->currentItem());
    else
      list->changeItem(disable, d, list->currentItem());
    saveLocalIndicator(d, set);
    emit signalDisableIndicator(s);
  }
  else
  {
    set.setData("enable", "1");
    config.setIndicator(s, set);
    if (localIndicators.findIndex(list->currentText()) != -1)
      list->changeItem(ok_gray, d, list->currentItem());
    else
      list->changeItem(ok, d, list->currentItem());
    saveLocalIndicator(d, set);
    emit signalEnableIndicator(s);
  }
}

void IndicatorPage::slotHelp ()
{
  QString s = "workwithmainindicators.html";
  HelpWindow *hw = new HelpWindow(this, s);
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
      case Qt::Key_W:
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
    case NewIndicator:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_W, 0, QString());
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
  if (group->count() == 1 && ! group->currentText().compare(currentGroup))
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
  group->blockSignals(TRUE);
  
  group->clear();
  
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
  group->insertStringList(l, -1);

  group->setCurrentItem(l.findIndex(currentGroup));
    
  group->blockSignals(FALSE);
}

void IndicatorPage::getIndicatorGroups (QStringList &l)
{
  l.clear();
  l.append(group->currentText());
  
  if (chartPath.length())
    l.append(tr("Local"));
}

QString IndicatorPage::getIndicatorGroup ()
{
  return group->currentText();
}

void IndicatorPage::setFocus ()
{
  list->setFocus();
}

void IndicatorPage::setChartPath (QString &d)
{
  chartPath = d;
}

void IndicatorPage::addLocalIndicators (QString &d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  int loop;
  Setting set;
  Config config;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    set.parse(l[loop]);
    QString s = baseDir + "/" + group->currentText() + "/" + set.getData("Name");
    config.setIndicator(s, set);
    if (set.getInt("enable"))
      emit signalEnableIndicator(s);
    localIndicators.append(set.getData("Name"));
  }
}

void IndicatorPage::removeLocalIndicators ()
{
  int loop;
  QDir dir;
  for (loop = 0; loop < (int) localIndicators.count(); loop++)
  {
    QString s = baseDir + "/" + group->currentText() + "/" + localIndicators[loop];
    emit signalDisableIndicator(s);
  }

  for (loop = 0; loop < (int) localIndicators.count(); loop++)
  {
    QString s = baseDir + "/" + group->currentText() + "/" + localIndicators[loop];
    dir.remove(s, TRUE);
  }
    
  localIndicators.clear();
}

void IndicatorPage::saveLocalIndicator (QString &d, Setting &set)
{
  if (localIndicators.findIndex(d) != -1)
  {
    Config config;
    QString plugin = config.parseDbPlugin(chartPath);
    DbPlugin *db = config.getDbPlugin(plugin);
    if (! db)
    {
      qDebug("IndicatorPage::saveLocalIndicator: can't open db plugin");
      config.closePlugin(plugin);
      return;
    }
  
    if (db->openChart(chartPath))
    {
      qDebug("IndicatorPage::saveLocalIndicator: can't open chart");
      config.closePlugin(plugin);
      return;
    }
    
    // save the local indicator to the db
    QString t;
    set.getString(t);
    db->setIndicator(d, t);
    
    config.closePlugin(plugin);
  }
}

