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
    \class Limiter
    \brief Multiply input realvec with Limiter

   Simple MarSystem example. Just multiply the values of the input realvec
with Limiter and put them in the output vector. This object can be used 
as a prototype template for building more complicated MarSystems. 
*/

/* How to use the Limiter Marsystem

	The system can be setup using the marsystem manager
		series->addMarSystem(mng.create("Limiter", "limiter"));

	Options: Threshold can be set to any value between 0 and 1
	Limiter		0 <= thresh <= 1.0	
		series->updctrl("Limiter/limiter/mrs_real/thresh", thresh);
		
	Attack time can be calculated using the following formula: at = 1 - exp(-2.2*T/t_AT)
	where at = attack time, T = sampling period, t_AT = time parameter 0.00016 < t_AT < 2.6 sec
		series->updctrl("Limiter/limiter/mrs_real/at", t_AT);
	
	Release time can be calculated similar to at time: rt = 1 - exp(-2.2*T/t_RT)
	where rt = rt time, T = sampling period, t_RT = time parameter 0.001 < t_RT < 5.0 msec
		series->updctrl("Limiter/limiter/mrs_real/rt", t_RT);
		
	Slope factor: 0 < slope <= 1.0
		series->updctrl("NoiseGate/noisegate/mrs_real/rt", slope);	
*/

#include "Limiter.h"

using namespace std;
using namespace Marsyas;

Limiter::Limiter(string name):MarSystem("Limiter",name)
{
  //type_ = "Limiter";
  //name_ = name;
  
	xdprev_ = 0.0;
  alpha_ = 0.0;

	addControls();
}


Limiter::~Limiter()
{
}


MarSystem* 
Limiter::clone() const 
{
  return new Limiter(*this);
}

void 
Limiter::addControls()
{
  addctrl("mrs_real/thresh", 1.0);
  addctrl("mrs_real/at", 0.0001);
  addctrl("mrs_real/rt", 0.130);
  addctrl("mrs_real/slope", 1.0);
}


void
Limiter::localUpdate()
{
  MRSDIAG("Limiter.cpp - Limiter:localUpdate");
  
//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
// 	setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
	MarSystem::localUpdate(); //default localUpdate as defined at parent MarSystem class...
  
	//defaultUpdate(); [!]
	inSamples_ = getctrl("mrs_natural/inSamples").toNatural();

  xd_.create(inSamples_);
  gains_.create(inSamples_);
}


void 
Limiter::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  mrs_real thresh = getctrl("mrs_real/thresh").toReal();
  mrs_real at = getctrl("mrs_real/at").toReal();
  mrs_real rt = getctrl("mrs_real/rt").toReal();
  mrs_real slope = getctrl("mrs_real/slope").toReal();
  
  // calculate at and rt time
  at = 1 - exp(-2.2/(22050*at));
  rt = 1 - exp(-2.2/(22050*rt));
    
  for (o = 0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
		// Calculates the current amplitude of signal and incorporates
		// the at and rt times into xd(o,t)
		alpha_ = fabs(in(o,t)) - xdprev_;
		
		if (alpha_<0)
		{
		   alpha_ = 0;
		}
		
		xd_(o,t)=xdprev_*(1-rt)+at*alpha_;
		xdprev_ = xd_(o,t);
		
		// Limiter
		if (xd_(o,t) > thresh)
		{
		  gains_(o,t) = pow((mrs_real)10.0,-slope*(log10(xd_(o,t))-log10(thresh)));
		  //  linear calculation of gains_ = 10^(-Limiter Slope * (current value - Limiter Threshold))
		}
		else
		{
		  gains_(o,t) = 1;
		}
		
	    out(o,t) =  gains_(o,t) * in(o,t);
      }
}







	
