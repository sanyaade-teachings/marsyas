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
    \class NoiseGate
    \brief Multiply input realvec with NoiseGate

   Simple MarSystem example. Just multiply the values of the input realvec
with NoiseGate and put them in the output vector. This object can be used 
as a prototype template for building more complicated MarSystems. 
*/

/* How to use the Limiter Marsystem

	The system can be setup using the marsystem manager
		series->addMarSystem(mng.create("Noisegate", "noisegate"));

	Options: Threshold can be set to any value between 0 and 1
		0 <= thresh <= 1.0	
		series->updctrl("NoiseGate/noisegate/mrs_real/thresh", thresh);
		
	Release threshold is the value where the gate is in the on possition.  This value is usually
	higher than the Threshold value.
		0 <= release <= 1.0
		series->updctrl("NoiseGate/noisegate/mrs_real/release", release);
	
	Rolloff time is the length of time desired for rolloff into noise gate.  This feature allows
	for a smooth transition between the gate being on and the off:
	rolloff = 1 - exp(-2.2*T/t_rolloff)
	where rolloff = rolloff time, T = sampling period, t_rolloff = time parameter 0.001 < t_rolloff < 1 sec
		series->updctrl("NoiseGate/noisegate/mrs_real/at", t_rolloff);
		
	Attack time can be calculated using the following formula: at = 1 - exp(-2.2*T/t_AT)
	where at = attack time, T = sampling period, t_AT = time parameter 0.00016 < t_AT < 2.6 sec
		series->updctrl("NoiseGate/noisegate/mrs_real/at", t_AT);
	
	Release time can be calculated similar to attack time: rt = 1 - exp(-2.2*T/t_RT)
	where rt = release time, T = sampling period, t_RT = time parameter 0.001 < t_RT < 5.0 msec
		series->updctrl("NoiseGate/noisegate/mrs_real/rt", t_RT);
		
	Slope factor: 0 < slope <= 1.0
		series->updctrl("NoiseGate/noisegate/mrs_real/rt", slope);
	
*/

#include "NoiseGate.h"

using namespace std;
using namespace Marsyas;

NoiseGate::NoiseGate(string name):MarSystem("NoiseGate",name)
{
  //type_ = "NoiseGate";
  //name_ = name;

  state_ = 1.0;
  xdprev_ = 0.0;
  alpha_ = 0.0;
  gainsprev_ = 1.0;

	addControls();
}


NoiseGate::~NoiseGate()
{
}


MarSystem* 
NoiseGate::clone() const 
{
  return new NoiseGate(*this);
}

void 
NoiseGate::addControls()
{
  addctrl("mrs_real/thresh", 0.1);
  addctrl("mrs_real/release", 0.5);
  addctrl("mrs_real/rolloff", .130);
  addctrl("mrs_real/at", 0.0001);
  addctrl("mrs_real/rt", 0.130);
  addctrl("mrs_real/slope", 1.0);
}

void
NoiseGate::localUpdate()
{
  MRSDIAG("NoiseGate.cpp - NoiseGate:localUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
	//defaultUpdate(); [!]
	inSamples_ = getctrl("mrs_natural/inSamples").toNatural();
  
	xd_.create(inSamples_);
  gains_.create(inSamples_);
}


void 
NoiseGate::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  mrs_real thresh = getctrl("mrs_real/thresh").toReal();
  mrs_real release = getctrl("mrs_real/release").toReal();
  mrs_real rolloff = getctrl("mrs_real/rolloff").toReal();
  mrs_real at = getctrl("mrs_real/at").toReal();
  mrs_real rt = getctrl("mrs_real/rt").toReal();
  mrs_real slope = getctrl("mrs_real/slope").toReal();
  
    // calculate rolloff, at and rt time
  at = 1 - exp(-2.2/(22050*at));
  rt = 1 - exp(-2.2/(22050*rt));
	
  for (o = 0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
		// Calculates the current amplitude of signal and incorporates
		// the at and rt times into xd(o,t)		
		alpha_ = fabs(in(o,t)) - xdprev_;
		if (alpha_ < 0)
		{
			alpha_ = 0;
		}
		xdprev_=xdprev_*(1-rt)+at*alpha_;	
		
		if (state_ == 1.0)
		  {
		  if (xdprev_ < thresh)
		    {
			gains_(o,t) = gainsprev_*rolloff;
			state_ = 0.0;
			}
		  else
		    {
			gains_(o,t) = 1;
			}
		  }
		else
		  {
			if (xdprev_ < release)
			  {
			  gains_(o,t) = gainsprev_*rolloff;
			  // rolloff time
			  }
		    else
			if (xdprev_ > release)
			  {
			  gains_(o,t) = 1.0;
			  state_ = 1.0;
			  }
			else
			  {
			  gains_(o,t) = 0.0;
			  }
		  }				
		
		gainsprev_ = gains_(o,t);
	    out(o,t) =  gainsprev_ * in(o,t);
		
      }
}







	
