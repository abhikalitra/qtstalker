/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "PlotLine.h"
#include "QSMath.h"
#include "BarData.h"
#include "FormulaInput.h"
#include "PrefDialog.h"
#include "../src/newchart.xpm"
#include "../src/delete.xpm"
#include "../src/edit.xpm"
#include <qlayout.h>

#define BUTTON_SIZE 24

FormulaEdit::FormulaEdit (QWidget *w) : QWidget(w)
{
  PlotLine *pl = new PlotLine;
  lineTypes = pl->getLineTypes();
  delete pl;
  
  QSMath *m = new QSMath;
  maTypeList = m->getMATypes();
  opList = m->getOperatorTypes ();
  delete m;
  
  functionList.append(QObject::tr("AD"));
  functionList.append(QObject::tr("ADD"));
  functionList.append(QObject::tr("ADX"));
  functionList.append(QObject::tr("BB"));
  functionList.append(QObject::tr("CCI"));
  functionList.append(QObject::tr("COMP"));
  functionList.append(QObject::tr("DIV"));
  functionList.append(QObject::tr("MA"));
  functionList.append(QObject::tr("MDI"));
  functionList.append(QObject::tr("MF"));
  functionList.append(QObject::tr("MOM"));
  functionList.append(QObject::tr("MUL"));
  functionList.append(QObject::tr("NVI"));
  functionList.append(QObject::tr("OBV"));
  functionList.append(QObject::tr("OSC"));
  functionList.append(QObject::tr("PC"));
  functionList.append(QObject::tr("PDI"));
  functionList.append(QObject::tr("PER"));
  functionList.append(QObject::tr("PVI"));
  functionList.append(QObject::tr("PVT"));
  functionList.append(QObject::tr("REF"));
  functionList.append(QObject::tr("ROC"));
  functionList.append(QObject::tr("RSI"));
  functionList.append(QObject::tr("SAR"));
  functionList.append(QObject::tr("SD"));
  functionList.append(QObject::tr("STOCH"));
  functionList.append(QObject::tr("SUB"));
  functionList.append(QObject::tr("TP"));
  functionList.append(QObject::tr("TR"));
  functionList.append(QObject::tr("VFI"));
  functionList.append(QObject::tr("VOLR"));
  functionList.append(QObject::tr("WAD"));
  functionList.append(QObject::tr("WILLR"));
  
  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setMargin(0);
  hbox->setSpacing(1);
  
  list = new QTable(0, 2, this);
  list->setSelectionMode(QTable::Single);
  list->setSorting(FALSE);
  list->horizontalHeader()->setLabel(0, tr("Function"));
  list->horizontalHeader()->setLabel(1, tr("Plot"));
  list->setColumnWidth(0, 275);
  list->setColumnWidth(1, 50);
  list->setColumnReadOnly(0, TRUE);
  hbox->addWidget(list);
  
  toolbar = new Toolbar(this, 30, 30, TRUE);
  hbox->addWidget(toolbar);

  toolbar->addButton("add", newchart, tr("Add"));
  QObject::connect(toolbar->getButton("add"), SIGNAL(clicked()), this, SLOT(addItem()));
  
  toolbar->addButton("edit", edit, tr("Edit"));
  QObject::connect(toolbar->getButton("edit"), SIGNAL(clicked()), this, SLOT(editItem()));
  
  toolbar->addButton("delete", deleteitem, tr("Delete"));
  QObject::connect(toolbar->getButton("delete"), SIGNAL(clicked()), this, SLOT(deleteItem()));
}

FormulaEdit::~FormulaEdit ()
{
}

void FormulaEdit::addItem ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("ADD Function"));
  dialog->createPage (tr("Functions"));
  dialog->addComboItem(tr("Functions"), tr("Functions"), functionList, 0);
  int rc = dialog->exec();
  if (rc != QDialog::Accepted)
    return;
  
  QString type = functionList[dialog->getComboIndex(tr("Functions"))];
  delete dialog;
  
  QString s = getDialog(type, QString::null);
  if (! s.length())
    return;

  list->setNumRows(list->numRows() + 1);
  
  list->setText(list->numRows() - 1, 0, s);
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  check->setChecked(FALSE);
  list->setItem(list->numRows() - 1, 1, check);
}

void FormulaEdit::editItem ()
{
  QString s = getDialog(QString::null, list->text(list->currentRow(), 0));
  if (! s.length())
    return;
  list->setText(list->currentRow(), 0, s);
}

void FormulaEdit::deleteItem ()
{
  list->removeRow(list->currentRow());
}

void FormulaEdit::setLine (QString d)
{
  QStringList l = QStringList::split("|", d, FALSE);
  if (l.count() != 2)
  {
    qDebug("FormulaEdit::setLine: invalid field count");
    return;
  }

  list->setNumRows(list->numRows() + 1);
    
  list->setText(list->numRows() - 1, 0, l[0]);
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  if (! l[1].toInt())
    check->setChecked(FALSE);
  else
    check->setChecked(TRUE);
  list->setItem(list->numRows() - 1, 1, check);
}

QString FormulaEdit::getLine (int i)
{
  QString s = list->text(i, 0);
  s.append("|");
  QCheckTableItem *check = (QCheckTableItem *) list->item(i, 1);
  s.append(QString::number(check->isChecked()));
  return s;
}

int FormulaEdit::getLines ()
{
  return list->numRows();
}

QString FormulaEdit::getFormula (int i)
{
  return list->text(i, 0);
}

QString FormulaEdit::getPlot (int i)
{
  QCheckTableItem *check = (QCheckTableItem *) list->item(i, 1);
  return QString::number(check->isChecked());
}

QString FormulaEdit::getDialog (QString t, QString f)
{
  bool flag = FALSE;
  int rc = 0;
  QStringList l;
  QStringList format;
  QString type;
  
  if (t.length())
    type = t;
  else
  {
    flag = TRUE;
    l = QStringList::split(",", f, FALSE);
    type = l[0];
  }
  
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Function Dialog"));
  dialog->createPage (tr("Parms"));
  if (flag)
  {
    dialog->addColorItem(tr("Color"), tr("Parms"), l[l.count() - 3]);
    dialog->addTextItem(tr("Label"), tr("Parms"), l[l.count() - 1]);
    dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, l[l.count() - 2]);
  }
  else
  {
    dialog->addColorItem(tr("Color"), tr("Parms"), QColor("red"));
    dialog->addTextItem(tr("Label"), tr("Parms"), tr("label"));
    dialog->addComboItem(tr("Line Type"), tr("Parms"), lineTypes, tr("Line"));
  }
  
  format.append(type);

  while (1)
  {
    if (! type.compare(QObject::tr("AD")) ||
        ! type.compare(QObject::tr("OBV")) ||
	! type.compare(QObject::tr("NVI")) ||
	! type.compare(QObject::tr("PER")) ||
	! type.compare(QObject::tr("PVI")) ||
	! type.compare(QObject::tr("PVT")) ||
	! type.compare(QObject::tr("TP")) ||
	! type.compare(QObject::tr("TR")) ||
	! type.compare(QObject::tr("WAD")))
    {
      rc = dialog->exec();
      break;
    }
    
    if (! type.compare(QObject::tr("ADD")) ||
        ! type.compare(QObject::tr("DIV")) ||
	! type.compare(QObject::tr("MUL")) ||
	! type.compare(QObject::tr("SUB")))
    {
      if (flag)
      {
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->currentRow(), l[1]);
        dialog->addFormulaInputItem(tr("Data2"), tr("Parms"), TRUE, list->currentRow(), l[2]);
      }
      else
      {
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->numRows(), tr("Close"));
        dialog->addFormulaInputItem(tr("Data2"), tr("Parms"), TRUE, list->numRows(), tr("Close"));
      }
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
      {
        format.append(dialog->getFormulaInput(tr("Data1")));
        format.append(dialog->getFormulaInput(tr("Data2")));
      }
      break;
    }
    
    if (! type.compare(QObject::tr("ADX")))
    {
      if (flag)
      {
        dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, l[1]);
        dialog->addIntItem(tr("Period"), tr("Parms"), l[2].toInt(), 1, 99999999);
      }
      else
      {
        dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, tr("SMA"));
        dialog->addIntItem(tr("Period"), tr("Parms"), 1, 1, 99999999);
      }
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
      {
        format.append(dialog->getCombo(tr("MA Type")));
        format.append(QString::number(dialog->getInt(tr("Period"))));
      }
      break;
    }
      
    if (! type.compare(QObject::tr("BB")))
    {
      QStringList l2;
      l2.append(tr("Lower"));
      l2.append(tr("Upper"));
      
      if (flag)
      {
        dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, l[1]);
        dialog->addIntItem(tr("Period"), tr("Parms"), l[2].toInt(), 1, 99999999);
        dialog->addFloatItem(tr("Deviation"), tr("Parms"), l[3].toDouble(), 0, 99999999);
        dialog->addComboItem(tr("Band"), tr("Parms"), l2, l[4].toInt());
      }
      else
      {
        dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, tr("SMA"));
        dialog->addIntItem(tr("Period"), tr("Parms"), 20, 1, 99999999);
        dialog->addFloatItem(tr("Deviation"), tr("Parms"), 2, 0, 99999999);
        dialog->addComboItem(tr("Band"), tr("Parms"), l2, 0);
      }
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
      {
        format.append(dialog->getCombo(tr("MA Type")));
        format.append(QString::number(dialog->getInt(tr("Period"))));
        format.append(QString::number(dialog->getFloat(tr("Deviation"))));
        format.append(QString::number(dialog->getComboIndex(tr("Band"))));
      }
      break;
    }
  
    if (! type.compare(QObject::tr("CCI")) ||
        ! type.compare(QObject::tr("MDI")) ||
	! type.compare(QObject::tr("MF")) ||
	! type.compare(QObject::tr("MOM")) ||
	! type.compare(QObject::tr("PDI")) ||
	! type.compare(QObject::tr("VFI")) ||
	! type.compare(QObject::tr("VOLR")) ||
	! type.compare(QObject::tr("WILLR")))
    {
      if (flag)
        dialog->addIntItem(tr("Period"), tr("Parms"), l[1].toInt(), 1, 99999999);
      else
        dialog->addIntItem(tr("Period"), tr("Parms"), 20, 1, 99999999);
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
        format.append(QString::number(dialog->getInt(tr("Period"))));
      break;
    }

    if (! type.compare(QObject::tr("COMP")))
    {
      if (flag)
      {
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->currentRow(), l[1]);
        dialog->addComboItem(tr("Operator"), tr("Parms"), opList, l[2]);
        dialog->addFormulaInputItem(tr("Data2"), tr("Parms"), TRUE, list->currentRow(), l[3]);
      }
      else
      {
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->numRows(), tr("Close"));
        dialog->addComboItem(tr("Operator"), tr("Parms"), opList, 0);
        dialog->addFormulaInputItem(tr("Data2"), tr("Parms"), TRUE, list->numRows(), tr("Close"));
      }
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
      {
        format.append(dialog->getFormulaInput(tr("Data1")));
        format.append(dialog->getCombo(tr("Operator")));
        format.append(dialog->getFormulaInput(tr("Data2")));
      }      
      break;
    }

    if (! type.compare(QObject::tr("MA")))
    {
      if (flag)
      {
        dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, l[1]);
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->currentRow(), l[2]);
        dialog->addIntItem(tr("Period"), tr("Parms"), l[3].toInt(), 1, 99999999);
      }
      else
      {
        dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, tr("SMA"));
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->numRows(), tr("Close"));
        dialog->addIntItem(tr("Period"), tr("Parms"), 20, 1, 99999999);
      }
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
      {
        format.append(dialog->getCombo(tr("MA Type")));
        format.append(dialog->getFormulaInput(tr("Data1")));
        format.append(QString::number(dialog->getInt(tr("Period"))));
      }      
      break;
    }
    
    if (! type.compare(QObject::tr("OSC")))
    {
      if (flag)
      {
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->currentRow(), l[1]);
        dialog->addComboItem(tr("Fast MA Type"), tr("Parms"), maTypeList, l[2]);
        dialog->addComboItem(tr("Slow MA Type"), tr("Parms"), maTypeList, l[3]);
        dialog->addIntItem(tr("Fast Period"), tr("Parms"), l[4].toInt(), 1, 99999999);
        dialog->addIntItem(tr("Slow Period"), tr("Parms"), l[5].toInt(), 1, 99999999);
      }
      else
      {
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->numRows(), tr("Close"));
        dialog->addComboItem(tr("Fast MA Type"), tr("Parms"), maTypeList, tr("SMA"));
        dialog->addComboItem(tr("Slow MA Type"), tr("Parms"), maTypeList, tr("SMA"));
        dialog->addIntItem(tr("Fast Period"), tr("Parms"), 10, 1, 99999999);
        dialog->addIntItem(tr("Slow Period"), tr("Parms"), 20, 1, 99999999);
      }
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
      {
        format.append(dialog->getFormulaInput(tr("Data1")));
        format.append(dialog->getCombo(tr("Fast MA Type")));
        format.append(dialog->getCombo(tr("Slow MA Type")));
        format.append(QString::number(dialog->getInt(tr("Fast Period"))));
        format.append(QString::number(dialog->getInt(tr("Slow Period"))));
      }      
      break;
    }
  
    if (! type.compare(QObject::tr("PC")) ||
        ! type.compare(QObject::tr("REF")) ||
	! type.compare(QObject::tr("ROC")) ||
	! type.compare(QObject::tr("RSI")) ||
	! type.compare(QObject::tr("SD")))
    {
      if (flag)
      {
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->currentRow(), l[1]);
        dialog->addIntItem(tr("Period"), tr("Parms"), l[2].toInt(), 0, 99999999);
      }
      else
      {
        dialog->addFormulaInputItem(tr("Data1"), tr("Parms"), FALSE, list->numRows(), tr("Close"));
        dialog->addIntItem(tr("Period"), tr("Parms"), 10, 0, 99999999);
      }
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
      {
        format.append(dialog->getFormulaInput(tr("Data1")));
        format.append(QString::number(dialog->getInt(tr("Period"))));
      }      
      break;
    }
  
    if (! type.compare(QObject::tr("SAR")))
    {
      if (flag)
      {
        dialog->addFloatItem(tr("Initial"), tr("Parms"), l[1].toDouble(), 0, 99999999);
        dialog->addFloatItem(tr("Add"), tr("Parms"), l[2].toDouble(), 0, 99999999);
        dialog->addFloatItem(tr("Limit"), tr("Parms"), l[3].toDouble(), 0, 99999999);
      }
      else
      {
        dialog->addFloatItem(tr("Initial"), tr("Parms"), 0.02, 0, 99999999);
        dialog->addFloatItem(tr("Add"), tr("Parms"), 0.02, 0, 99999999);
        dialog->addFloatItem(tr("Limit"), tr("Parms"), 0.2, 0, 99999999);
      }
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
      {
        format.append(QString::number(dialog->getFloat(tr("Initial"))));
        format.append(QString::number(dialog->getFloat(tr("Add"))));
        format.append(QString::number(dialog->getFloat(tr("Limit"))));
      }      
      break;
    }
  
    if (! type.compare(QObject::tr("STOCH")))
    {
      if (flag)
      {
        dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, l[1]);
        dialog->addIntItem(tr("%D Period"), tr("Parms"), l[2].toInt(), 1, 99999999);
        dialog->addIntItem(tr("%K Period"), tr("Parms"), l[3].toInt(), 1, 99999999);
      }
      else
      {
        dialog->addComboItem(tr("MA Type"), tr("Parms"), maTypeList, tr("SMA"));
        dialog->addIntItem(tr("%D Period"), tr("Parms"), 3, 1, 99999999);
        dialog->addIntItem(tr("%K Period"), tr("Parms"), 3, 1, 99999999);
      }
      rc = dialog->exec();
      if (rc == QDialog::Accepted)
      {
        format.append(dialog->getCombo(tr("MA Type")));
        format.append(QString::number(dialog->getInt(tr("%D Period"))));
        format.append(QString::number(dialog->getInt(tr("%K Period"))));
      }      
      break;
    }
  
    break;
  }

  format.append(dialog->getColor(tr("Color")).name());
  format.append(dialog->getCombo(tr("Line Type")));
  format.append(dialog->getText(tr("Label")));

  delete dialog;
  
  if (rc != QDialog::Accepted)
    return QString::null;
  else
    return format.join(",");
}

