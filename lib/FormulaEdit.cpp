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

#include "FormulaEdit.h"
#include "PrefDialog.h"
#include "ta_libc.h"
#include "IndicatorPlugin.h"
#include "PlotLine.h"
#include "BARS.h"
#include "UTIL.h"
#include "../pics/insert.xpm"
#include "../pics/openchart.xpm"
#include "../pics/ok.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"

#include <QLayout>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QHBoxLayout>
#include <QtDebug>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QIcon>


FormulaEdit::FormulaEdit ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(5);
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(5);
  vbox->addLayout(hbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(2);
  hbox->addLayout(tvbox);

  openButton = new QToolButton;
  openButton->setIcon(QIcon(openchart));
  openButton->setToolTip(tr("Open Rule"));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openRule()));
  tvbox->addWidget(openButton);

  addButton = new QToolButton;
  addButton->setIcon(QIcon(insert));
  addButton->setToolTip(tr("Add Function"));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addFunction()));
  tvbox->addWidget(addButton);

  editButton = new QToolButton;
  editButton->setIcon(QIcon(edit));
  editButton->setToolTip(tr("Edit Function"));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editFunction()));
  tvbox->addWidget(editButton);

  deleteButton = new QToolButton;
  deleteButton->setIcon(QIcon(deleteitem));
  deleteButton->setToolTip(tr("Delete Function"));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteFunction()));
  tvbox->addWidget(deleteButton);

  tvbox->addStretch(1);

  // create the formula widget area
  formula = new QListWidget;
  connect(formula, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelected()));
  connect(formula, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(doubleClicked(QListWidgetItem *)));
  hbox->addWidget(formula, 1);

  // plot settings area
  plotBox = new QGroupBox;
  plotBox->setTitle(tr("Plot Parameters"));
  plotBox->setFlat(FALSE);
  plotBox->setCheckable(TRUE);
  plotBox->setChecked(FALSE);
  connect(plotBox, SIGNAL(clicked(bool)), this, SLOT(plotBoxChecked(bool)));
  hbox->addWidget(plotBox);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  plotBox->setLayout(tvbox);

  QColor color("red");  
  colorButton = new ColorButton(this, color);
  connect(colorButton, SIGNAL(valueChanged()), this, SLOT(colorChanged()));
  tvbox->addWidget(colorButton);

  plotLabel = new QLineEdit;
  tvbox->addWidget(plotLabel);
  connect(plotLabel, SIGNAL(textEdited(const QString &)), this, SLOT(plotLabelChanged(const QString &)));

  lineType = new QComboBox;
  PlotLine pl;
  QStringList l;
  pl.getLineTypes(l);
  lineType->addItems(l);
  connect(lineType, SIGNAL(currentIndexChanged(int)), this, SLOT(lineTypeChanged(int)));
  tvbox->addWidget(lineType);

  tvbox->addStretch(1);

  itemSelected();
}

void FormulaEdit::openRule ()
{
/*
  DataBase db;
  QStringList l;
  db.getIndicatorList(l);
  bool ok;
  QString selection = QInputDialog::getItem(this, tr("Select Indicator"), tr("Select indicator to open."), l, 0, FALSE, &ok, 0);
  if (! ok)
    return;

  QString s = name;
  name = selection;
  loadIndicator();
  name = s;
*/
}

void FormulaEdit::doubleClicked (QListWidgetItem *)
{
}

void FormulaEdit::addFunction ()
{
  IndicatorPlugin ip;
  QStringList l;
  ip.getIndicatorList(l);

  bool ok;
  QString function = QInputDialog::getItem(this,
					   QObject::tr("Indicator Selection"),
                                           QObject::tr("Select an indicator:"),
                                           l,
                                           0,
                                           TRUE,
                                           &ok,
                                           0);
  if (! ok)
    return;

  IndicatorParms parms;
  parms.setIndicator(function);

  QStringList vl;
  getVariableList(vl, TRUE);

  if (function == "BARS")
  {
    BARSDialog(parms, vl);
    return;
  }

  if (function == "UTIL")
  {
    UTILDialog(parms, vl);
    return;
  }

  PrefDialog *dialog = new PrefDialog(0);
  dialog->setWindowTitle(QObject::tr("Function Parms"));

  QString vnl = QObject::tr("Variable Name");
  dialog->addTextItem(vnl, function);

  const TA_FuncHandle *handle;
  const TA_FuncInfo *theInfo;
 
  // open a TALIB handle
  TA_RetCode retCode = TA_GetFuncHandle((char *) function.toStdString().c_str(), &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("FormulaEdit::functionDialog:can't open handle");
    return;
  }

  // get info on the function
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::getFormatList:can't get function info");
    return;
  }

  // check for any array inputs
  QString s;
  const TA_InputParameterInfo *inputParms;
  int loop;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    s = QObject::tr("Input") + QString::number(loop + 1);
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    switch (inputParms->type)
    {
      case TA_Input_Real:
        dialog->addComboItem(s, vl, (int) BarData::Close);
        break;
      default:
        break;
    }
  }

  QStringList mal;
  ip.getMATypes(mal);

  // get the input parms
  const TA_OptInputParameterInfo *optInfo;
  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    s = optInfo->displayName;
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        dialog->addDoubleItem(s, (double) optInfo->defaultValue, 0, 99999999);
        break;
      case TA_OptInput_IntegerRange:
        dialog->addIntItem(s, (int) optInfo->defaultValue, 1, 999999);
        break;
      case TA_OptInput_IntegerList:
        dialog->addComboItem(s, mal, (int) optInfo->defaultValue);
        break;
      case TA_OptInput_RealList:
        break;
      default:
        break;
    }
  }

  // get the number of outputs
  if (theInfo->nbOutput > 1)
  {
    s = "Output";
    dialog->addIntItem(s, 1, 1, theInfo->nbOutput);
  }

  int rc = dialog->exec();
  
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  dialog->getText(vnl, s);
  if (s.isEmpty())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Variable name missing."));
    delete dialog;
    return;
  }

  if (vl.indexOf(s) != -1)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate variable name."));
    delete dialog;
    return;
  }
  parms.setVariable(s);

  QString ts;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    s = QObject::tr("Input") + QString::number(loop + 1);
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    switch (inputParms->type)
    {
      case TA_Input_Real:
        dialog->getCombo(s, ts);
        parms.setData(s, ts);
        break;
      default:
        break;
    }
  }

  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    s = optInfo->displayName;
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        ts = QString::number(dialog->getDouble(s));
        parms.setData(s, ts);
        break;
      case TA_OptInput_IntegerRange:
        ts = QString::number(dialog->getInt(s));
        parms.setData(s, ts);
        break;
      case TA_OptInput_IntegerList:
        dialog->getCombo(s, ts);
        parms.setData(s, ts);
        break;
      default:
        break;
    }
  }

  if (theInfo->nbOutput > 1)
  {
    s = "Output";
    ts = QString::number(dialog->getInt(s));
    parms.setData(s, ts);
  }

  parms.getVariable(ts);
  formula->addItem(ts);

  indicator.appendParm(parms);

  delete dialog;
}

void FormulaEdit::editFunction ()
{
  IndicatorPlugin ip;
  IndicatorParms parms;
  indicator.getParm(formula->currentRow(), parms);

  QStringList vl;
  getVariableList(vl, TRUE);

  QString function;
  parms.getIndicator(function);
  if (function == "BARS")
  {
    BARS i;
    i.prefDialog(parms, vl);
    indicator.setParm(formula->currentRow(), parms);
    return;
  }

  if (function == "UTIL")
  {
    UTIL i;
    i.prefDialog(parms, vl);
    indicator.setParm(formula->currentRow(), parms);
    return;
  }

  PrefDialog *dialog = new PrefDialog(0);
  dialog->setWindowTitle(QObject::tr("Function Parms"));

  QString vnl = QObject::tr("Variable Name");
  dialog->addTextItem(vnl, function);

  const TA_FuncHandle *handle;
  const TA_FuncInfo *theInfo;
 
  // open a TALIB handle
  TA_RetCode retCode = TA_GetFuncHandle((char *) function.toStdString().c_str(), &handle);
  if (retCode != TA_SUCCESS)
  {
    qDebug("FormulaEdit::functionDialog:can't open handle");
    return;
  }

  // get info on the function
  retCode = TA_GetFuncInfo(handle, &theInfo);
  if (retCode != TA_SUCCESS)
  {
    qDebug("TALIB::getFormatList:can't get function info");
    return;
  }

  // check for any array inputs
  QString s, s2;
  const TA_InputParameterInfo *inputParms;
  int loop;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    s = QObject::tr("Input") + QString::number(loop + 1);
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    switch (inputParms->type)
    {
      case TA_Input_Real:
        parms.getData(s, s2);
        dialog->addComboItem(s, vl, s2);
        break;
      default:
        break;
    }
  }

  QStringList mal;
  ip.getMATypes(mal);

  // get the input parms
  const TA_OptInputParameterInfo *optInfo;
  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    s = optInfo->displayName;
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        parms.getData(s, s2);
        dialog->addDoubleItem(s, s2.toDouble(), 0, 99999999);
        break;
      case TA_OptInput_IntegerRange:
        parms.getData(s, s2);
        dialog->addIntItem(s, s2.toInt(), 1, 999999);
        break;
      case TA_OptInput_IntegerList:
        parms.getData(s, s2);
        dialog->addComboItem(s, mal, s2);
        break;
      case TA_OptInput_RealList:
        break;
      default:
        break;
    }
  }

  // get the number of outputs
  if (theInfo->nbOutput > 1)
  {
    s = "Output";
    parms.getData(s, s2);
    dialog->addIntItem(s, s2.toInt(), 1, theInfo->nbOutput);
  }

  int rc = dialog->exec();
  
  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  dialog->getText(vnl, s);
  if (s.isEmpty())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Variable name missing."));
    delete dialog;
    return;
  }

  if (vl.indexOf(s) != -1)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate variable name."));
    delete dialog;
    return;
  }
  parms.setVariable(s);

  QString ts;
  for (loop = 0; loop < (int) theInfo->nbInput; loop++ )
  {
    s = QObject::tr("Input") + QString::number(loop + 1);
    TA_GetInputParameterInfo(theInfo->handle, loop, &inputParms);
    switch (inputParms->type)
    {
      case TA_Input_Real:
        dialog->getCombo(s, ts);
        parms.setData(s, ts);
        break;
      default:
        break;
    }
  }

  for (loop = 0; loop < (int) theInfo->nbOptInput; loop++ )
  {
    TA_GetOptInputParameterInfo(theInfo->handle, loop, &optInfo);
    s = optInfo->displayName;
    switch (optInfo->type)
    {
      case TA_OptInput_RealRange:
        ts = QString::number(dialog->getDouble(s));
        parms.setData(s, ts);
        break;
      case TA_OptInput_IntegerRange:
        ts = QString::number(dialog->getInt(s));
        parms.setData(s, ts);
        break;
      case TA_OptInput_IntegerList:
        dialog->getCombo(s, ts);
        parms.setData(s, ts);
        break;
      default:
        break;
    }
  }

  if (theInfo->nbOutput > 1)
  {
    s = "Output";
    ts = QString::number(dialog->getInt(s));
    parms.setData(s, ts);
  }

  parms.getVariable(ts);
  formula->addItem(ts);

  indicator.appendParm(parms);

  delete dialog;
}

void FormulaEdit::deleteFunction ()
{
  QListWidgetItem *item = formula->currentItem();
  if (! item)
    return;

  int row = formula->currentRow();
  indicator.removeParm(row);
  
  delete item;

  itemSelected();
}

void FormulaEdit::getVariableList (QStringList &l, bool flag)
{
  l.clear();

  if (flag)
  {
    QString s;
    BarData bd(s);
    bd.getInputFields(l);
  }

  int loop;
  for (loop = 0; loop < formula->count(); loop++)
  {
    QListWidgetItem *item = formula->item(loop);
    l.append(item->text());
  }
}

void FormulaEdit::itemSelected ()
{
  // actions 3,4
  bool flag = FALSE;
  QListWidgetItem *item = formula->currentItem();
  if (item)
    flag = TRUE;

  editButton->setEnabled(flag);
  deleteButton->setEnabled(flag);

  if (! flag)
  {
    plotBox->setChecked(FALSE);
    return;
  }

  IndicatorParms parms;
  indicator.getParm(formula->currentRow(), parms);

  if (! parms.getPlot())
  {
    plotBox->setChecked(FALSE);
    return;
  }

  plotBox->setChecked(TRUE);

  QColor color("red");
  parms.getColor(color);
  colorButton->setColor(color);

  QString s;
  parms.getLabel(s);
  plotLabel->setText(s);

  parms.getLineType(s);
  lineType->setCurrentIndex(lineType->findText(s, Qt::MatchExactly));
}

void FormulaEdit::plotBoxChecked (bool status)
{
  QListWidgetItem *item = formula->currentItem();
  if (! item)
    return;

  IndicatorParms parms;
  indicator.getParm(formula->currentRow(), parms);

  if (status)
  {
    item->setIcon(QIcon(ok));

    QColor c;
    parms.getColor(c);
    if (! c.isValid())
    {
      c.setNamedColor("red");
      parms.setColor(c);
    }
    colorButton->setColor(c);

    QString s;
    parms.getLabel(s);
    if (! s.length())
    {
      s = "Label";
      parms.setLabel(s);
    }
    plotLabel->setText(s);

    parms.getLineType(s);
    if (! s.length())
    {
      s = "Line";
      parms.setLineType(s);
    }
    lineType->setCurrentIndex(lineType->findText(s, Qt::MatchExactly));
  }
  else
    item->setIcon(QIcon());

  parms.setPlot((int) status);

  indicator.setParm(formula->currentRow(), parms);
}

void FormulaEdit::plotLabelChanged (const QString &t)
{
  IndicatorParms parms;
  indicator.getParm(formula->currentRow(), parms);
  QString s = t;
  parms.setLabel(s);
  indicator.setParm(formula->currentRow(), parms);
}

void FormulaEdit::lineTypeChanged (int)
{
  IndicatorParms parms;
  indicator.getParm(formula->currentRow(), parms);
  QString s = lineType->currentText();
  parms.setLineType(s);
  indicator.setParm(formula->currentRow(), parms);
}

void FormulaEdit::colorChanged ()
{
  IndicatorParms parms;
  indicator.getParm(formula->currentRow(), parms);
  QColor c;
  colorButton->getColor(c);
  parms.setColor(c);
  indicator.setParm(formula->currentRow(), parms);
}

void FormulaEdit::setIndicator (Indicator  &i)
{
  indicator = i;

  formula->clear();

  int loop;
  for (loop = 0; loop < indicator.count(); loop++)
  {
    IndicatorParms parms;
    indicator.getParm(loop, parms);
    
    QString s;
    parms.getVariable(s);
    QListWidgetItem *item = new QListWidgetItem(s, formula);

    if (parms.getPlot())
      item->setIcon(QIcon(ok));
  }
}

void FormulaEdit::getIndicator (Indicator &i)
{
  i = indicator;
}

int FormulaEdit::getLines ()
{
  return formula->count();
}

//**********************************************************************
//**********************************************************************
//**********************************************************************

void FormulaEdit::BARSDialog (IndicatorParms &parms, QStringList &vl)
{
  BARS i;
  i.prefDialog(parms, vl);

  QString s;
  parms.getVariable(s);
  formula->addItem(s);
  indicator.appendParm(parms);
}

void FormulaEdit::UTILDialog (IndicatorParms &parms, QStringList &vl)
{
  UTIL i;
  i.prefDialog(parms, vl);

  QString s;
  parms.getVariable(s);
  formula->addItem(s);
  indicator.appendParm(parms);
}

