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

#include "Plot.h"
#include "BarLength.h"
#include "PluginFactory.h"
#include "PlotStatus.h"
#include "Global.h"
//#include "DialogConfirm.h"

#include "../pics/delete.xpm"
#include "../pics/date.xpm"
#include "../pics/grid.xpm"
#include "../pics/edit.xpm"
#include "../pics/buyarrow.xpm"
#include "../pics/sellarrow.xpm"
#include "../pics/fib.xpm"
#include "../pics/horizontal.xpm"
#include "../pics/text.xpm"
#include "../pics/trend.xpm"
#include "../pics/vertical.xpm"
#include "../pics/about.xpm"

#include <QtDebug>
#include <QToolTip>
#include <QSettings>
#include <QUuid>
#include <QTextEdit>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>

Plot::Plot (QString name, QWidget *p) : QwtPlot (p)
{
  _ps.name = name;
  _ps.spacing = 8;
  _ps.high = 0;
  _ps.low = 0;
  _ps.startPos = -1;
  _ps.endPos = -1;
  _ps.selected = 0;
  _ps.antiAlias = TRUE;
  _ps.barLength = BarLength::_DAILY;
  _ps.status = PlotStatus::_NONE;
  _ps.info = TRUE;
  
  setMinimumHeight(20);

  setCanvasBackground(QColor(Qt::black));
  setMargin(0);
  enableAxis(QwtPlot::yRight, TRUE);
  enableAxis(QwtPlot::yLeft, FALSE);

  // add custom date scale drawing class to plot
  _dateScaleDraw = new PlotDateScaleDraw;
  setAxisScaleDraw(QwtPlot::xBottom, _dateScaleDraw);

  // add custom date scale drawing class to plot
  _plotScaleDraw = new PlotScaleDraw;
  setAxisScaleDraw(QwtPlot::yRight, _plotScaleDraw);

  _grid = new QwtPlotGrid;
//  _grid->enableXMin(FALSE);
  _grid->enableX(FALSE);
  _grid->enableYMin(FALSE);
  _grid->setMajPen(QPen(Qt::gray, 0, Qt::DotLine));
  _grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  _grid->setYAxis(QwtPlot::yRight);
  _grid->attach(this);

  // try to set the scale width to a sane size to keep plots aligned
  QwtScaleWidget *sw = axisWidget(QwtPlot::yRight);
  sw->scaleDraw()->setMinimumExtent(60);

  // setup the mouse events handler
  _picker = new PlotPicker(this);
  connect(_picker, SIGNAL(signalMouseMove(QPoint)), this, SLOT(mouseMove(QPoint)));
  connect(_picker, SIGNAL(signalMouseClick(int, QPoint)), this, SLOT(mouseClick(int, QPoint)));
  connect(_picker, SIGNAL(signalMouseDoubleClick(int, QPoint)), this, SLOT(mouseDoubleClick(int, QPoint)));
//  connect(_picker, SIGNAL(signalMouseRelease(int, QPoint)), this, SLOT(mouseRelease(int, QPoint)));

  // setup the context menu
  createMenu();
  
//  grabGesture(Qt::TapAndHoldGesture);
//  grabGesture(Qt::SwipeGesture);

  _plotInfo = new PlotInfo;
  _plotInfo->attach(this);
}

Plot::~Plot ()
{
  clear();
}

void
Plot::clear ()
{
  if (_ps.curves.size())
    qDeleteAll(_ps.curves);
  _ps.curves.clear();

  if (_ps.markers.size())
    qDeleteAll(_ps.markers);
  _ps.markers.clear();

  _ps.selected = 0;

  _dateScaleDraw->clear();

  QwtPlot::clear();
}
 
void
Plot::resizeEvent (QResizeEvent *event)
{
  emit signalResizeScrollBar(this);
  QwtPlot::resizeEvent(event);
}

void
Plot::setDates ()
{
  _dateScaleDraw->setDates();
}

void
Plot::updatePlot ()
{
//  setDates();

  // calculate end of chart pos
  _ps.endPos = g_symbol->bars();
  int page = width() / _ps.spacing;
  _ps.startPos = _ps.endPos - page;
  if (_ps.startPos < 0)
    _ps.startPos = 0;

  setStartIndex(_ps.startPos);

//  emit signalIndex(_ps.startPos);
}

void
Plot::setCurve (Curve *curve)
{
  curve->itemChanged();
  curve->setYAxis(QwtPlot::yRight);
  curve->attach(this);
  _ps.curves.insert(curve->label(), curve);
}

void
Plot::setBackgroundColor (QColor d)
{
  setCanvasBackground(d);
  replot();
}

void
Plot::setGridColor (QColor d)
{
  _grid->setMajPen(QPen(d, 0, Qt::DotLine));
  replot();
}

void
Plot::setCrossHairsColor (QColor d)
{
  _picker->setColor(d);
  replot();
}

void
Plot::setGrid (bool d)
{
//  _grid->enableX(d);
  _grid->enableY(d);
  replot();
}

void
Plot::setInfo (bool d)
{
  _ps.info = d;
  
  if (d == FALSE)
  {
    _plotInfo->setData(QStringList());
    replot();
  }
}

void
Plot::setFont (QFont d)
{
  setAxisFont(QwtPlot::yRight, d);
  setAxisFont(QwtPlot::xBottom, d);
  replot();
}

void
Plot::setLogScaling (bool d)
{
  if (d == TRUE)
    setAxisScaleEngine(QwtPlot::yRight, new QwtLog10ScaleEngine);
  else
    setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine);

//  replot();
}

void
Plot::showDate (bool d)
{
  enableAxis(QwtPlot::xBottom, d);
}

void
Plot::setCrossHairs (bool d)
{
  _picker->setCrossHairs(d);
  replot();
}

void
Plot::setBarSpacing (int d)
{
  _ps.spacing = d;
  setStartIndex(_ps.startPos);
}

void
Plot::setHighLow ()
{
  _ps.high = 0;
  _ps.low = 0;
  bool flag = FALSE;

  QHashIterator<QString, Curve *> it(_ps.curves);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    double h, l;
    if (! curve->highLowRange(_ps.startPos, _ps.endPos, h, l))
      continue;

    if (! flag)
    {
      _ps.high = h;
      _ps.low = l;
      flag = TRUE;
    }
    else
    {
      if (h > _ps.high)
        _ps.high = h;

      if (l < _ps.low)
        _ps.low = l;
    }
  }

  QHashIterator<QString, Marker *> it2(_ps.markers);
  while (it2.hasNext())
  {
    it2.next();
    Marker *co = it2.value();
    
    double h, l;
    if (! co->highLow(_ps.startPos, _ps.endPos, h, l))
      continue;

    if (! flag)
    {
      _ps.high = h;
      _ps.low = l;
      flag = TRUE;
    }
    else
    {
      if (h > _ps.high)
        _ps.high = h;

      if (l < _ps.low)
        _ps.low = l;
    }
  }

  setAxisScale(QwtPlot::yRight, _ps.low, _ps.high);
}

void
Plot::setStartIndex (int index)
{
  int dstart, dend;
  if (! _dateScaleDraw->startEndRange(dstart, dend))
    return;
  
  _ps.startPos = index;
  int page = width() / _ps.spacing;
  _ps.endPos = _ps.startPos + page;
  if (_ps.endPos > dend)
    _ps.endPos = dend;

//qDebug() << "Plot::setStartIndex" << _startPos << _endPos;

  setHighLow();

  setAxisScale(QwtPlot::xBottom, _ps.startPos, _ps.endPos);

  setYPoints();

  replot();
}

void
Plot::setYPoints ()
{
  _plotScaleDraw->clearPoints();

  QHashIterator<QString, Curve *> it(_ps.curves);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();

    int min = 0;
    int max = 0;
    g_symbol->startEndRange(min, max);
    int index = _ps.endPos;
    int flag = 0;
    if (index > max)
    {
      index = max;
      flag++;
    }

    QColor color(Qt::red);
    double val = 0;
    if (! curve->scalePoint(index, color, val))
      continue;

    _plotScaleDraw->addPoint(color, val);
  }
}

void
Plot::showContextMenu ()
{
  if (_ps.selected)
    return;

  _menu->exec(QCursor::pos());
}

int
Plot::index ()
{
  return _ps.startPos;
}

void
Plot::mouseClick (int button, QPoint p)
{
  if (! g_symbol)
    return;

  if (_ps.selected)
  {
    _ps.selected->click(_ps.status, button, p);
    return;
  }
    
  QHashIterator<QString, Marker *> it(_ps.markers);
  while (it.hasNext())
  {
    it.next();
    Marker *co = it.value();
    
    if (co->readOnly())
      continue;

    if (! co->isSelected(p))
      continue;

    _ps.selected = co;
    _ps.selected->click(_ps.status, button, p);
    return;
  }
}

void
Plot::mouseDoubleClick (int, QPoint)
{
/*
  if (! _dateScaleDraw->count())
    return;

  if (_chartObjectDialog)
    return;

  if (_selected)
    return;

  int pos = (int) invTransform(QwtPlot::xBottom, p.x());
  int page = width() / _spacing;
  int center = _startPos + (page / 2);
//  if (pos > center)
//    nextPage();
//  else
//  {
//    if (pos < center)
//      previousPage();
//  }
//qDebug() << "Plot::mouseDoubleClick:" << pos;
*/
}

void
Plot::mouseMove (QPoint p)
{
  if (! _ps.info)
    return;
  
  if (! g_symbol)
    return;

  if (_ps.selected)
  {
    _ps.selected->move(_ps.status, p);
    return;
  }

  QHashIterator<QString, Marker *> it2(_ps.markers);
  while (it2.hasNext())
  {
    it2.next();
    Marker *m = it2.value();
    
    if (! m->isSelected(p))
      continue;

    QStringList info;
    info << _ps.name;
    
    if (! m->info(info))
      continue;

    _plotInfo->setData(info);
    replot();

    return;
  }

  int index = (int) invTransform(QwtPlot::xBottom, p.x());
  
  QStringList info;
  info << _ps.name;

  _dateScaleDraw->info(index, info);

  QHashIterator<QString, Curve *> it(_ps.curves);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    curve->info(index, info);
  }

  _plotInfo->setData(info);
  
  replot();
}

QMenu *
Plot::menu ()
{
  return _menu;
}

void
Plot::setName (QString d)
{
  _ps.name = d;
}

QString
Plot::name ()
{
  return _ps.name;
}

void
Plot::scrollBarSize (int &page, int &max)
{
  page = width() / _ps.spacing;
  max = g_symbol->bars() - page - 1;
  if (max < 0)
  {
    max = 0;
    page = 0;
  }
}

void
Plot::setBarLength (int d)
{
  _ps.barLength = d;
}

bool
Plot::date ()
{
  return _dateAction->isChecked();
}

bool
Plot::grid ()
{
  return _gridAction->isChecked();
}

bool
Plot::info ()
{
  return _infoAction->isChecked();
}

void
Plot::createMenu ()
{
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu()));

  _menu = new QMenu(this);
  
  _dateAction = new QAction(QIcon(date_xpm), tr("&Date"), this);
  _dateAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
  _dateAction->setToolTip(tr("Date"));
  _dateAction->setStatusTip(tr("Date"));
  _dateAction->setCheckable(TRUE);
  _dateAction->setChecked(TRUE);
  connect(_dateAction, SIGNAL(triggered(bool)), this, SLOT(showDate(bool)));
  _menu->addAction(_dateAction);

  _gridAction = new QAction(QIcon(grid_xpm), tr("&Grid"), this);
  _gridAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
  _gridAction->setToolTip(tr("Grid"));
  _gridAction->setStatusTip(tr("Grid"));
  _gridAction->setCheckable(TRUE);
  _gridAction->setChecked(TRUE);
  connect(_gridAction, SIGNAL(triggered(bool)), this, SLOT(setGrid(bool)));
  _menu->addAction(_gridAction);

  _infoAction = new QAction(QIcon(about_xpm), tr("Bar &Info"), this);
  _infoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
  _infoAction->setToolTip(tr("Bar Info"));
  _infoAction->setStatusTip(tr("Bar Info"));
  _infoAction->setCheckable(TRUE);
  _infoAction->setChecked(TRUE);
  connect(_infoAction, SIGNAL(triggered(bool)), this, SLOT(setInfo(bool)));
  _menu->addAction(_infoAction);

  _menu->addSeparator();
  
  // create the chart object menu
  QMenu *mm = new QMenu(this);
  mm->setTitle(tr("New Plot Marker..."));
  connect(mm, SIGNAL(triggered(QAction *)), this, SLOT(markerMenuSelected(QAction *)));
  _menu->addMenu(mm);
  
  // buy
  QAction *a = new QAction(QIcon(buyarrow_xpm), tr("&Buy"), this);
  a->setToolTip(tr("Create Buy Arrow Plot Marker"));
  a->setStatusTip(QString(tr("Create Buy Arrow Plot Marker")));
  a->setData(QVariant("MarkerBuy"));
  mm->addAction(a);

  // hline
  a = new QAction(QIcon(horizontal_xpm), tr("&HLine"), this);
  a->setToolTip(tr("Create Horizontal Line Plot Marker"));
  a->setStatusTip(QString(tr("Create Horizontal Line Plot Marker")));
  a->setData(QVariant("MarkerHLine"));
  mm->addAction(a);

  // retracement
  a = new QAction(QIcon(fib_xpm), tr("&Retracement"), this);
  a->setToolTip(tr("Create Retracement Levels Plot Marker"));
  a->setStatusTip(QString(tr("Create Retracement Levels Plot Marker")));
  a->setData(QVariant("MarkerRetracement"));
  mm->addAction(a);

  // sell
  a = new QAction(QIcon(sellarrow_xpm), tr("&Sell"), this);
  a->setToolTip(tr("Create Sell Arrow Plot Marker"));
  a->setStatusTip(QString(tr("Create Sell Arrow Plot Marker")));
  a->setData(QVariant("MarkerSell"));
  mm->addAction(a);

  // text
  a = new QAction(QIcon(text_xpm), tr("&Text"), this);
  a->setToolTip(tr("Create Text Plot Marker"));
  a->setStatusTip(QString(tr("Create Text Plot Marker")));
  a->setData(QVariant("MarkerText"));
  mm->addAction(a);

  // tline
  a = new QAction(QIcon(trend_xpm), tr("T&Line"), this);
  a->setToolTip(tr("Create Trend Line Plot Marker"));
  a->setStatusTip(QString(tr("Create Trend Line Plot Marker")));
  a->setData(QVariant("MarkerTLine"));
  mm->addAction(a);

  // vline
  a = new QAction(QIcon(vertical_xpm), tr("&VLine"), this);
  a->setToolTip(tr("Create Vertical Line Plot Marker"));
  a->setStatusTip(QString(tr("Create Vertical Line Plot Marker")));
  a->setData(QVariant("MarkerVLine"));
  mm->addAction(a);
  
  // delete all chart objects
  a = new QAction(QIcon(delete_xpm), tr("Delete All Plot &Markers"), this);
  a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
  a->setToolTip(tr("Delete All Plot Markers"));
  a->setStatusTip(tr("Delete All Plot Markers"));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(deleteAllMarkersDialog()));
  _menu->addAction(a);

  // marker menu
  _markerMenu = new QMenu(this);
  _markerMenu->addAction(QIcon(edit_xpm), QObject::tr("&Edit"), this, SLOT(markerDialog()), QKeySequence(Qt::ALT+Qt::Key_E));
  _markerMenu->addAction(QIcon(delete_xpm), QObject::tr("&Delete"), this, SLOT(deleteMarker()), QKeySequence(Qt::ALT+Qt::Key_D));
}

//********************************************************************
//***************** CHART OBJECT FUNCTIONS ***************************
//********************************************************************

void
Plot::markerMenuSelected (QAction *a)
{
  newMarker(a->data().toString());
}

void
Plot::newMarker (QString plugin)
{
  QString id = QUuid::createUuid().toString();
  id.remove("{");
  id.remove("}");
  id.remove("-");
  
  _ps.selected = new Marker(plugin);
  
  Entity *e = _ps.selected->settings();
  
  QVariant *tset = e->get(QString("symbol"));
  tset->setValue(g_symbol->symbol());
  
  tset = e->get(QString("plot"));
  tset->setValue(_ps.name);
  
  _ps.selected->setID(id);
  _ps.markers.insert(id, _ps.selected);
  
  _ps.selected->attach(this);
  _ps.selected->setModified(TRUE);
  _ps.selected->create(_ps.status);
}

void
Plot::deleteAllMarkersDialog ()
{
  if (! _ps.markers.size())
    return;
  
//  DialogConfirm *dialog = new DialogConfirm(this);
//  dialog->setMessage(tr("Confirm delete all plot markers"));
//  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteAllMarkers()));
//  dialog->show();
}

void
Plot::deleteAllMarkers ()
{
  QStringList dl;
  QHashIterator<QString, Marker *> it(_ps.markers);
  while (it.hasNext())
  {
    it.next();
    Marker *co = it.value();

    if (co->readOnly())
      continue;

    delete co;
    
    dl << it.key();
    
    _ps.markers.remove(it.key());
  }
  
  emit signalDeleteMarkers(dl);

  setHighLow();
  replot();
}

void
Plot::setMarker (Marker *m)
{
  _ps.markers.insert(m->ID(), m);
  m->attach(this);
}

void
Plot::showMarkerMenu ()
{
  if (! _ps.selected)
    return;

  _markerMenu->exec(QCursor::pos());
}

void
Plot::markerDialog ()
{
  if (! _ps.selected)
    return;

  Entity *e = _ps.selected->settings();
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

  connect(pd.dialog, SIGNAL(accepted()), this, SLOT(markerDialog2()));
  pd.dialog->show();
}

void Plot::markerDialog2 ()
{
  if (! _ps.selected)
    return;

  Entity *e = _ps.selected->settings();
  if (! e)
    return;
  
  _ps.selected->setModified(TRUE);

  setHighLow();
  replot();
}

void
Plot::deleteMarker ()
{
  if (! _ps.markers.size())
    return;
  
//  DialogConfirm *dialog = new DialogConfirm(this);
//  dialog->setMessage(tr("Confirm delete selected plot marker"));
//  connect(dialog, SIGNAL(accepted()), this, SLOT(deleteMarker2()));
//  dialog->show();
}


void Plot::deleteMarker2 ()
{
  if (! _ps.selected)
    return;

  QString id = _ps.selected->ID();
  _ps.markers.remove(id);
  delete _ps.selected;
  _ps.selected = 0;

  setHighLow();
  replot();
  
  emit signalDeleteMarkers(QStringList() << id);
}

void
Plot::unselectMarker ()
{
  _ps.selected = 0;
}

void
Plot::selectMarker (QString id)
{
  _ps.selected = _ps.markers.value(id);
}

QHash<QString, Marker *> 
Plot::markers ()
{
  return _ps.markers;
}
