
#include <cstdio>
#include<iostream>
#include<iomanip>
#include <string>

#include "Conversions.h"
#include "CommandLineOptions.h"
#include "MarSystemManager.h"
#include "FileName.h"

#define EMPTYSTRING "MARSYAS_EMPTY" 

using namespace std;
using namespace Marsyas;

mrs_natural hopSize = 512;
mrs_natural windowSize = 1024;
mrs_natural lpcOrder = 10;

string inName = EMPTYSTRING;
string outName = EMPTYSTRING;
string outputDirectory = EMPTYSTRING;

CommandLineOptions cmd_options;

int process(string inName, string outName)
{
	MarSystemManager mng;
	// setting up the network
	MarSystem* input = mng.create("Series", "input");

	input->addMarSystem(mng.create("SoundFileSource","src"));
  //input->addMarSystem(mng.create("ShiftInput", "si"));
  
	MarSystem * fanout = mng.create("Fanout", "fanout");
	input->addMarSystem(fanout);
	
	// first branch of the fanout with the processing lpc
	MarSystem* lspS = mng.create("Series","lspS");
	lspS->addMarSystem(mng.create("Hamming", "ham"));
	lspS->addMarSystem(mng.create("LPC", "lpc"));
	fanout->addMarSystem(lspS);
	// second branch of the fanout with the filtering
	MarSystem* lspF = mng.create("Series","lspF");
	lspF->addMarSystem(mng.create("Filter", "residual"));
	lspF->addMarSystem(mng.create("Filter", "inverse"));
  MarSystem* audioSink = mng.create("SoundFileSink", "audioSink");
	lspF->addMarSystem(audioSink);
	fanout->addMarSystem(lspF);
  // third branch of the fanout
	fanout->addMarSystem(mng.create("Gain", "gain1"));

	input->updctrl("SoundFileSource/src/mrs_string/filename", inName);
	input->updctrl("SoundFileSource/src/mrs_natural/inSamples", hopSize);
	input->updctrl("ShiftInput/si/mrs_natural/Decimation", hopSize);
	input->updctrl("ShiftInput/si/mrs_natural/WindowSize", windowSize);

 	input->updctrl("Fanout/fanout/Series/lspS/LPC/lpc/mrs_natural/order",lpcOrder);
 	input->updctrl("Fanout/fanout/Series/lspS/LPC/lpc/mrs_real/lambda",0.0);
 	input->updctrl("Fanout/fanout/Series/lspS/LPC/lpc/mrs_real/gamma",1.0);
  input->updctrl("Fanout/fanout/Series/lspS/LPC/lpc/mrs_natural/featureMode", 0);

	input->linkctrl("Fanout/fanout/Series/lspS/LPC/lpc/mrs_realvec/coeffs", "Fanout/fanout/Series/lspF/Filter/residual/mrs_realvec/ncoeffs") ;
  input->linkctrl("Fanout/fanout/Series/lspS/LPC/lpc/mrs_realvec/coeffs", "Fanout/fanout/Series/lspF/Filter/inverse/mrs_realvec/dcoeffs") ;

	realvec one(lpcOrder);
	one.setval(0);
	one(1) = 1;

	input->updctrl("Fanout/fanout/Series/lspF/Filter/residual/mrs_realvec/dcoeffs", one);
  input->updctrl("Fanout/fanout/Series/lspF/Filter/inverse/mrs_realvec/ncoeffs", one);

	input->updctrl("Fanout/fanout/Series/lspF/SoundFileSink/audioSink/mrs_string/filename", outName);

	int i = 0;
	while(input->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
	{
		input->tick();

		realvec toto = input->getctrl("Fanout/fanout/Series/lspS/LPC/lpc/mrs_realvec/coeffs")->toVec();
		cout << toto << endl;
		realvec titi = input->getctrl("Fanout/fanout/Series/lspF/Filter/residual/mrs_realvec/ncoeffs")->toVec();
		cout << titi << endl;
		cout << "Processed frame " << i << endl;
		i++;
	}

	cout << endl << "LPC processing finished!";
	delete input;
	return 0;
}

void initOptions()
{
cmd_options.addNaturalOption("order", "o", lpcOrder);
cmd_options.addStringOption("outputDirectory", "O", outputDirectory);
}

void loadOptions()
{
lpcOrder = cmd_options.getNaturalOption("order");
outputDirectory = cmd_options.getStringOption("outputDirectory");
}

int
main(int argc, const char **argv)
{
	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();  

	vector<string> soundfiles = cmd_options.getRemaining();
	vector<string>::iterator sfi;

  cout << "vocalEffort Marsys implementation" << endl;
 
  cout<<"LPC and LSP order: " <<lpcOrder <<endl;
  cout<<"hopeSize: " <<hopSize <<endl;
	
		for (sfi=soundfiles.begin() ; sfi!=soundfiles.end() ; sfi++)
		{
	 cout << "Sound to analyze: " << *sfi << endl;
		FileName Sfname(*sfi);
outName = outputDirectory + "/" + Sfname.name();
			process(*sfi, outName);
		}

	cout << endl << "LPCwarped and LSP processing finished!";
}