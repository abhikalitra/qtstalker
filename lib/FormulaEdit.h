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

#ifndef FORMULAEDIT_HPP
#define FORMULAEDIT_HPP

#include <QString>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QList>
#include <QLineEdit>
#include <QComboBox>
#include <QToolButton>

#include "ColorButton.h"
#include "Indicator.h"


class FormulaEdit : public QWidget
{
  Q_OBJECT

  public:
    FormulaEdit ();
    void getVariableList (QStringList &, bool);
    void BARSDialog (IndicatorParms &, QStringList &);
    void UTILDialog (IndicatorParms &, QStringList &);
    void setIndicator (Indicator &);
    void getIndicator (Indicator &);
    int getLines ();

  public slots:
    void openRule ();
    void doubleClicked (QListWidgetItem *);
    void addFunction ();
    void editFunction ();
    void deleteFunction ();
    void itemSelected ();
    void plotBoxChecked (bool);
    void plotLabelChanged (const QString &);
    void lineTypeChanged (int);
    void colorChanged ();

  protected:
    QListWidget *formula;
    QStringList functionList;
    QStringList formulaList;
    Indicator indicator;
    ColorButton *colorButton;
    QLineEdit *plotLabel;
    QGroupBox *plotBox;
    QComboBox *lineType;
    QToolButton *openButton;
    QToolButton *addButton;
    QToolButton *editButton;
    QToolButton *deleteButton;
};

#endif
