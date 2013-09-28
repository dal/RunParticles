//
//  main.cpp
//  RunParticles
//
//  Created by Doug Letterman on 9/27/13.
//
//

#include <QApplication>
#include "GLWidget.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    
    GLWidget *widget = new GLWidget();
    
    widget->show();
    
    app.exec();
}

