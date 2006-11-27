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
 \class PeSimilarity
    \brief similarities computation routines for peaks extraction project
*/

#include "PeSimilarity.h"
#include "PeUtilities.h"

using namespace std;
using namespace Marsyas;

void
Marsyas::similarityMatrix(realvec &data, realvec& m, string type, mrs_natural nbMaxSines, mrs_natural hSize)
{
	mrs_natural i, j;
	realvec vec(data.getRows());

	m.setval(1);

	for (i=0 ; i< (mrs_natural) type.length() ; i+=2)
	{
		switch(type[i])
		{
		case 'f':
			j=0;
			break;
		case 'a':
			j=1;
			break;
		case 'F':
			j=2;
			break;
		case 'A':
			j=3;
			break;
		case 'h':
			j=-1;
			break;
		case 't':
			j=5;
			break;
		default:
			cout << "unrecognized peak parameter: " << type[i] << endl;
			exit(1);
			break;
		}
		if(j>=0)
		{
			// select the parameter
			for(int k=0 ; k<data.getRows() ; k++)
				vec(k) = data(k, j);

			// apply processing
			switch(type[i+1])
		 {
			case 'o':
				break;
			case 'l':
				if(type[i] == 'f')
					vec.apply(hertz2bark);
				else if (type[i] == 'a')
					vec.apply(amplitude2dB);
				else
			 {
				 cout << "unrecognized parameter normalization : " << type[i] << endl;
				 exit(1);
			 }
				break;
			case 'n':
				vec.normObsMinMax();
				break;
			case 'b':
				if(type[i] == 'f')
					vec.apply(hertz2bark);
				else if (type[i] == 'a')
					vec.apply(amplitude2dB);
				else
			 {
				 cout << "unrecognized parameter normalization : " << type[i] << endl;
				 exit(1);
			 }
				vec.normObsMinMax();
				break;
			default:
				cout << "unrecognized peak processing: " << type[i] << endl;
				exit(1);
				break;
		 }
			// do the job
			similarityCompute(vec, m);
		}
		else
		{// specific similarity computation
			std::vector<realvec> frequencySet_;
			std::vector<realvec> amplitudeSet_;

			realvec normData = data;
			for(int j =0 ; j<normData.getRows() ; j++)
				//normData(j, pkAmplitude) = amplitude2dB(normData(j, pkAmplitude));

				

				// build frequency and amplitude matrices
				extractParameter(normData, frequencySet_, pkFrequency, nbMaxSines);
				normData.normSplMinMax(2);
			extractParameter(normData, amplitudeSet_, pkAmplitude, nbMaxSines);

			harmonicitySimilarityCompute(data, frequencySet_, amplitudeSet_, m, hSize);	
		}
	}
}

void
Marsyas::similarityCompute(realvec &d, realvec& m)
{
	int i, j;

	// d.dump();
	// similarity computing
	for(i=0 ; i<d.getRows() ; i++)
	{
		for(j=0 ; j<i ; j++)
		{
			mrs_real val = exp(-(d(i)-d(j))*(d(i)-d(j)));
			m(i, j) *= val;
			m(j, i) *= val;
		}
	}
}

void
Marsyas::harmonicitySimilarityCompute(realvec& data, std::vector<realvec>& fSet, std::vector<realvec>& aSet, realvec& m, mrs_natural hSize)
{
	mrs_natural i, j, startIndex = (mrs_natural) data(0, 5);

	// similarity computing
	for(i=0 ; i<data.getRows() ; i++)
	{
		for(j=0 ; j<i ; j++)
		{
			mrs_real val=0, hF;
			mrs_natural indexFirst = (mrs_natural) (data(i, 5)-startIndex), indexSecond = (mrs_natural) (data(j, 5)-startIndex);

			realvec firstF = fSet[indexFirst];
			realvec firstA = aSet[indexFirst];
			realvec secondF = fSet[indexSecond];
			realvec secondA = aSet[indexSecond];


			MATLAB_PUT(data(i, pkFrequency), "sf1");
			MATLAB_PUT(data(j, pkFrequency), "sf2");
			MATLAB_PUT(firstF, "f1");
			MATLAB_PUT(firstA, "a1");
			MATLAB_PUT(secondF, "f2");
			MATLAB_PUT(secondA, "a2");

			// fundamentqal frequency estimates
			hF = min(data(i, pkFrequency), data(j, pkFrequency));
			// mrs_real mhF = min(hF, abs(data(i, pkFrequency)-data(j, pkFrequency)));
		
			// weight the amplitudes
			//for (mrs_natural k=0 ; k<firstF.getSize() ; k++)
			//	firstA(k)*=harmonicWeighting(firstF(k), hF, harmonicityWeight_);
			//for (mrs_natural k=0 ; k<secondF.getSize() ; k++)
			//	secondA(k)*=harmonicWeighting(secondF(k), hF, harmonicityWeight_);


			//cout << firstF;
			// align and wrap frequencies
			firstF-=data(i, pkFrequency);
			secondF-=data(j, pkFrequency);

			firstF/=hF;
			secondF/=hF;

			for (mrs_natural k=0 ; k<firstF.getSize() ; k++)
			{
				firstF(k)=fmod(firstF(k), 1);
				if(firstF(k)<0)
					firstF(k)+=1;
			}
			for (mrs_natural k=0 ; k<secondF.getSize() ; k++)
			{
				secondF(k)=fmod(secondF(k), 1);
				if(secondF(k)<0)
					secondF(k)+=1;
			}

			//	realvec v(100);
			//		for (mrs_natural k=0 ; k<v.getSize() ; k++)
			//v(k) = harmonicWeighting(k, 10, 0.1);
			//MATLAB_PUT(v, "v");
			//MATLAB_EVAL("clf ; plot(v)");

			// compare the two
			// cout <<firstF<<secondF;

	/*		MATLAB_PUT(firstF, "F1");
			MATLAB_PUT(firstA, "A1");
			MATLAB_PUT(secondF, "F2");
			MATLAB_PUT(secondA, "A2");*/
		//	MATLAB_EVAL("plotHarmo");

			// cout << data(i, pkFrequency) << " " <<data(j, pkFrequency) <<endl;
			/*if(firstF.getSize() < secondF.getSize())
				val = correlatePeakSets(firstF,firstA,secondF,secondA);
			else
				val = correlatePeakSets(secondF,secondA,firstF,firstA);*/

			val = cosinePeakSets(firstF,firstA, secondF,secondA, aSet[indexFirst], aSet[indexSecond], hSize);

			//	val=exp(-val);
	//		cout << data(i, pkFrequency) << " " <<data(j, pkFrequency) << " value: "<< val <<endl;
			m(i, j) *= exp(val*val);
			m(j, i) *= exp(val*val);
		}
	}
}

