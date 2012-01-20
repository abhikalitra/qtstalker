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

#ifndef TYPE_OPERATOR_HPP
#define TYPE_OPERATOR_HPP

#include "Type.h"

class TypeOperator : public Type
{
  public:
    enum Key
    {
      _LESS_THAN,
      _LESS_THAN_EQUAL,
      _EQUAL,
      _NOT_EQUAL,
      _GREATER_THAN_EQUAL,
      _GREATER_THAN
    };
    
    TypeOperator ();
    int test (double val, TypeOperator::Key, double val2);
};

#endif
