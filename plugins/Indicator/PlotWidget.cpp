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

#include "PlotWidget.h"
#include "PluginFactory.h"
#include "IndicatorDialog.h"
#include "Global.h"
#include "BarType.h"
#include "PluginData.h"

#include <QtDebug>
#include <QLayout>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QTimer>
#include <QTextEdit>
#include <QInputDialog>


PlotWidget::PlotWidget ()
{
  g_symbol = new Bars;
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(0);
  vbox->setMargin(0);
  setLayout(vbox);
  
  // chart splitter
  _csplitter = new QSplitter(Qt::Vertical, 0);
  _csplitter->setOpaqueResize(FALSE);
  vbox->addWidget(_csplitter);
  
  _cw = new ControlWidget;
  connect(_cw, SIGNAL(signalSelected()), this, SLOT(refresh()));
  connect(_cw, SIGNAL(signalRefresh()), this, SLOT(refresh()));
  connect(_cw, SIGNAL(signalIndicator()), this, SLOT(indicatorDialog()));
  connect(_cw, SIGNAL(signalLength()), this, SLOT(setBarLength()));
  connect(_cw, SIGNAL(signalRange()), this, SLOT(refresh()));
  connect(_cw, SIGNAL(signalScrollBarChanged(int)), this, SLOT(scrollBarChanged(int)));
  connect(_cw, SIGNAL(signalRemovePlot()), this, SLOT(removeIndicator()));
  connect(_cw, SIGNAL(signalEditPlot()), this, SLOT(editIndicator()));
  vbox->addWidget(_cw);
  
  // fix for messed up plot screen if we draw the plot before it has become visible
  // we load settings just after plot is visible with this delay
  QTimer::singleShot(10, this, SLOT(loadSettings()));
}

PlotWidget::~PlotWidget ()
{
  saveSettings();
  qDeleteAll(_plots);
  qDeleteAll(_settings);
  delete g_symbol;
}

void
PlotWidget::addPlot (QString plugin, int row, QString name)
{
  PluginFactory fac;
  Plugin *plug = fac.load(plugin);
  if (! plug)
    return;

  PluginData pd;
  pd.command = QString("settings");
  if (! plug->command(&pd))
    return;
  
  Entity *settings = pd.settings;
  settings->setName(name);
  
  _settings.insert(name, settings);
  
  addPlotSettings(settings);

  QVariant *tset = settings->get(QString("row"));
  if (tset)
    tset->setValue(row);

  Plot *plot = new Plot(name, 0);
  plot->setMinimumHeight(30);
  _plots.insert(name, plot);
  
  _csplitter->insertWidget(row, plot);

  plot->setStartIndex(_cw->scrollBarValue());
  
  // connect all we need here
  connect(plot, SIGNAL(signalResizeScrollBar(Plot *)), _cw, SLOT(resizeScrollBar(Plot *)));
  connect(this, SIGNAL(signalClear()), plot, SLOT(clear()));
  connect(this, SIGNAL(signalDraw()), plot, SLOT(updatePlot()));
  connect(this, SIGNAL(signalSetDates()), plot, SLOT(setDates()));
  connect(this, SIGNAL(signalBarLength(int)), plot, SLOT(setBarLength(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setStartIndex(int)));
  connect(plot, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(plot, SIGNAL(signalDeleteMarkers(QStringList)), this, SLOT(deleteMarkers(QStringList)));
}

void
PlotWidget::removePlot (QString name)
{
  Plot *plot = _plots.value(name);
  if (! plot)
    return;
  
  Entity *e = _settings.value(name);
  if (! e)
    return;
  
  // remove markers
  DataBase db(g_session);
  db.transaction();
  
  QHash<QString, Marker *> markers = plot->markers();
  QList<QString> ml = markers.keys();
  db.remove(ml);
  
  // remove indicator settings
  db.remove(QStringList() << name);
  
  db.commit();
  
  delete plot;
  delete e;
  _plots.remove(name);
  _settings.remove(name);
}

void
PlotWidget::scrollBarChanged (int d)
{
  emit signalIndex(d);
}

void
PlotWidget::setScrollBarSize ()
{
  bool flag = FALSE;
  int page = 0;
  int max = 0;
  QHashIterator<QString, Plot *> it(_plots);
  while (it.hasNext())
  {
    it.next();
    Plot *plot = it.value();
    
    int tpage, tmax;
    plot->scrollBarSize(tpage, tmax);
    
    if (! flag)
    {
      page = tpage;
      max = tmax;
      flag = TRUE;
    }
    else
    {
      if (tpage > page)
        page = tpage;
      if (tmax > max)
        max = tmax;
    }
  }

  _cw->setScrollBar(0, max, page);
}

void
PlotWidget::indicatorDialog ()
{
  IndicatorDialog *dialog = new IndicatorDialog(this);
  connect(dialog, SIGNAL(signalDone(QString, int, QString)), this, SLOT(indicatorDialog2(QString, int, QString)));
  dialog->show();
}

void
PlotWidget::indicatorDialog2 (QString plugin, int row, QString name)
{
  addPlot(plugin, row, name);
  refresh();
}

void
PlotWidget::refresh ()
{
  if (! _plots.size())
    return;
  
  // load fresh symbol data
  if (! _cw->count())
    return;
  
  PluginFactory fac;
  Plugin *qplug = fac.load(QString("DBSymbol"));
  if (! qplug)
    return;

  DataBase db(g_session);
  
  saveMarkers(db);
  
  Bars sym = _cw->currentSymbol();
  g_symbol->clear();
  g_symbol->setSymbol(sym.symbol());
  g_symbol->setLength(_cw->length());
  g_symbol->setRange(_cw->range());

  PluginData pd;
  pd.command = QString("getBars");
  pd.bars = g_symbol;
  
  if (! qplug->command(&pd))
    return;
  else
    emit signalClear();

  // refresh dates
  emit signalSetDates();
  
  QHashIterator<QString, Plot *> it(_plots);
  while (it.hasNext())
  {
    it.next();
    Plot *plot = it.value();
    Entity *e = _settings.value(it.key());
    if (! e)
      continue;

    QVariant *plugin = e->get(QString("plugin"));
    if (! plugin)
      continue;
    
    Plugin *iplug = fac.load(plugin->toString());
    if (! iplug)
      continue;

    PluginData tpd;
    tpd.command = QString("runIndicator");
    tpd.settings = e;
    
    if (! iplug->command(&tpd))
      continue;

    for (int tpos = 0; tpos < tpd.curves.size(); tpos++)
      plot->setCurve(tpd.curves.at(tpos));

    for (int tpos = 0; tpos < tpd.markers.size(); tpos++)
      plot->setMarker(tpd.markers.at(tpos));
  }

  loadMarkers(db);
  
  emit signalDraw();
  
  setScrollBarSize();
  
  QStringList tl;
  tl << "OTA" << "-" << sym.symbol() << "(" + sym.name() + ")" << _cw->lengthText() << _cw->rangeText();
  emit signalTitle(tl.join(" "));
}

void
PlotWidget::editIndicator ()
{
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setWindowTitle(tr("Edit Indicator"));
  dialog->setLabelText(tr("Indicator"));
  dialog->setComboBoxItems(_plots.keys());
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(editIndicator2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void
PlotWidget::editIndicator2 (QString name)
{
  Entity *e = _settings.value(name);
  if (! e)
    return;

  QVariant *plugin = e->get(QString("plugin"));
  if (! plugin)
    return;
  
  PluginFactory fac;
  Plugin *plug = fac.load(plugin->toString());
  if (! plug)
    return;

  PluginData pd;
  pd.command = QString("dialog");
  pd.dialogParent = this;
  pd.settings = e;
  if (! plug->command(&pd))
    return;
  
  connect(pd.dialog, SIGNAL(accepted()), this, SLOT(refresh()));
  pd.dialog->show();
}

void
PlotWidget::removeIndicator ()
{
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setWindowTitle(tr("Remove Indicator"));
  dialog->setLabelText(tr("Indicator"));
  dialog->setComboBoxItems(_plots.keys());
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(removeIndicator2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void
PlotWidget::removeIndicator2 (QString name)
{
  QStringList tl;
  tl << tr("Remove") << name;
  
  QMessageBox msgBox;
  msgBox.setText(tl.join(" "));
  msgBox.setInformativeText("Do you want to remove this indicator?");
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Ok);
  int ret = msgBox.exec();
  
  if (ret == QMessageBox::Cancel)
    return;
  
  removePlot(name);
}

void
PlotWidget::setBarLength ()
{
  emit signalBarLength(_cw->length());
  refresh();
}

void
PlotWidget::loadSettings ()
{
  DataBase db(g_session);
  
  // load controlWidget settings
  _cw->loadSettings(&db);
  
  // load indicator/plot settings
  QStringList l;
  db.getTypes(QString("indicator"), l);
  for (int pos = 0; pos < l.size(); pos++)
  {
    // get the plugin, row first
    Entity te;
    te.setName(l.at(pos));
    te.set(QString("plugin"), new QVariant(QString()));
    te.set(QString("row"), new QVariant(0));
    
    if (! db.get(&te))
      continue;
    
    QVariant *plugin = te.get(QString("plugin"));
    QVariant *row = te.get(QString("row"));
    addPlot(plugin->toString(), row->toInt(), l.at(pos));
    
    Entity *is = _settings.value(l.at(pos));
    if (! is)
      continue;
    
    if (! db.get(is))
      continue;

    // load plot settings
    Plot *plot = _plots.value(l.at(pos));
    if (! plot)
      continue;

    QVariant *tset = is->get(QString("date"));
    if (tset)
      plot->showDate(tset->toBool());

    tset = is->get(QString("grid"));
    if (tset)
      plot->setGrid(tset->toBool());

    tset = is->get(QString("info"));
    if (tset)
      plot->setInfo(tset->toBool());
  }

  // load this settings
  Entity *e = settings();
  if (! db.get(e))
  {
    delete e;
    return;
  }
  
  QVariant *tset = e->get(QString("sizes"));
  if (! tset)
    return;
  
  l = tset->toStringList();
  QList<int> sizes;
  for (int pos = 0; pos < l.size(); pos++)
    sizes << l.at(pos).toInt();
  _csplitter->setSizes(sizes);
  
  refresh();
}

void
PlotWidget::saveSettings ()
{
  DataBase db(g_session);
  db.transaction();

  // save controlWidget settings
  _cw->saveSettings(&db);

  // save this settings
  Entity *e = settings();
  db.set(e);
  delete e;
  
  QHashIterator<QString, Entity *> it(_settings);
  while (it.hasNext())
  {
    it.next();
    Entity *e = it.value();
    
    Plot *p = _plots.value(it.key());
    if (! p)
      continue;
    
    QVariant *tset = e->get(QString("date"));
    if (tset)
      tset->setValue(p->date());

    tset = e->get(QString("grid"));
    if (tset)
      tset->setValue(p->grid());

    tset = e->get(QString("info"));
    if (tset)
      tset->setValue(p->info());
    
    // save indicator settings
    db.set(e);
  }

  // save markers
  saveMarkers(db);
  
  db.commit();
}

Entity *
PlotWidget::settings ()
{
  Entity *e = new Entity;
  e->setName(QString("plotSplitter"));

  QStringList l;
  QList<int> sizes = _csplitter->sizes();
  for (int pos = 0; pos < sizes.size(); pos++)
    l << QString::number(sizes.at(pos));
  
  e->set(QString("sizes"), new QVariant(l));

  return e;
}

void
PlotWidget::saveMarkers (DataBase &db)
{
  // save plot markers
  QHashIterator<QString, Plot *> pit(_plots);
  while (pit.hasNext())
  {
    pit.next();
    Plot *p = pit.value();
    
    QHash<QString, Marker *> markers = p->markers();
    QHashIterator<QString, Marker *> mit(markers);
    while (mit.hasNext())
    {
      mit.next();
      Marker *m = mit.value();
    
      if (m->readOnly())
        continue;
      
      if (! m->modified())
        continue;
      
      Entity *e = m->settings();
      e->setName(mit.key());
      
      db.transaction();
      db.set(e);
      db.commit();
      
      m->setModified(FALSE);
    }
  }
}

void
PlotWidget::loadMarkers (DataBase &db)
{
  // load marker names
  QStringList names;
  db.getTypes(QString("marker"), names);
  
  for (int pos = 0; pos < names.size(); pos++)
  {
    Entity te;
    te.setName(names.at(pos));
    te.set(QString("plugin"), new QVariant(QString()));
    te.set(QString("plot"), new QVariant(QString()));
    te.set(QString("symbol"), new QVariant(QString()));
    
    if (! db.get(&te))
      continue;

    QVariant *tset = te.get(QString("symbol"));
    if (tset->toString() != g_symbol->symbol())
      continue;
    
    tset = te.get(QString("plot"));
    Plot *p = _plots.value(tset->toString());
    if (! p)
    {
      qDebug() << "PlotWidget::loadMarkers: plot not found" << tset->toString();
      continue;
    }
    
    tset = te.get(QString("plugin"));
    
    Marker *m = new Marker(tset->toString());
    m->setID(names.at(pos));
    
    Entity *e = m->settings();
    
    e->setName(names.at(pos));
    db.get(e);

    p->setMarker(m);
  }
}

void
PlotWidget::deleteMarkers (QStringList l)
{
  DataBase db(g_session);
  db.transaction();
  db.remove(l);
  db.commit();
}

void
PlotWidget::addPlotSettings (Entity *e)
{
  e->set(QString("date"), new QVariant(TRUE));
  e->set(QString("grid"), new QVariant(TRUE));
  e->set(QString("info"), new QVariant(TRUE));
  e->set(QString("row"), new QVariant(0));
}
