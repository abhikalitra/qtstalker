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
#include "BBANDS.h"
#include "MAType.h"
#include "BarType.h"
#include "CurveLineType.h"
#include "CurveOHLCType.h"
#include "Global.h"
#include "BBANDSDialog.h"

int
BBANDS::command (PluginData *pd)
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
BBANDS::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "BBANDS::dialog: invalid parent";
    return 0;
  }

  if (! pd->settings)
  {
    qDebug() << "BBANDS::dialog: invalid settings";
    return 0;
  }

  BBANDSDialog *dialog = new BBANDSDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;

  return 1;
}

int
BBANDS::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;

  QVariant *uc = pd->settings->get(QString("upColor"));
  if (! uc)
    return 0;

  QVariant *dc = pd->settings->get(QString("downColor"));
  if (! dc)
    return 0;

  QVariant *nc = pd->settings->get(QString("neutralColor"));
  if (! nc)
    return 0;

  QVariant *label = pd->settings->get(QString("label"));
  if (! label)
    return 0;

  QVariant *style = pd->settings->get(QString("style"));
  if (! style)
    return 0;

  Curve *ohlc = getOHLC(style->toString(), label->toString(), uc->toString(), dc->toString(), nc->toString());
  if (! ohlc)
    return 0;
  pd->curves << ohlc;

  // bbands
  QVariant *input = pd->settings->get(QString("input"));
  if (! input)
    return 0;

  QVariant *period = pd->settings->get(QString("period"));
  if (! period)
    return 0;

  QVariant *pdev = pd->settings->get(QString("devUp"));
  if (! pdev)
    return 0;

  QVariant *ndev = pd->settings->get(QString("devDown"));
  if (! ndev)
    return 0;

  QVariant *maType = pd->settings->get(QString("type"));
  if (! maType)
    return 0;
  MAType mat;
  int ma = mat.stringToIndex(maType->toString());

  QVariant *bbuLabel = pd->settings->get(QString("upLabel"));
  if (! bbuLabel)
    return 0;

  QVariant *bbmLabel = pd->settings->get(QString("midLabel"));
  if (! bbmLabel)
    return 0;

  QVariant *bblLabel = pd->settings->get(QString("downLabel"));
  if (! bblLabel)
    return 0;

  if (! getBBANDS(input->toString(),
                  period->toInt(),
                  pdev->toDouble(),
                  ndev->toDouble(),
                  ma,
                  bbuLabel->toString(),
                  bbmLabel->toString(),
                  bblLabel->toString()))
    return 0;

  // upper
  QVariant *show = pd->settings->get(QString("upShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("upperColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    QVariant *style = pd->settings->get(QString("upStyle"));
    if (! style)
      return 0;

    QVariant *width = pd->settings->get(QString("upWidth"));
    if (! width)
      return 0;

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(bbuLabel->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(bbuLabel->toString(), QString(), QString(), QString(), QColor());
    c->setPen(width->toInt());
    pd->curves << c;
  }

  // middle
  show = pd->settings->get(QString("midShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("midColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    QVariant *style = pd->settings->get(QString("midStyle"));
    if (! style)
      return 0;

    QVariant *width = pd->settings->get(QString("midWidth"));
    if (! width)
      return 0;

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(bbmLabel->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(bbmLabel->toString(), QString(), QString(), QString(), QColor());
    c->setPen(width->toInt());
    pd->curves << c;
  }

  // lower
  show = pd->settings->get(QString("downShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("lowerColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    QVariant *style = pd->settings->get(QString("downStyle"));
    if (! style)
      return 0;

    QVariant *width = pd->settings->get(QString("downWidth"));
    if (! width)
      return 0;

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(bblLabel->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(bblLabel->toString(), QString(), QString(), QString(), QColor());
    c->setPen(width->toInt());
    pd->curves << c;
  }

  return 1;
}

int
BBANDS::getBBANDS (QString inKey, int period, double pdev, double ndev, int type,
                   QString ulabel, QString mlabel, QString llabel)
{
  if (! g_symbol)
    return 0;

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "BBANDS::BBANDS: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real input[MAX_SIZE];
  TA_Real out[MAX_SIZE];
  TA_Real out2[MAX_SIZE];
  TA_Real out3[MAX_SIZE];
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

  rc = TA_BBANDS(0,
                 dpos - 1,
                 &input[0],
                 period,
                 pdev,
                 ndev,
                 (TA_MAType) type,
                 &outBeg,
                 &outNb,
                 &out[0],
                 &out2[0],
                 &out3[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "BBANDS::getBBANDS: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));

    bar->set(ulabel, out[outLoop]);
    bar->set(mlabel, out2[outLoop]);
    bar->set(llabel, out3[outLoop]);

    keyLoop--;
    outLoop--;
  }

  return 1;
}

Curve *
BBANDS::getOHLC (QString tstyle, QString key, QString tuc, QString tdc, QString tnc)
{
  if (! g_symbol)
    return 0;

  Curve *ohlc = new Curve(QString("CurveOHLC"));
  ohlc->setLabel(key);

  CurveOHLCType ohlcType;
  ohlc->setStyle(ohlcType.stringToIndex(tstyle));

  QColor uc(tuc);
  QColor dc(tdc);
  QColor nc(tnc);

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
BBANDS::settings (PluginData *pd)
{
  Entity *command = new Entity;
  BarType bt;
  CurveOHLCType ct;
  CurveLineType clt;
  MAType mat;

  command->set(QString("plugin"), new QVariant(QString("BBANDS")));
  command->set(QString("type"), new QVariant(QString("indicator")));

  command->set(QString("style"), new QVariant(ct.indexToString(CurveOHLCType::_OHLC)));
  command->set(QString("upColor"), new QVariant(QString("green")));
  command->set(QString("downColor"), new QVariant(QString("red")));
  command->set(QString("neutralColor"), new QVariant(QString("blue")));
  command->set(QString("label"), new QVariant(QString("OHLC")));

  command->set(QString("input"), new QVariant(bt.indexToString(BarType::_CLOSE)));
  command->set(QString("type"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("period"), new QVariant(20));
  command->set(QString("devUp"), new QVariant(2.0));
  command->set(QString("devDown"), new QVariant(2.0));

  command->set(QString("upStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("upWidth"), new QVariant(1));
  command->set(QString("upperColor"), new QVariant(QString("red")));
  command->set(QString("upLabel"), new QVariant(QString("BBU")));
  command->set(QString("upShow"), new QVariant(TRUE));

  command->set(QString("midStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("midWidth"), new QVariant(1));
  command->set(QString("midColor"), new QVariant(QString("yellow")));
  command->set(QString("midLabel"), new QVariant(QString("BBM")));
  command->set(QString("midShow"), new QVariant(TRUE));

  command->set(QString("downStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("downWidth"), new QVariant(1));
  command->set(QString("lowerColor"), new QVariant(QString("green")));
  command->set(QString("downLabel"), new QVariant(QString("BBL")));
  command->set(QString("downShow"), new QVariant(TRUE));

  pd->settings = command;

  return 1;
}

int
BBANDS::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(bbands, BBANDS);
