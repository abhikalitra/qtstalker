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

#include <QtGui>

#include "ta_libc.h"
#include "SAR.h"
#include "CurveLineType.h"
#include "CurveOHLCType.h"
#include "Global.h"
#include "SARDialog.h"
#include "BarType.h"


int
SAR::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "dialog" << "runIndicator" << "settings";
  
  switch (cl.indexOf(pd->command))
  {
    case 0: // type
      pd->type = QString("indicator");
      rc = 1;
      break;
    case 1: // dialog
      rc = dialog(pd);
      break;
    case 2: // runIndicator
      rc = run(pd);
      break;
    case 3: // settings
      rc = settings(pd);
      break;
    default:
      break;
  }
  
  return rc;
}

int
SAR::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "SAR::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "SAR::dialog: invalid settings";
    return 0;
  }
  
  SARDialog *dialog = new SARDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
SAR::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  QVariant *var = pd->settings->get(QString("upColor"));
  if (! var)
    return 0;
  QColor uc(var->toString());
  
  var = pd->settings->get(QString("downColor"));
  if (! var)
    return 0;
  QColor dc(var->toString());
  
  var = pd->settings->get(QString("neutralColor"));
  if (! var)
    return 0;
  QColor nc(var->toString());
  
  QVariant *label = pd->settings->get(QString("label"));
  if (! label)
    return 0;
  
  QVariant *style = pd->settings->get(QString("style"));
  if (! style)
    return 0;
  
  Curve *ohlc = getOHLC(style->toString(), label->toString(), uc, dc, nc);
  if (! ohlc)
    return 0;
  pd->curves << ohlc;
  
  // sar
  QVariant *init = pd->settings->get(QString("init"));
  if (! init)
    return 0;
  
  QVariant *max = pd->settings->get(QString("max"));
  if (! max)
    return 0;
  
  var = pd->settings->get(QString("sarColor"));
  if (! var)
    return 0;
  QColor color(var->toString());
  
  label = pd->settings->get(QString("sarLabel"));
  if (! label)
    return 0;
  
  if (! getSAR(init->toDouble(), max->toDouble(), label->toString()))
    return 0;
  
  Curve *line = new Curve(QString("CurveLine"));
  line->setColor(color);
  line->setLabel(label->toString());
  line->setStyle(CurveLineType::_DOT);
  line->fill(label->toString(), QString(), QString(), QString(), QColor());
  pd->curves << line;
  
  return 1;
}

Curve *
SAR::getOHLC (QString tstyle, QString key, QColor uc, QColor dc, QColor nc)
{
  if (! g_symbol)
    return 0;
  
  CurveOHLCType ohlcType;
  Curve *ohlc = new Curve(QString("CurveOHLC"));
  ohlc->setLabel(key);
  ohlc->setStyle(ohlcType.stringToIndex(tstyle));
  
  BarType bt;
  ohlc->fill(bt.indexToString(BarType::_OPEN),
             bt.indexToString(BarType::_HIGH),
             bt.indexToString(BarType::_LOW),
             bt.indexToString(BarType::_CLOSE),
             nc);
  
  QList<int> keys = g_symbol->keys();

  for (int pos = 1; pos < keys.size(); pos++)
  {
    Bar *pbar = ohlc->bar(keys.at(pos - 1));
    Bar *bar = ohlc->bar(keys.at(pos));
    
    if (bar->close() > pbar->close())
      bar->setColor(uc);
    else
    {
      if (bar->close() < pbar->close())
        bar->setColor(dc);
    }
  }  
  
  return ohlc;
}

int
SAR::getSAR (double init, double max, QString okey)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "SAR::SAR: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  BarType bt;
  QString highKey = bt.indexToString(BarType::_HIGH);
  QString lowKey = bt.indexToString(BarType::_LOW);
  
  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double h;
    if (! bar->get(highKey, h))
      continue;
    
    double l;
    if (! bar->get(lowKey, l))
      continue;
    
    high[dpos] = (TA_Real) h;
    low[dpos] = (TA_Real) l;
    dpos++;
  }

  rc = TA_SAR(0, dpos - 1, &high[0], &low[0], init, max, &outBeg, &outNb, &out[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "SAR::getSAR: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    bar->set(okey, out[outLoop]);
    keyLoop--;
    outLoop--;
  }
  
  return 1;
}

int
SAR::settings (PluginData *pd)
{
  Entity *command = new Entity;
  CurveOHLCType ct;

  command->set(QString("plugin"), new QVariant(QString("SAR")));
  command->set(QString("type"), new QVariant(QString("indicator")));

  command->set(QString("style"), new QVariant(ct.indexToString(CurveOHLCType::_OHLC)));
  command->set(QString("upColor"), new QVariant(QString("green")));
  command->set(QString("downColor"), new QVariant(QString("red")));
  command->set(QString("neutralColor"), new QVariant(QString("blue")));
  command->set(QString("label"), new QVariant(QString("OHLC")));

  command->set(QString("init"), new QVariant(0.02));
  command->set(QString("max"), new QVariant(0.2));
  command->set(QString("sarColor"), new QVariant(QString("yellow")));
  command->set(QString("sarLabel"), new QVariant(QString("SAR")));
  
  pd->settings = command;
  
  return 1;
}

int
SAR::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(sar, SAR);
