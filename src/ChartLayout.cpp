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

#include "ChartLayout.h"
#include "Plot.h"
#include "Globals.h"
#include "Command.h"
#include "Script.h"

#include <QDebug>
#include <QCursor>
#include <QLabel>
#include <QTabBar>
#include <qwt_plot.h>
#include <QSettings>

ChartLayout::ChartLayout ()
{
  _startIndex = -1;
  _barSpacing = 8;
  
  setOpaqueResize(FALSE); // only draw widget after resize is complete to speed things up

  int loop = 1;
  for (; loop < 4; loop++)
  {
    QWidget *w = new QWidget;
    _holders.insert(loop, w);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->setSpacing(0);
    w->setLayout(hbox);
    _grids.insert(loop, hbox);

    addWidget(w);
    w->hide();
    
    QString key = QString::number(loop);
    TabWidget *tab = new TabWidget(key);
    _tabs.insert(key, tab);
    hbox->addWidget(tab);
    tab->hide();
  }
}

void ChartLayout::save ()
{
  QSettings settings;
  settings.beginGroup("main" + g_session);

  QStringList l;
  QList<int> sizeList = sizes();
  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    l.append(QString::number(sizeList[loop]));
  settings.setValue("chart_layout_sizes", l);

  QHashIterator<QString, TabWidget *> it(_tabs);
  while (it.hasNext())
  {
    it.next();
    Plot *p = (Plot *) it.value()->currentWidget();
    if (p)
      settings.setValue("chart_layout_" + it.key(), it.value()->currentIndex());
  }

  settings.sync();

  // save any chart object changes
  emit signalSaveSettings();
}

void ChartLayout::load ()
{
  // we need to load our splitter sizes
  QSettings settings;
  settings.beginGroup("main" + g_session);

  QStringList l = settings.value("chart_layout_sizes").toStringList();
  if (! l.count())
  {
    QList<int> l2;
    l2 << 437 << 20 << 200;
    setSizes(l2);
  }
  else
  {
    QList<int> sizeList = sizes();

    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      if (loop >= sizeList.count())
        break;

      if (l[loop].toInt() < 25)
        sizeList[loop] = 25;
      else
        sizeList[loop] = l[loop].toInt();
    }

    setSizes(sizeList);
  }

  // setup the initial indicators
  l.clear();
  l << "INDICATOR_DATABASE" << "INDICATORS";

  Command command(l.join(","));

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ChartLayout::load: no plugin" << command.plugin();
    return;
  }

  if (plug->command(&command))
  {
    delete plug;
    qDebug() << "ChartLayout::load: command failed";
    return;
  }

  l = command.stringData().split(",", QString::SkipEmptyParts);
  
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList cl;
    cl << "INDICATOR_DATABASE" << "LOAD" << l.at(loop);

    command.parse(cl.join(","));

    plug = _factory.plugin(command.plugin());
    if (! plug)
    {
      qDebug() << "ChartLayout::load: no plugin" << command.plugin();
      continue;
    }

    if (plug->command(&command))
    {
      qDebug() << "ChartLayout::load: command failed";
      continue;
    }

    cl = command.stringData().split(",", QString::SkipEmptyParts);
    if (cl.count() != 5)
    {
      qDebug() << "ChartLayout::load: field # error" << cl.count();
      continue;
    }

    Indicator *i = new Indicator;
    i->setName(l.at(loop));
    i->setScript(cl.at(0));
    i->setCommand(cl.at(1));
    i->setTabRow(cl.at(2).toInt());
    i->setLog(cl.at(3).toInt());
    i->setDate(cl.at(4).toInt());
    g_indicators.insert(i->name(), i);
    addTab(i);
  }

  QHashIterator<QString, TabWidget *> it(_tabs);
  while (it.hasNext())
  {
    it.next();
    Plot *p = (Plot *) it.value()->currentWidget();
    if (p)
    {
      QString k = "chart_layout_" + it.key();
      it.value()->setCurrentIndex(settings.value(k).toInt());
    }
  }

  // set plot settings from config
  QString s = settings.value("background_color").toString();
  QColor color(s);
  emit signalBackgroundColor(color);

  l = settings.value("plot_font").toStringList();
  if (l.count())
  {
    QFont font;
    font.setFamily(l[0]);
    font.setPointSize(l[1].toInt());
    font.setWeight(l[2].toInt());
    font.setItalic(l[3].toInt());
    font.setBold(l[4].toInt());
    emit signalFont(font);
  }

  emit signalCrossHairs(settings.value("crosshairs").toInt());

  s = settings.value("crosshairs_color").toString();
  color.setNamedColor(s);
  emit signalCrossHairsColor(color);

  emit signalIndex(0);
}

void ChartLayout::addTab (QString &name)
{
  QStringList cl;
  cl << "INDICATOR_DATABASE" << "LOAD" << name;

  Command command(cl.join(","));

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ChartLayout::addTab: no plugin" << command.plugin();
    return;
  }

  if (plug->command(&command))
  {
    delete plug;
    return;
  }

  cl = command.stringData().split(",", QString::SkipEmptyParts);
  delete plug;
  if (cl.count() != 5)
  {
    qDebug() << "ChartLayout::addTab: field # error" << cl.count();
    return;
  }

  Indicator *i = new Indicator;
  i->setName(name);
  i->setScript(cl.at(0));
  i->setCommand(cl.at(1));
  i->setTabRow(cl.at(2).toInt());
  i->setLog(cl.at(3).toInt());
  i->setDate(cl.at(4).toInt());
  g_indicators.insert(name, i);
  addTab(i);
}

void ChartLayout::addTab (Indicator *i)
{
  QWidget *w = _holders.value(i->tabRow());
  if (! w)
    return;
  
  w->show();

  QString key = QString::number(i->tabRow());
  TabWidget *tab = _tabs.value(key);
  tab->show();

  PlotSettings settings;
  settings.plot = new Plot;

  settings.name = i->name();
  settings.plot->setIndicator(i->name());
  int pos = tab->addTab(settings.plot, QString());
  tab->setTabButton(pos, i->name());

  settings.row = i->tabRow();
  _plots.insert(i->name(), settings);

  settings.plot->showDate(i->date());
  settings.plot->setLogScaling(i->getLog());

  settings.plot->setBarSpacing(_barSpacing);

  connect(this, SIGNAL(signalBackgroundColor(QColor)), settings.plot, SLOT(setBackgroundColor(QColor)));
  connect(this, SIGNAL(signalFont(QFont)), settings.plot, SLOT(setFont(QFont)));
  connect(this, SIGNAL(signalGridColor(QColor)), settings.plot, SLOT(setGridColor(QColor)));
  connect(settings.plot, SIGNAL(signalInfoMessage(Setting)), this, SIGNAL(signalInfo(Setting)));
  connect(settings.plot, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(this, SIGNAL(signalBarSpacing(int)), settings.plot, SLOT(setBarSpacing(int)));
  connect(this, SIGNAL(signalClear()), settings.plot, SLOT(clear()));
  connect(this, SIGNAL(signalGrid(bool)), settings.plot, SLOT(setGrid(bool)));
  connect(this, SIGNAL(signalDraw()), settings.plot, SLOT(replot()));
  connect(this, SIGNAL(signalIndex(int)), settings.plot, SLOT(setStartIndex(int)));
  connect(settings.plot, SIGNAL(signalNewIndicator()), this, SLOT(newIndicator()));
//  connect(settings.plot, SIGNAL(signalEditIndicator(QString)), this, SLOT(editIndicator(QString)));
  connect(settings.plot, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(deleteIndicator()));
  connect(settings.plot, SIGNAL(signalBackgroundColorChanged(QColor)), this, SLOT(backgroundColorChanged(QColor)));
  connect(settings.plot, SIGNAL(signalFontChanged(QFont)), this, SLOT(fontChanged(QFont)));
  connect(this, SIGNAL(signalCrossHairs(bool)), settings.plot, SLOT(setCrossHairs(bool)));
  connect(this, SIGNAL(signalCrossHairsColor(QColor)), settings.plot, SLOT(setCrossHairsColor(QColor)));
  connect(this, SIGNAL(signalSaveSettings()), settings.plot, SLOT(clear()));
}

//******************************************************************
//************* DRAW FUNCTIONS ************************************
//*****************************************************************

void ChartLayout::loadPlots (int index)
{
  if (! g_barData->count())
    return;

  _startIndex = index;

  QStringList l;
  QHashIterator<QString, Indicator *> it(g_indicators);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
  }

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    if (! _plots.contains(l.at(loop)))
      continue;
    
    PlotSettings ps = _plots.value(l.at(loop));

    ps.plot->setDates();

    Indicator *i = g_indicators.value(l.at(loop));
    if (! i)
      continue;

    Script *script = new Script;
    script->setName(i->name());
    script->setFile(i->script());
    script->setCommand(i->command());
    connect(script, SIGNAL(signalDone(QString)), this, SLOT(indicatorScriptFinished(QString)));
    script->setBarData(g_barData);
    script->setIndicator(i);
    script->startScript();
  }
}

void ChartLayout::indicatorScriptFinished (QString name)
{
  Indicator *i = g_indicators.value(name);
  if (! i)
  {
    qDebug() << "ChartLayout::indicatorScriptFinished: no indicator for" << name;
    return;
  }

  // add plots to the plot
  if (! _plots.contains(name))
  {
    qDebug() << "ChartLayout::indicatorScriptFinished: no plot for" << name;
    return;
  }
  
  PlotSettings settings = _plots.value(name);

  settings.plot->clear();
  
  i->weedPlots();
  settings.plot->addCurves(i->curves());
  i->clearLines();
  
  settings.plot->loadChartObjects();
  settings.plot->setStartIndex(_startIndex);
}

void ChartLayout::setGridColor (QColor d)
{
  emit signalGridColor(d);
  emit signalDraw();
}

void ChartLayout::setCrossHairsColor (QColor d)
{
  emit signalCrossHairsColor(d);
  emit signalDraw();
}

void ChartLayout::setBarSpacing (int d)
{
  _barSpacing = d;
  emit signalBarSpacing(d);
  emit signalDraw();
}

void ChartLayout::setIndex (int d)
{
  emit signalIndex(d);
}

void ChartLayout::clearIndicator ()
{
  emit signalClear();
  emit signalDraw();
}

void ChartLayout::saveSettings ()
{
  emit signalSaveSettings();
}

void ChartLayout::setGrid (bool d)
{
  emit signalGrid(d);
  emit signalDraw();
}

void ChartLayout::setCrossHairs (bool d)
{
  emit signalCrossHairs(d);
  emit signalDraw();
}

void ChartLayout::setZoom (int pixelSpace, int index)
{
  emit signalBarSpacing(pixelSpace);
  emit signalIndex(index);
  emit signalDraw();
}

void ChartLayout::backgroundColorChanged (QColor c)
{
  QSettings settings;
  settings.beginGroup("main" + g_session);
  settings.setValue("background_color", c.name());
  settings.sync();
  
  emit signalBackgroundColor(c);
  emit signalDraw();
}

void ChartLayout::fontChanged (QFont f)
{
  QSettings settings;
  settings.beginGroup("main" + g_session);

  QStringList l;
  l << f.family() << QString::number(f.pointSize()) << QString::number(f.weight());
  l << QString::number(f.italic ()) << QString::number(f.bold ());
  settings.setValue("plot_font", l);
  settings.sync();

  emit signalFont(f);
  emit signalDraw();
}

int ChartLayout::plotWidth ()
{
  // find the best plot width for either < 1 column or > 1 column
  int maxWidth = 0;
  QHashIterator<QString, PlotSettings> it(_plots);
  while (it.hasNext())
  {
     it.next();
     PlotSettings settings = it.value();

     int width = settings.plot->width();
     
     if (width > maxWidth)
       maxWidth = width;
  }

  return maxWidth;
}

void ChartLayout::newIndicator ()
{
  Command command("INDICATOR_NEW_DIALOG");

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ChartLayout::newIndicator: no plugin" << command.plugin();
    return;
  }

  connect(plug, SIGNAL(signalDone(QStringList)), this, SLOT(newIndicator2(QStringList)));

  plug->command(&command);
}

void ChartLayout::newIndicator2 (QStringList l)
{
  if (l.count() != 6)
    return;

  Indicator *i = new Indicator;
  i->setName(l.at(0));
  i->setScript(l.at(1));
  i->setCommand(l.at(2));
  i->setTabRow(l.at(3).toInt());
  i->setLog(l.at(4).toInt());
  i->setDate(l.at(5).toInt());
  g_indicators.insert(i->name(), i);
  addTab(i->name());

  if (! g_barData->count())
    return;

  PlotSettings ps = _plots.value(i->name());

  ps.plot->setDates();

  Script *script = new Script;
  script->setIndicator(i);
  script->setBarData(g_barData);
  script->setName(i->name());
  script->setFile(i->script());
  script->setCommand(i->command());
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(indicatorScriptFinished(QString)), Qt::QueuedConnection);
  script->startScript();
}

/*
void ChartLayout::editIndicator (QString name)
{
  Indicator i;
  i.setName(name);
  
  IndicatorDataBase db;
  db.getIndicator(i);
  
  IndicatorPluginFactory fac;
  IndicatorPlugin *ip = fac.plugin(i.indicator());
  if (! ip)
  {
    qDebug() << "ChartLayout::editIndicator: plugin error" << i.indicator();
    return;
  }

  IndicatorPluginDialog *dialog = ip->dialog(i);
  if (! dialog)
  {
    qDebug() << "ChartLayout::editIndicator: no dialog";
    return;
  }

  connect(dialog, SIGNAL(signalDone(Indicator)), this, SLOT(editIndicator2(Indicator)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}
*/
/*
void ChartLayout::editIndicator2 (Indicator)
{
  if (! g_barData.count())
    return;

  qRegisterMetaType<Indicator>("Indicator");
  IndicatorThread *r = new IndicatorThread(this, i);
  connect(r, SIGNAL(signalDone(Indicator)), this, SLOT(indicatorThreadFinished(Indicator)), Qt::QueuedConnection);
  connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
  r->start();
}
*/

void ChartLayout::deleteIndicator ()
{
  Command command("INDICATOR_DELETE_DIALOG");

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ChartLayout::deleteIndicator: no plugin" << command.plugin();
    return;
  }

  connect(plug, SIGNAL(signalDone(QStringList)), this, SLOT(removeTab(QStringList)));

  plug->command(&command);
}

void ChartLayout::removeTab (QStringList l)
{
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    if (! _plots.contains(l.at(loop)))
      continue;

    PlotSettings settings = _plots.value(l.at(loop));
    int row = settings.row;

    QString key = QString::number(row);
    TabWidget *tab = _tabs.value(key);
    if (! tab)
      continue;

    int ti = tab->indexOf(settings.plot);
    if (ti == -1)
    {
      qDebug() << "ChartLayout::removeTab: tab not found";
      continue;
    }
    
    tab->removeTab(ti);

    delete settings.plot;
    _plots.remove(l.at(loop));

    if (! tab->count())
      tab->hide();

    int loop = 1;
    int flag = 0;
    for (; loop < 3; loop++)
    {
      key = QString::number(row);
      tab = _tabs.value(key);
      if (tab->count())
      {
        flag = 1;
        break;
      }
    }

    if (! flag)
    {
      QWidget *baseRow = _holders.value(row);
      baseRow->hide();
    }
  }
}

QHash<QString, PlotSettings> & ChartLayout::plots ()
{
  return _plots;
}
