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
#include "ROC.h"
#include "MAType.h"
#include "BarType.h"
#include "CurveLineType.h"
#include "CurveHistogramType.h"
#include "Global.h"
#include "ROCDialog.h"
#include "ROCType.h"


int
ROC::command (PluginData *pd)
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
ROC::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "ROC::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "ROC::dialog: invalid settings";
    return 0;
  }
  
  ROCDialog *dialog = new ROCDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
ROC::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  QVariant *input = pd->settings->get(QString("input"));
  if (! input)
    return 0;
  
  QVariant *var = pd->settings->get(QString("method"));
  if (! var)
    return 0;
  ROCType rt;
  int method = rt.stringToIndex(var->toString());
  if (method == -1)
    return 0;
  
  QVariant *period = pd->settings->get(QString("period"));
  if (! period)
    return 0;
  
  QVariant *label = pd->settings->get(QString("label"));
  if (! label)
    return 0;
  
  if (! getROC(input->toString(), method, period->toInt(), label->toString()))
    return 0;
  
  // roc
  QVariant *show = pd->settings->get(QString("rocShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("upColor"));
    if (! var)
      return 0;
    QColor uc(var->toString());
  
    var = pd->settings->get(QString("downColor"));
    if (! var)
      return 0;
    QColor dc(var->toString());

    Curve *hl = new Curve(QString("CurveHistogram"));
    hl->setColor(uc);
    hl->setLabel(label->toString());
    hl->setStyle(CurveHistogramType::_BAR);
    hl->fill(label->toString(), QString(), QString(), QString(), uc);
    pd->curves << hl;

    // color down bars
    QList<int> keys = g_symbol->keys();
  
    for (int pos = 0; pos < keys.size(); pos++)
    {
      Bar *bar = hl->bar(keys.at(pos));
      if (! bar)
        continue;

      if (bar->value() < bar->base())
        bar->setColor(dc);
    }
  }
  
  // ma
  show = pd->settings->get(QString("maShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    Curve *ma = getMA(pd->settings);
    if (ma)
      pd->curves << ma;
  }
  
  return 1;
}

Curve *
ROC::getMA (Entity *settings)
{
  QVariant *type = settings->get(QString("maType"));
  if (! type)
    return 0;
  
  QVariant *period = settings->get(QString("maPeriod"));
  if (! period)
    return 0;
  
  QVariant *var = settings->get(QString("maColor"));
  if (! var)
    return 0;
  QColor color(var->toString());
  
  QVariant *label = settings->get(QString("maLabel"));
  if (! label)
    return 0;
  
  QVariant *style = settings->get(QString("maStyle"));
  if (! style)
    return 0;
  
  QVariant *width = settings->get(QString("maWidth"));
  if (! width)
    return 0;
  
  QVariant *momLabel = settings->get(QString("label"));
  if (! momLabel)
    return 0;

  if (! getMA(momLabel->toString(), label->toString(), type->toInt(), period->toInt()))
    return 0;
  
  CurveLineType clt;
  Curve *curve = new Curve(QString("CurveLine"));
  curve->setLabel(label->toString());
  curve->setStyle(clt.stringToIndex(style->toString()));
  curve->setPen(width->toInt());
  curve->setColor(color);
  curve->fill(label->toString(), QString(), QString(), QString(), color);

  return curve;
}

int
ROC::getROC (QString ikey, int method, int period, QString okey)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "ROC::ROC: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double v;
    if (! bar->get(ikey, v))
      continue;
    
    input[dpos++] = (TA_Real) v;
  }

  switch ((ROCType::Key) method)
  {
    case ROCType::_ROC:
      rc = TA_ROC (0, dpos - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ROCType::_ROCP:
      rc = TA_ROCP (0, dpos - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ROCType::_ROCR:
      rc = TA_ROCR (0, dpos - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case ROCType::_ROCR100:
      rc = TA_ROCR100 (0, dpos - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }
    
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ROC::getROC: TA-Lib error" << rc;
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
ROC::getMA (QString inKey, QString outKey, int type, int period)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "ROC::getMA: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double v;
    if (! bar->get(inKey, v))
      continue;
    
    input[dpos++] = (TA_Real) v;
  }

  rc = TA_MA(0, dpos - 1, &input[0], period, (TA_MAType) type, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "ROC::getMA: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    bar->set(outKey, out[outLoop]);
    keyLoop--;
    outLoop--;
  }
  
  return 1;
}

int
ROC::settings (PluginData *pd)
{
  Entity *command = new Entity;
  BarType bt;
  MAType mat;
  CurveLineType clt;
  ROCType rt;
  
  command->set(QString("plugin"), new QVariant(QString("ROC")));
  command->set(QString("type"), new QVariant(QString("indicator")));
  
  command->set(QString("input"), new QVariant(bt.indexToString(BarType::_CLOSE)));
  command->set(QString("upColor"), new QVariant(QString("green")));
  command->set(QString("downColor"), new QVariant(QString("red")));
  command->set(QString("period"), new QVariant(14));
  command->set(QString("label"), new QVariant(QString("ROC")));
  command->set(QString("method"), new QVariant(rt.indexToString(ROCType::_ROC)));
  command->set(QString("rocShow"), new QVariant(TRUE));
  
  command->set(QString("maType"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("maStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("maWidth"), new QVariant(1));
  command->set(QString("maColor"), new QVariant(QString("yellow")));
  command->set(QString("maPeriod"), new QVariant(10));
  command->set(QString("maLabel"), new QVariant(QString("MA")));
  command->set(QString("maShow"), new QVariant(TRUE));
  
  pd->settings = command;
  
  return 1;
}

int
ROC::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(roc, ROC);
