#include "QtMarRhythmLines.h"
#include <iostream>
using namespace std;

namespace MarsyasQt
{

QtMarRhythmLines::QtMarRhythmLines(QWidget *parent)
		: QtMarPlot(parent)
{
	setVertical(0,1);
	setPlotName("onsets");
	expectedOffset_ = 0;
	detectedOffset_ = 0;
}

QtMarRhythmLines::~QtMarRhythmLines()
{
}

void
QtMarRhythmLines::paintEvent(QPaintEvent *)
{
	if (data_.getSize()==0)
		return;

	QPainter painter(this);
	// plot name
	painter.drawText( 4, 14, plotName_);

	// dotted line
	QPen pen(Qt::SolidPattern);

	pen.setWidth(width_);
	pen.setStyle(Qt::SolidLine);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(pen);

	// constants
	mrs_real hScale =
		width() / expectedLines_(expectedLines_.getSize()-1);
	mrs_real vScale = height() / (highVal_ - minVal_);
	//mrs_natural midY = height()/2;

	// variables from data
	mrs_natural start, end;
	//mrs_natural errorDirection;
	//mrs_real errorMagnitude;

	// variables
	mrs_natural y;

	mrs_natural x;
	// iterates over the data_
	for (mrs_natural i=0; i<data_.getSize() - detectedOffset_; i++)
	{
		x = mrs_natural (i * hScale);
		if ( (i+detectedOffset_)<0)
			y=0;
		else
			y = mrs_natural ( data_(i+detectedOffset_) * vScale );
		painter.drawLine(x, height()-y, x, height()-1);
	}

	// draws the expected lines
	painter.setPen(QColor(Qt::red));
	end = 0;
	for (mrs_natural i=1; i<expectedLines_.getSize(); i++) {
		start = end;
		end = (mrs_natural) ( expectedLines_(i) * hScale );
		painter.drawLine(start, 0, start, height()-1);
	}

}

} //namespace
