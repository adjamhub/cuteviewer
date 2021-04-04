/*
 * Copyright (C) Andrea Diamantini 2021 <adjam@protonmail.com>
 *
 * CuteViewer project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#ifndef APPLICATION_H
#define APPLICATION_H


#include <QApplication>

class MainWindow;


class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char *argv[]);

    void parseCommandlineArgs();

    void loadPaths(const QStringList& paths);
    void loadPath(const QString& path);

    void removeWindowFromList(MainWindow* w);

    void loadSettings();

private:
    QList<MainWindow*> _windows;
};

#endif // APPLICATION_H
