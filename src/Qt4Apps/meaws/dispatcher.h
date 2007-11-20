#ifndef MEAWS_DISPATCHER_H
#define MEAWS_DISPATCHER_H

#include "defs.h"
#include "user.h"
#include "metro.h"
#include "backend.h"

#include "choose-exercise.h"

#include <QObject>

class Dispatcher : public QObject
{
	Q_OBJECT

public:
	Dispatcher(QObject* mainWindow, QFrame* centralFrame);
	~Dispatcher();

	QObject* getUserPointer()
		{ return user_; };

	bool close();

	QString getTitle();
	QString getMessage();

public slots:
	void openExercise();
/*
	void open();
	void close();
	void toggleAttempt();
	void setAttempt(bool running);
	bool openAttempt();

	void playFile();

	void analyze();  // temp ?
	void analysisDone(); // even tempier

	void newTry();

	void addTry()
	{
		exercise_->addTry();
	};
	void delTry()
	{
		exercise_->delTry();
	};

*/
signals:
	void updateMain(int state);

private:
// main object variables
	User *user_;
	Metro *metro_;
	Exercise *exercise_;
	MarBackend *marBackend_;

// setup
	void setupExercise();

	// basic GUI frame
	QFrame *centralFrame_;
/*
//	void connectMain(QObject* mainWindow);
	bool chooseEvaluation();


	// left-over garbage (?)
	QString exerciseName_;
	bool attemptRunningBool_;

	QString statusMessage_;
*/
};
#endif
