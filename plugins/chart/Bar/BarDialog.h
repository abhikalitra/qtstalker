/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include <qtabdialog.h>
#include <qcombobox.h>
#include <qcolor.h>
#include <qspinbox.h>
#include "FormulaEdit.h"
#include "ColorButton.h"

class BarDialog : public QTabDialog
{
  Q_OBJECT
  
  public:
    BarDialog (QString);
    ~BarDialog ();
    QString getText ();
    void setLine (QString);
    void createMainPage ();
    void createBarPage ();
    void createPaintBarPage ();
    void setBarColors (QColor bu, QColor bd, QColor bn);
    QColor getBarUpColor ();
    QColor getBarDownColor ();
    QColor getBarNeutralColor ();
    void setPaintBarColors (QColor bu, QColor bd);
    QColor getPaintUpColor ();
    QColor getPaintDownColor ();
    void setSpacing (int);
    int getSpacing ();
    void setStyle (QString);
    QString getStyle ();
    
  public slots:
    void help ();
    
  private:
    FormulaEdit *list;
    QComboBox *style;
    QSpinBox *spacing;
    ColorButton *paintDownButton;
    ColorButton *paintUpButton;
    ColorButton *barDownButton;
    ColorButton *barUpButton;
    ColorButton *barNeutralButton;
    QString helpFile;
};

    

