/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "PlotMenu.h"
#include "ChartObject.h"
#include "Globals.h"
#include "Script.h"

#include "../pics/loggrid.xpm"
#include "../pics/date.xpm"
#include "../pics/delete.xpm"
#include "../pics/buyarrow.xpm"
#include "../pics/sellarrow.xpm"
#include "../pics/fib.xpm"
#include "../pics/horizontal.xpm"
#include "../pics/text.xpm"
#include "../pics/trend.xpm"
#include "../pics/vertical.xpm"
#include "../pics/edit.xpm"

#include <QtDebug>
#include <QSettings>

PlotMenu::PlotMenu (QWidget *p) : QMenu (p)
{
  init ();
}

void PlotMenu::init ()
{
  _coListMenu = new QMenu(this);
  _coListMenu->setTitle(tr("New Chart Object..."));
  
  QAction *a = _coListMenu->addAction(QPixmap(buyarrow_xpm), tr("&Buy"));
  a->setShortcut(Qt::ALT+Qt::Key_B);
  a->setData(QVariant(ChartObject::_Buy));
  
  a = _coListMenu->addAction(QPixmap(horizontal_xpm), tr("&HLine"));
  a->setShortcut(Qt::ALT+Qt::Key_H);
  a->setData(QVariant(ChartObject::_HLine));
  
  a = _coListMenu->addAction(QPixmap(fib_xpm), tr("&Retracement"));
  a->setShortcut(Qt::ALT+Qt::Key_R);
  a->setData(QVariant(ChartObject::_Retracement));
  
  a = _coListMenu->addAction(QPixmap(sellarrow_xpm), tr("&Sell"));
  a->setShortcut(Qt::ALT+Qt::Key_S);
  a->setData(QVariant(ChartObject::_Sell));
  
  a = _coListMenu->addAction(QPixmap(text_xpm), tr("Te&xt"));
  a->setShortcut(Qt::ALT+Qt::Key_X);
  a->setData(QVariant(ChartObject::_Text));
  
  a = _coListMenu->addAction(QPixmap(trend_xpm), tr("&TLine"));
  a->setShortcut(Qt::ALT+Qt::Key_T);
  a->setData(QVariant(ChartObject::_TLine));
  
  a = _coListMenu->addAction(QPixmap(vertical_xpm), tr("&VLine"));
  a->setShortcut(Qt::ALT+Qt::Key_V);
  a->setData(QVariant(ChartObject::_VLine));
  
  connect(_coListMenu, SIGNAL(triggered(QAction *)), this, SLOT(chartObjectMenuSelected(QAction *)));
  
//  addAction(QPixmap(indicator_xpm), tr("&New Indicator..."), this, SIGNAL(signalNewIndicator()), Qt::ALT+Qt::Key_N);
  
  addAction(QPixmap(edit_xpm), tr("Edit &Indicator..."), this, SLOT(editIndicator()), Qt::ALT+Qt::Key_I);
  
  addAction(QPixmap(delete_xpm), tr("De&lete Indicator..."), this, SLOT(deleteIndicator()), Qt::ALT+Qt::Key_L);
  
  addSeparator ();
  
  addMenu(_coListMenu);
  
  addAction(QPixmap(delete_xpm), tr("Delete &All Chart Objects"), this, SLOT(deleteAllChartObjects()), Qt::ALT+Qt::Key_A);
  
  addSeparator ();
  
  _dateAction = addAction(QPixmap(date), tr("&Date"), this, SIGNAL(signalToggleDate()), Qt::ALT+Qt::Key_D);
  _dateAction->setCheckable(TRUE);
  
  _logAction = addAction(QPixmap(loggridicon), tr("Log &Scaling"), this, SIGNAL(signalToggleLog()), Qt::ALT+Qt::Key_S);
  _logAction->setCheckable(TRUE);
}

void PlotMenu::editIndicator ()
{
  QSettings settings(g_settingsFile);
  
  Script *script = new Script;
  script->setName("IndicatorEdit");
  script->setFile(settings.value("indicator_edit_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void PlotMenu::deleteIndicator ()
{
  QSettings settings(g_settingsFile);
  
  Script *script = new Script;
  script->setName("IndicatorDelete");
  script->setFile(settings.value("indicator_delete_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void PlotMenu::deleteAllChartObjects ()
{
  QSettings settings(g_settingsFile);

  Script *script = new Script;
  script->setName("ChartObjectDeleteAll");
  script->setFile(settings.value("chart_object_delete_all_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void PlotMenu::chartObjectMenuSelected (QAction *a)
{
  emit signalNewChartObject(a->data().toInt());
}

void PlotMenu::setCOMenuStatus (bool status)
{
  _coListMenu->setEnabled(status);
}

void PlotMenu::setLogStatus (bool status)
{
  _logAction->setChecked(status);
}

void PlotMenu::setDateStatus (bool status)
{
  _dateAction->setChecked(status);
}
