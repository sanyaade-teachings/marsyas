/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

/** 
    \class PeakPeriods2BPM
    \brief PeakPeriods2BPM: convert peaks to beats per minute 

*/

#include "PeakPeriods2BPM.h"

using namespace std;
using namespace Marsyas;

PeakPeriods2BPM::PeakPeriods2BPM(string name):MarSystem("PeakPeriods2BPM",name)
{
  //type_ = "PeakPeriods2BPM";
  //name_ = name;
}


PeakPeriods2BPM::~PeakPeriods2BPM()
{
}


MarSystem* 
PeakPeriods2BPM::clone() const 
{
  return new PeakPeriods2BPM(*this);
}

void
PeakPeriods2BPM::localUpdate()
{
  MRSDIAG("PeakPeriods2BPM.cpp - PeakPeriods2BPM:localUpdate");
  
//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	MarSystem::localUpdate();

  srate_ = getctrl("mrs_real/israte").toReal();

}

void 
PeakPeriods2BPM::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
      
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_/2; t++)
      {
	out(o,2*t) = in(o,2*t);
	out(o,2*t+1) = (mrs_real)(srate_ * 60.0 / in(o, 2*t+1));
      }
  
}







	

	
