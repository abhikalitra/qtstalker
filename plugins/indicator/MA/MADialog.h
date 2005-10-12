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
#include <qlineedit.h>
#include <qlabel.h>
#include "ColorButton.h"

class MADialog : public QTabDialog
{
  Q_OBJECT
  
  public:
    MADialog (QWidget *, QString);
    ~MADialog ();
    void setColor (QColor);
    QColor getColor ();
    void setMAType (QStringList, int);
    int getMAType ();
    void setLabel (QString);
    QString getLabel ();
    void setLineType (QStringList, int);
    int getLineType ();
    void setPeriod (int);
    int getPeriod ();
    void setInput (QStringList, int);
    int getInput ();
    void setFreq (double);
    double getFreq ();
    void setWidth (double);
    double getWidth ();
    
  public slots:
    void help ();
    void typeChanged (int);
    
  private:
    QComboBox *maType;
    QComboBox *lineType;
    QComboBox *input;
    QLineEdit *text;
    QLineEdit *freq;
    QLineEdit *width;
    QSpinBox *period;
    ColorButton *colorButton;
    QString helpFile;
    QLabel *freqLabel;
    QLabel *widthLabel;
};

    

