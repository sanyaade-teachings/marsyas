// Basic Audio Processing checks
#include "common-reg.h"

void
basic_vibrato(string infile, string outfile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	addSource( pnet, infile );
	pnet->addMarSystem(mng.create("Vibrato", "vib"));
	addDest( pnet, outfile);

	pnet->updctrl("Vibrato/vib/mrs_real/mod_freq", 10.0);
	while (pnet->getctrl("mrs_bool/notEmpty")->toBool())
	{
		pnet->tick();
	}
	delete pnet;
}


void
basic_pitch(string infile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	// sets up SoundFileSource, links notEmpty, and sets srate
	addSource( pnet, infile );

	pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
	pnet->addMarSystem(mng.create("PitchPraat", "pitch"));

	mrs_real lowPitch = 50;
	mrs_real highPitch = 100;
	mrs_real lowFreq = pitch2hertz(lowPitch);
	mrs_real highFreq = pitch2hertz(highPitch);
	// note the reversed order!
	mrs_natural lowSamples = hertz2samples(highFreq, srate);
	mrs_natural highSamples = hertz2samples(lowFreq, srate);

	pnet->updctrl("PitchPraat/pitch/mrs_natural/lowSamples", lowSamples);
	pnet->updctrl("PitchPraat/pitch/mrs_natural/highSamples", highSamples);

	//  The window should be just long enough to contain three periods
	//  (for pitch detection) of MinimumPitch. E.g. if MinimumPitch is
	//  75 Hz, the window length is 40 ms and padded with zeros to reach
	//  a power of two.
	mrs_real windowSize = 3.0 / lowPitch * srate;
	pnet->updctrl("mrs_natural/inSamples", 512);
	pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize",
	              powerOfTwo(windowSize));

	while (pnet->getctrl("mrs_bool/notEmpty")->toBool())
	{
		pnet->tick();
		const realvec& processedData =
		    pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
		cout<<samples2hertz( processedData(1), srate)<<" ";
	}
	cout<<endl;
	delete pnet;
}

void
basic_windowing(string infile, string outfile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	addSource( pnet, infile );
	pnet->addMarSystem(mng.create("Windowing", "win"));
	pnet->updctrl("Windowing/win/mrs_string/type", "Hanning");
	addDest( pnet, outfile);

	while (pnet->getctrl("mrs_bool/notEmpty")->toBool())
	{
		pnet->tick();
	}
	delete pnet;
}
