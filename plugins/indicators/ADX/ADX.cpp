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
#include "MAUtils.h"
#include "DIV.h"
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

  methodList << "ADX" << "ADXR" << "+DI" << "-DI" << "DX" << "-DM" << "+DM";
}

int ADX::getIndicator (Indicator &ind, BarData *data)
{
  int period = settings.getInt(Period);

  if (settings.getInt(MDICheck))
  {
    PlotLine *line = getLine(data, period, MDI);
    if (! line)
      return 1;

    QString s;
    settings.getData(MDIColor, s);
    line->setColor(s);
    settings.getData(MDIPlot, s);
    line->setPlugin(s);
    settings.getData(MDILabel, s);
    line->setLabel(s);
    ind.addLine(line);
  }

  if (settings.getInt(PDICheck))
  {
    PlotLine *line = getLine(data, period, PDI);
    if (! line)
      return 1;

    QString s;
    settings.getData(PDIColor, s);
    line->setColor(s);
    settings.getData(PDIPlot, s);
    line->setPlugin(s);
    settings.getData(PDILabel, s);
    line->setLabel(s);
    ind.addLine(line);
  }

  if (settings.getInt(ADXCheck))
  {
    PlotLine *line = getLine(data, period, _ADX);
    if (! line)
      return 1;

    QString s;
    settings.getData(ADXColor, s);
    line->setColor(s);
    settings.getData(ADXPlot, s);
    line->setPlugin(s);
    settings.getData(ADXLabel, s);
    line->setLabel(s);
    ind.addLine(line);
  }

  if (settings.getInt(ADXRCheck))
  {
    PlotLine *line = getLine(data, period, ADXR);
    if (! line)
      return 1;

    QString s;
    settings.getData(ADXRColor, s);
    line->setColor(s);
    settings.getData(ADXRPlot, s);
    line->setPlugin(s);
    settings.getData(ADXRLabel, s);
    line->setLabel(s);
    ind.addLine(line);
  }

  return 0;
}

int ADX::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,ADX,<METHOD>,<NAME>,<PERIOD>

  if (set.count() != 6)
  {
    qDebug() << indicator << "::calculate: invalid settings count" << set.count();
    return 1;
  }

  int method = methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << indicator << "::calculate: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::calculate: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::calculate: invalid period" << set[5];
    return 1;
  }

  PlotLine *line = getLine(data, period, method);
  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * ADX::getLine (BarData *data, int period, int method)
{
  PlotLine *line = 0;

  switch ((Method) method)
  {
    case MDI:
      line = getDI(data, period, 0);
      break;
    case PDI:
      line = getDI(data, period, 1);
      break;
    case _ADX:
      line = getADX(data, period);
      break;
    case ADXR:
      line = getADXR(data, period);
      break;
    case DX:
      line = getDX(data, period);
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

PlotLine * ADX::getADXR (BarData *data, int period)
{
  PlotLine *adx = getADX(data, period);
  if (! adx)
    return 0;

  PlotLine *adxr = new PlotLine;
  int loop = period;
  for (; loop < adx->count(); loop++)
    adxr->append((adx->getData(loop) + adx->getData(loop - period)) / 2);

  delete adx;
  return adxr;
}

PlotLine * ADX::getADX (BarData *data, int period)
{
  PlotLine *dx = getDX(data, period);
  if (! dx)
    return 0;

  MAUtils mau;
  PlotLine *adx = mau.getMA(dx, period, MAUtils::Wilder);
  
  delete dx;
  return adx;
}

PlotLine * ADX::getDX (BarData *data, int period)
{
  PlotLine *pdi = getDI(data, period, 1);
  if (! pdi)
    return 0;

  PlotLine *mdi = getDI(data, period, 0);
  if (! mdi)
  {
    delete pdi;
    return 0;
  }

  PlotLine *dx = new PlotLine;
  int loop = 0;
  for (; loop < pdi->count(); loop++)
  {
    double t = fabs(pdi->getData(loop) - mdi->getData(loop));
    double t2 = pdi->getData(loop) + mdi->getData(loop);
    dx->append((t / t2) * 100);
  }

  delete pdi;
  delete mdi;
  
  return dx;  
}

PlotLine * ADX::getDI (BarData *data, int period, int flag)
{
  PlotLine *dm = getDM(data, flag);
  if (! dm)
    return 0;
  
  MAUtils mau;
  PlotLine *dmma = mau.getMA(dm, period, MAUtils::Wilder);
  if (! dmma)
  {
    delete dm;
    return 0;
  }

  TR ttr;
  PlotLine *tr = ttr.getTR(data);
  PlotLine *trma = mau.getMA(tr, period, MAUtils::Wilder);
  if (! trma)
  {
    delete dm;
    delete dmma;
    delete tr;
    return 0;
  }

  DIV div;
  PlotLine *tdi = div.getDIV(dmma, trma);

  PlotLine *di = new PlotLine;
  int loop = 0;
  for (; loop < tdi->count(); loop++)
  {
    di->append(tdi->getData(loop) * 100);
  }

  delete dm;
  delete dmma;
  delete tr;
  delete trma;
  delete tdi;
  
  return di;
}

PlotLine * ADX::getDM (BarData *data, int flag)
{
  PlotLine *line = new PlotLine;
  int loop = 1;
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
      line->append(pdm);
    else
      line->append(mdm);
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


