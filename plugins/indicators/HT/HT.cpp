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

#include "HT.h"
#include "ta_libc.h"
#include "BARSUtils.h"
#include "PlotFactory.h"

#include <QtDebug>

HT::HT ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("TALIB::setDefaults:error on TA_Initialize");

  _indicator = "HT";

  _settings.setData(Method, "TRENDLINE");
  _settings.setData(Color, "red");
  _settings.setData(PhaseColor, "red");
  _settings.setData(QuadColor, "yellow");
  _settings.setData(SineColor, "red");
  _settings.setData(LeadColor, "yellow");
  _settings.setData(Plot, "Line");
  _settings.setData(PhasePlot, "Line");
  _settings.setData(QuadPlot, "Line");
  _settings.setData(SinePlot, "Line");
  _settings.setData(LeadPlot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(PhaseLabel, "PHASE");
  _settings.setData(QuadLabel, "QUAD");
  _settings.setData(SineLabel, "SINE");
  _settings.setData(LeadLabel, "LEAD");
  _settings.setData(Input, "Close");

  _methodList << "DCPERIOD";
  _methodList << "DCPHASE";
  _methodList << "TRENDLINE";
  _methodList << "TRENDMODE";
  _methodList << "PHASOR";
  _methodList << "SINE";
}

int HT::getIndicator (Indicator &ind, BarData &data)
{
  QString s;
  _settings.getData(Input, s);
  PlotLine *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::calculate: input not found" << s;
    return 1;
  }

  _settings.getData(Method, s);
  int method = _methodList.indexOf(s);

  switch (method)
  {
    case 4: // phasor
    {
      _settings.getData(PhaseColor, s);
      QColor pcolor(s);

      PlotFactory fac;
      _settings.getData(PhasePlot, s);
      int plineType = fac.typeFromString(s);

      _settings.getData(QuadColor, s);
      QColor qcolor(s);

      _settings.getData(QuadPlot, s);
      int qlineType = fac.typeFromString(s);

      QList<PlotLine *> pl;
      if (getPHASE(in, plineType, qlineType, pcolor, qcolor, pl))
      {
        delete in;
        return 1;
      }

      PlotLine *line = pl.at(0);
      _settings.getData(PhaseLabel, s);
      line->setLabel(s);

      s = "0";
      ind.setLine(s, line);
      ind.addPlotOrder(s);

      line = pl.at(1);
      _settings.getData(QuadLabel, s);
      line->setLabel(s);
      
      s = "1";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
    case 5: // sine
    {
      _settings.getData(SineColor, s);
      QColor scolor(s);

      PlotFactory fac;
      _settings.getData(SinePlot, s);
      int slineType = fac.typeFromString(s);

      _settings.getData(LeadColor, s);
      QColor lcolor(s);

      _settings.getData(LeadPlot, s);
      int llineType = fac.typeFromString(s);

      QList<PlotLine *> pl;
      if (getSINE(in, slineType, llineType, scolor, lcolor, pl))
      {
        delete in;
        return 1;
      }
      
      PlotLine *line = pl.at(0);
      _settings.getData(SineLabel, s);
      line->setLabel(s);
      
      s = "2";
      ind.setLine(s, line);
      ind.addPlotOrder(s);

      line = pl.at(1);
      _settings.getData(LeadLabel, s);
      line->setLabel(s);
      
      s = "3";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
    default:
    {
      QColor up("green");
      QColor down("red");
      QColor neutral("blue");
      BARSUtils b;
      PlotLine *bars = b.getBARS(data, up, down, neutral);
      if (bars)
      {
        s = "4";
        ind.setLine(s, bars);
        ind.addPlotOrder(s);
      }

      _settings.getData(Color, s);
      QColor color(s);

      PlotFactory fac;
      _settings.getData(Plot, s);
      int lineType = fac.typeFromString(s);

      PlotLine *line = getHT(in, method, lineType, color);
      if (! line)
      {
        delete in;
        return 1;
      }

      _settings.getData(Label, s);
      line->setLabel(s);
      
      s = "5";
      ind.setLine(s, line);
      ind.addPlotOrder(s);
      break;
    }
  }

  delete in;
  return 0;
}

int HT::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,HT,METHOD,*
  //     0       1     2    3

  if (set.count() < 4)
  {
    qDebug() << _indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  int rc = 1;
  int method = _methodList.indexOf(set[3]);
  switch (method)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      rc = getCUSHT(set, ind, data);
      break;      
    case 4:
      rc = getCUSPHASE(set, ind, data);
      break;
    case 5:
      rc = getCUSSINE(set, ind, data);
      break;
    default:
      break;
  }

  return rc;
}

int HT::getCUSPHASE (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,HT,<METHOD>,<INPUT>,<PHASE NAME>,<QUAD NAME>,<PHASE PLOT TYPE>,<QUAD PLOT TYPE>,<PHASE COLOR>,<QUAD COLOR>
  //     0       1     2     3       4          5           6              7                 8              9            10

  if (set.count() != 11)
  {
    qDebug() << _indicator << "::getCUSPHASE: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << _indicator << "::getCUSPHASE: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  PlotLine *tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUSPHASE: duplicate phase name" << set[5];
    return 1;
  }

  tl = ind.line(set[6]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUSPHASE: duplicate quad name" << set[6];
    return 1;
  }

  PlotFactory fac;
  int plineType = fac.typeFromString(set[7]);
  if (plineType == -1)
  {
    qDebug() << _indicator << "::getCUSPHASE: invalid phase plot type" << set[7];
    return 1;
  }

  int qlineType = fac.typeFromString(set[8]);
  if (qlineType == -1)
  {
    qDebug() << _indicator << "::getCUSPHASE: invalid quad plot type" << set[8];
    return 1;
  }

  QColor pcolor(set[9]);
  if (! pcolor.isValid())
  {
    qDebug() << _indicator << "::getCUSPHASE: invalid phase color" << set[9];
    return 1;
  }

  QColor qcolor(set[10]);
  if (! qcolor.isValid())
  {
    qDebug() << _indicator << "::getCUSPHASE: invalid quad color" << set[10];
    return 1;
  }

  QList<PlotLine *> pl;
  if (getPHASE(in, plineType, qlineType, pcolor, qcolor, pl))
    return 1;

  pl.at(0)->setLabel(set[5]);
  pl.at(1)->setLabel(set[6]);

  ind.setLine(set[5], pl.at(0));
  ind.setLine(set[6], pl.at(1));

  return 0;
}

int HT::getCUSSINE (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,HT,<METHOD>,<INPUT>,<SINE NAME>,<LEAD NAME>,<SINE PLOT TYPE>,<LEAD PLOT TYPE>,<SINE COLOR>,<LEAD COLOR>
  //     0       1     2     3       4          5           6              7                 8              9            10

  if (set.count() != 11)
  {
    qDebug() << _indicator << "::getCUSSINE: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << _indicator << "::getCUSSINE: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  PlotLine *tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUSSINE: duplicate sine name" << set[5];
    return 1;
  }

  tl = ind.line(set[6]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUSSINE: duplicate lead name" << set[6];
    return 1;
  }

  PlotFactory fac;
  int slineType = fac.typeFromString(set[7]);
  if (slineType == -1)
  {
    qDebug() << _indicator << "::getCUSSINE: invalid sine plot type" << set[7];
    return 1;
  }

  int llineType = fac.typeFromString(set[8]);
  if (llineType == -1)
  {
    qDebug() << _indicator << "::getCUSSINE: invalid lead plot type" << set[8];
    return 1;
  }

  QColor scolor(set[9]);
  if (! scolor.isValid())
  {
    qDebug() << _indicator << "::getCUSSINE: invalid sine color" << set[9];
    return 1;
  }

  QColor lcolor(set[10]);
  if (! lcolor.isValid())
  {
    qDebug() << _indicator << "::getCUSPHASE: invalid lead color" << set[10];
    return 1;
  }

  QList<PlotLine *> pl;
  if (getSINE(in, slineType, llineType, scolor, lcolor, pl))
    return 1;

  pl.at(0)->setLabel(set[5]);
  pl.at(1)->setLabel(set[6]);

  ind.setLine(set[5], pl.at(0));
  ind.setLine(set[6], pl.at(1));

  return 0;
}

int HT::getCUSHT (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,HT,<METHOD>,<NAME>,<INPUT>,<PLOT TYPE>,<COLOR>
  //     0       1     2    3       4       5        6          7

  if (set.count() != 8)
  {
    qDebug() << _indicator << "::getCUSHT: invalid settings count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << _indicator << "::getCUSHT: invalid method" << set[3];
    return 1;
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUSHT: duplicate name" << set[4];
    return 1;
  }

  PlotLine *in = ind.line(set[5]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[5]));
    if (! in)
    {
      qDebug() << _indicator << "::getCUSHT: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in);
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[6]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUSHT: invalid plot type" << set[6];
    return 1;
  }

  QColor color(set[7]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUSHT: invalid color" << set[7];
    return 1;
  }

  PlotLine *line = getHT(in, method, lineType, color);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

PlotLine * HT::getHT (PlotLine *in, int method, int lineType, QColor &color)
{
  if (in->count() < 1)
    return 0;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];

  QList<int> keys;
  in->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar bar;
    in->data(keys.at(loop), bar);
    input[loop] = (TA_Real) bar.data();
  }

  TA_RetCode rc = TA_SUCCESS;
  switch (method)
  {
    case 0:
      rc = TA_HT_DCPERIOD (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 1:
      rc = TA_HT_DCPHASE (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 2:
      rc = TA_HT_TRENDLINE (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case 3:
    {
      TA_Integer iout[size];
      rc = TA_HT_TRENDMODE (0, size - 1, &input[0], &outBeg, &outNb, &iout[0]);
      if (rc != TA_SUCCESS)
      {
        qDebug() << _indicator << "::getHT TA-Lib error" << rc;
        return 0;
      }
      
      PlotFactory fac;
      PlotLine *line = fac.plot(lineType);
      if (! line)
        return 0;

      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        PlotLineBar bar(color, (double) iout[outLoop]);
        line->setData(keys.at(keyLoop), bar);
        keyLoop--;
        outLoop--;
      }

      return line;
      break;
    }
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getHT TA-Lib error" << rc;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    PlotLineBar bar(color, out[outLoop]);
    line->setData(keys.at(keyLoop), bar);
    keyLoop--;
    outLoop--;
  }

  return line;
}

int HT::getPHASE (PlotLine *in, int plineType, int qlineType, QColor &pcolor, QColor &qcolor, QList<PlotLine *> &pl)
{
  if (in->count() < 1)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar bar;
    in->data(keys.at(loop), bar);
    input[loop] = (TA_Real) bar.data();
  }

  TA_RetCode rc = TA_HT_PHASOR (0, size - 1, &input[0], &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getPHASE: TA-Lib error" << rc;
    return 1;
  }

  PlotFactory fac;
  PlotLine *pline = fac.plot(plineType);
  if (! pline)
    return 1;

  PlotLine *qline = fac.plot(qlineType);
  if (! qline)
  {
    delete pline;
    return 1;
  }

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    PlotLineBar bar(pcolor, out[outLoop]);
    pline->setData(keys.at(keyLoop), bar);
    
    PlotLineBar bar2(qcolor, out2[outLoop]);
    qline->setData(keys.at(keyLoop), bar2);
    keyLoop--;
    outLoop--;
  }

  pl.append(pline);
  pl.append(qline);

  return 0;
}

int HT::getSINE (PlotLine *in, int slineType, int llineType, QColor &scolor, QColor &lcolor, QList<PlotLine *> &pl)
{
  if (in->count() < 1)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar bar;
    in->data(keys.at(loop), bar);
    input[loop] = (TA_Real) bar.data();
  }

  TA_RetCode rc = TA_HT_SINE (0, size - 1, &input[0], &outBeg, &outNb, &out[0], &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << _indicator << "::getSINE: TA-Lib error" << rc;
    return 1;
  }

  PlotFactory fac;
  PlotLine *sline = fac.plot(slineType);
  if (! sline)
    return 1;

  PlotLine *lline = fac.plot(llineType);
  if (! lline)
  {
    delete sline;
    return 1;
  }

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    PlotLineBar bar(scolor, out[outLoop]);
    sline->setData(keys.at(keyLoop), bar);
    
    PlotLineBar bar2(lcolor, out2[outLoop]);
    lline->setData(keys.at(keyLoop), bar2);
    keyLoop--;
    outLoop--;
  }

  pl.append(sline);
  pl.append(lline);

  return 0;
}

int HT::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  _settings.getData(Color, d);
  dialog->addColorItem(Color, page, QObject::tr("Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d, QString());

  BarData bd;
  QStringList inputList;
  bd.getInputFields(inputList);

  _settings.getData(Input, d);
  dialog->addComboItem(Input, page, QObject::tr("Input"), inputList, d);

  _settings.getData(Method, d);
  dialog->addComboItem(Method, page, QObject::tr("Method"), _methodList, d);

  page++;
  k = QObject::tr("Phasor");
  dialog->addPage(page, k);

  _settings.getData(PhaseColor, d);
  dialog->addColorItem(PhaseColor, page, QObject::tr("Phase Color"), d);

  _settings.getData(QuadColor, d);
  dialog->addColorItem(QuadColor, page, QObject::tr("Quad Color"), d);

  _settings.getData(PhasePlot, d);
  dialog->addComboItem(PhasePlot, page, QObject::tr("Phase Plot"), plotList, d);

  _settings.getData(QuadPlot, d);
  dialog->addComboItem(QuadPlot, page, QObject::tr("Quad Plot"), plotList, d);

  _settings.getData(PhaseLabel, d);
  dialog->addTextItem(PhaseLabel, page, QObject::tr("Phase Label"), d, QString());

  _settings.getData(QuadLabel, d);
  dialog->addTextItem(QuadLabel, page, QObject::tr("Quad Label"), d, QString());

  page++;
  k = QObject::tr("Sine Wave");
  dialog->addPage(page, k);

  _settings.getData(SineColor, d);
  dialog->addColorItem(SineColor, page, QObject::tr("Sine Color"), d);

  _settings.getData(LeadColor, d);
  dialog->addColorItem(LeadColor, page, QObject::tr("Lead Color"), d);

  _settings.getData(SinePlot, d);
  dialog->addComboItem(SinePlot, page, QObject::tr("Sine Plot"), plotList, d);

  _settings.getData(LeadPlot, d);
  dialog->addComboItem(LeadPlot, page, QObject::tr("Lead Plot"), plotList, d);

  _settings.getData(SineLabel, d);
  dialog->addTextItem(SineLabel, page, QObject::tr("Sine Label"), d, QString());

  _settings.getData(LeadLabel, d);
  dialog->addTextItem(LeadLabel, page, QObject::tr("Lead Label"), d, QString());

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
  HT *o = new HT;
  return ((IndicatorPlugin *) o);
}


