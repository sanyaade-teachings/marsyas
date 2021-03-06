/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYAS_NORMALIZEABS_H
#define MARSYAS_NORMALIZEABS_H

#include "MarSystem.h"

namespace Marsyas
{
/**
	\class NormalizeAbs
	\ingroup Processing
	\brief Normalize each observation channel by absolute value

	The Norm and Normalize MarSystems both change values based
on the mean and std.  This MarSystem changes values based on the
maximum absolute value.

	Controls:
	- \b mrs_real/target [w] : sets the absolute target maximum value
*/

class marsyas_EXPORT NormalizeAbs: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


  /// MarControlPtr for the gain control
  MarControlPtr ctrl_target_;

public:
  /// NormalizeAbs constructor.
  NormalizeAbs(std::string name);

  /// NormalizeAbs copy constructor.
  NormalizeAbs(const NormalizeAbs& a);

  /// NormalizeAbs destructor.
  ~NormalizeAbs();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_NORMALIZEABS_H

