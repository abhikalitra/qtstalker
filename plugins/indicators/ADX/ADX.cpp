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

#include "ADX.h"
#include "Wilder.h"
#include "TR.h"
#include "PlotFactory.h"

#include <QtDebug>


ADX::ADX ()
{
  indicator = "ADX";

  settings.setData(ADXColor, "blue");
  settings.setData(ADXRColor, "yellow");
  settings.setData(PDIColor, "green");
  settings.setData(MDIColor, "red");
  settings.setData(ADXPlot, "Line");
  settings.setData(ADXRPlot, "Line");
  settings.setData(PDIPlot, "Line");
  settings.setData(MDIPlot, "Line");
  settings.setData(ADXLabel, "ADX");
  settings.setData(ADXRLabel, "ADXR");
  settings.setData(PDILabel, "+DI");
  settings.setData(MDILabel, "-DI");
  settings.setData(ADXCheck, 1);
  settings.setData(ADXRCheck, 1);
  settings.setData(PDICheck, 1);
  settings.setData(MDICheck, 1);
  settings.setData(Period, 14);

  methodList << "ADX" << "ADXR" << "+DI" << "-DI" << "DX";
}

int ADX::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(Period);

  if (settings.getInt(MDICheck))
  {
    QString s;
    settings.getData(MDIColor, s);
    QColor color(s);

    PlotFactory fac;
    settings.getData(MDIPlot, s);
    int lineType = fac.typeFromString(s);
    
    PlotLine *line = getLine(data, period, MDI, lineType, color);
    if (! line)
      return 1;

    settings.getData(MDILabel, s);
    line->setLabel(s);
    ind.addLine(line);
  }

  if (settings.getInt(PDICheck))
  {
    QString s;
    settings.getData(PDIColor, s);
    QColor color(s);

    PlotFactory fac;
    settings.getData(PDIPlot, s);
    int lineType = fac.typeFromString(s);

    PlotLine *line = getLine(data, period, PDI, lineType, color);
    if (! line)
      return 1;

    settings.getData(PDILabel, s);
    line->setLabel(s);
    ind.addLine(line);
  }

  if (settings.getInt(ADXCheck))
  {
    QString s;
    settings.getData(ADXColor, s);
    QColor color(s);

    PlotFactory fac;
    settings.getData(ADXPlot, s);
    int lineType = fac.typeFromString(s);

    PlotLine *line = getLine(data, period, _ADX, lineType, color);
    if (! line)
      return 1;

    settings.getData(ADXLabel, s);
    line->setLabel(s);
    ind.addLine(line);
  }

  if (settings.getInt(ADXRCheck))
  {
    QString s;
    settings.getData(ADXRColor, s);
    QColor color(s);

    PlotFactory fac;
    settings.getData(ADXRPlot, s);
    int lineType = fac.typeFromString(s);

    PlotLine *line = getLine(data, period, ADXR, lineType, color);
    if (! line)
      return 1;

    settings.getData(ADXRLabel, s);
    line->setLabel(s);
    ind.addLine(line);
  }

  return 0;
}

int ADX::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,ADX,<METHOD>,<NAME>,<PERIOD>,<PLOT TYPE>,<COLOR>
  //     0       1     2     3        4      5          6         7

  if (set.count() != 8)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << indicator << "::getCUS: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid period" << set[5];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[6]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[6];
    return 1;
  }

  QColor color(set[7]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[7];
    return 1;
  }

  PlotLine *line = getLine(data, period, method, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * ADX::getLine (BarData *data, int period, int method, int lineType, QColor &color)
{
  PlotLine *line = 0;

  switch ((Method) method)
  {
    case MDI:
      line = getDI(data, period, 0, lineType, color);
      break;
    case PDI:
      line = getDI(data, period, 1, lineType, color);
      break;
    case _ADX:
      line = getADX(data, period, lineType, color);
      break;
    case ADXR:
      line = getADXR(data, period, lineType, color);
      break;
    case DX:
      line = getDX(data, period, lineType, color);
      break;
    case MDM:
      line = getDM(data, 0);
      break;
    case PDM:
      line = getDM(data, 1);
      break;
    default:
      break;
  }

  return line;
}

PlotLine * ADX::getADXR (BarData *data, int period, int lineType, QColor &color)
{
  PlotLine *adx = getADX(data, period, lineType, color);
  if (! adx)
    return 0;

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
  {
    delete adx;
    return 0;
  }

  QList<int> keys;
  adx->keys(keys);
  
  int loop = period;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = adx->data(keys.at(loop));
    PlotLineBar *pbar = adx->data(keys.at(loop - period));
    line->setData(keys.at(loop), new PlotLineBar(color, (bar->data() + pbar->data()) / 2));
  }

  delete adx;
  return line;
}

PlotLine * ADX::getADX (BarData *data, int period, int lineType, QColor &color)
{
  PlotLine *dx = getDX(data, period, lineType, color);
  if (! dx)
    return 0;

  Wilder ma;
  PlotLine *adx = ma.wilder(dx, period, lineType, color);
  
  delete dx;
  return adx;
}

PlotLine * ADX::getDX (BarData *data, int period, int lineType, QColor &color)
{
  PlotLine *pdi = getDI(data, period, 1, lineType, color);
  if (! pdi)
    return 0;

  PlotLine *mdi = getDI(data, period, 0, lineType, color);
  if (! mdi)
  {
    delete pdi;
    return 0;
  }

  PlotFactory fac;
  PlotLine *dx = fac.plot(lineType);
  if (! dx)
  {
    delete pdi;
    delete mdi;
    return 0;
  }

  QList<int> keys;
  pdi->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *pbar = pdi->data(keys.at(loop));
    PlotLineBar *mbar = mdi->data(keys.at(loop));
    
    double t = fabs(pbar->data() - mbar->data());
    double t2 = pbar->data() + mbar->data();
    dx->setData(keys.at(loop), new PlotLineBar(color, (t / t2) * 100));
  }

  delete pdi;
  delete mdi;
  
  return dx;  
}

PlotLine * ADX::getDI (BarData *data, int period, int flag, int lineType, QColor &color)
{
  PlotLine *dm = getDM(data, flag);
  if (! dm)
    return 0;
  
  Wilder ma;
  PlotLine *dmma = ma.wilder(dm, period, lineType, color);
  if (! dmma)
  {
    delete dm;
    return 0;
  }

  TR ttr;
  PlotLine *tr = ttr.tr(data, lineType, color);
  PlotLine *trma = ma.wilder(tr, period, lineType, color);
  if (! trma)
  {
    delete dm;
    delete dmma;
    delete tr;
    return 0;
  }

  PlotFactory fac;
  PlotLine *di = fac.plot(lineType);
  if (! di)
  {
    delete dm;
    delete dmma;
    delete tr;
    delete trma;
    return 0;
  }

  QList<int> keys;
  trma->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *dmbar = dmma->data(keys.at(loop));
    if (! dmbar)
      continue;
    
    PlotLineBar *trbar = trma->data(keys.at(loop));
    
    double t = dmbar->data() / trbar->data();
    di->setData(keys.at(loop), new PlotLineBar(color, t * 100));
  }

  delete dm;
  delete dmma;
  delete tr;
  delete trma;
  
  return di;
}

PlotLine * ADX::getDM (BarData *data, int flag)
{
  PlotLine *line = new PlotLine;
  int loop = 1;
  QColor color("red");
  for (; loop < data->count(); loop++)
  {
    double pdm = 0;
    double mdm = 0;
    
    Bar *pbar = data->getBar(loop - 1);
    Bar *bar = data->getBar(loop);

    double pd = bar->getHigh() - pbar->getHigh();
    double md = pbar->getLow() - bar->getLow();

    if ((pd > 0) && (pd > md))
      pdm = pd;
    else if ((md > 0) && (md > pd))
        mdm = md;

    if (flag)
      line->setData(loop, new PlotLineBar(color, pdm));
    else
      line->setData(loop, new PlotLineBar(color, mdm));
  }

  return line;
}

int ADX::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("General");
  dialog->addPage(page, k);

  dialog->addIntItem(Period, page, QObject::tr("Period"), settings.getInt(Period), 2, 100000);

  page++;
  k = QObject::tr("-DI");
  dialog->addPage(page, k);

  settings.getData(MDIColor, d);
  dialog->addColorItem(MDIColor, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  settings.getData(MDIPlot, d);
  dialog->addComboItem(MDIPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MDILabel, d);
  dialog->addTextItem(MDILabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(MDICheck, page, QObject::tr("Show"), settings.getInt(MDICheck));

  page++;
  k = QObject::tr("+DI");
  dialog->addPage(page, k);

  settings.getData(PDIColor, d);
  dialog->addColorItem(PDIColor, page, QObject::tr("Color"), d);

  settings.getData(PDIPlot, d);
  dialog->addComboItem(PDIPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(PDILabel, d);
  dialog->addTextItem(PDILabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(PDICheck, page, QObject::tr("Show"), settings.getInt(MDICheck));

  page++;
  k = QObject::tr("ADX");
  dialog->addPage(page, k);

  settings.getData(ADXColor, d);
  dialog->addColorItem(ADXColor, page, QObject::tr("Color"), d);

  settings.getData(ADXPlot, d);
  dialog->addComboItem(ADXPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(ADXLabel, d);
  dialog->addTextItem(ADXLabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(ADXCheck, page, QObject::tr("Show"), settings.getInt(MDICheck));

  page++;
  k = QObject::tr("ADXR");
  dialog->addPage(page, k);

  settings.getData(ADXRColor, d);
  dialog->addColorItem(ADXRColor, page, QObject::tr("Color"), d);

  settings.getData(ADXRPlot, d);
  dialog->addComboItem(ADXRPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(ADXRLabel, d);
  dialog->addTextItem(ADXRLabel, page, QObject::tr("Label"), d);

  dialog->addCheckItem(ADXRCheck, page, QObject::tr("Show"), settings.getInt(MDICheck));

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  getDialogSettings(dialog);

  delete dialog;
  return rc;
}


//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ADX *o = new ADX;
  return ((IndicatorPlugin *) o);
}


