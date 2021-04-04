/*
 * Copyright (C) Andrea Diamantini 2021 <adjam@protonmail.com>
 *
 * CuteViewer project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "application.h"

#include "config.h"


int main(int argc, char *argv[])
{
    Application app(argc,argv);
    
    QCoreApplication::setApplicationName( QStringLiteral(PROJECT_NAME) );
    QCoreApplication::setApplicationVersion( QStringLiteral(PROJECT_VERSION) );
    QCoreApplication::setOrganizationName( QStringLiteral("adjam") );
    QCoreApplication::setOrganizationDomain( QStringLiteral("adjam.org") );

    app.parseCommandlineArgs();

    return app.exec();
}
