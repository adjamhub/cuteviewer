/*
 * Copyright (C) Andrea Diamantini 2020-2021 <adjam@protonmail.com>
 *
 * CutePad project
 *
 * @license GPL-3.0 <https://www.gnu.org/licenses/gpl-3.0.txt>
 */


#include "statusbar.h"

#include <QHBoxLayout>
#include <QLabel>


StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
    , _langLabel(new QLabel(this))
    , _posLabel(new QLabel(this))
    , _codecLabel(new QLabel(this))
    , _zoomLabel(new QLabel(this))
{
    // The UI
    auto layout = new QHBoxLayout;
    layout->setContentsMargins (0, 0, 0, 0);
    layout->addWidget (_posLabel);
    layout->addWidget (_langLabel);
    layout->addWidget (_codecLabel);
    layout->addWidget (_zoomLabel);
    setLayout (layout);
}


void StatusBar::setLanguage(const QString& lang)
{
    QString msg;
    msg += QLatin1String("&nbsp;&nbsp;<b>") + tr("Language") + QLatin1String(": </b>");
    msg += lang;
    _langLabel->setText(msg);
}


void StatusBar::setPosition(int row, int col)
{
    QString msg;
    msg += QLatin1String("&nbsp;&nbsp;<b>") + tr("Row") + QLatin1String(": </b>");
    msg += QString::number(row + 1);
    msg += QLatin1String(", <b>") + tr("Column") + QLatin1String(": </b>");
    msg += QString::number(col + 1);
    _posLabel->setText(msg);
}


void StatusBar::setCodec(const QString& codec)
{
    QString msg;
    msg += QLatin1String("&nbsp;&nbsp;<b>") + tr("Encoding") + QLatin1String(": </b>");
    msg += codec;
    _codecLabel->setText(msg);
}


void StatusBar::setZoom(const QString& zoom)
{
    QString msg;
    msg += QLatin1String("&nbsp;&nbsp;<b>") + tr("Zoom") + QLatin1String(": </b>");
    msg += zoom;
    _zoomLabel->setText(msg);
}
