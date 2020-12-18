///////////////////////////////////////////////////
//
//	Hamish Carr
//	January, 2018
//
//	------------------------
//	main.cpp
//	------------------------
//
///////////////////////////////////////////////////

#include <QApplication>
#include "RenderWidget.h"
#include "MasterWidget.h"
#include "GeometricSurfaceFaceDS.h"
#include <stdio.h>

#define SCREENX 900
#define SCREENY 600

int main(int argc, char **argv)
	{ // main()
	// initialize QT
	QApplication app(argc, argv);


	// check the args to make sure there's an input file
	if (argc == 2)
		{ // two parameters - read a file

			//	create a window
			//RenderWidget aWindow(&surface, NULL);

			MasterWidget aWindow(argv[1], NULL);

			// 	set the initial size
			aWindow.resize(SCREENX, SCREENY);

			aWindow.show();

			// set QT running
			return app.exec();
		} // two parameters - read a file
	else
		{ // too many parameters
		printf("Usage: %s filename\n", argv[0]);
		exit (0);
		} // too many parameters

	// paranoid return value
	exit(0);
	} // main()
