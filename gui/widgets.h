// -*- c-basic-offset: 4 -*-

/*
    Rosegarden-4 v0.1
    A sequencer and musical notation editor.

    This program is Copyright 2000-2002
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <bownie@bownie.com>

    The moral right of the authors to claim authorship of this work
    has been asserted.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qfont.h>
#include <qprogressdialog.h>

#include "Progress.h"

#ifndef _WIDGETS_H_
#define _WIDGETS_H_

/** Create out own check box which is always Tristate 
 * and allows us to click only between on and off
 * and only to _show_ the third ("Some") state 
 */
class RosegardenTristateCheckBox : public QCheckBox
{
Q_OBJECT
public:
    RosegardenTristateCheckBox(QWidget *parent=0,
                               const char *name=0):QCheckBox(parent, name)
        { setTristate(true) ;}

    virtual ~RosegardenTristateCheckBox();

protected:
    // don't emit when the button is released
    virtual void mouseReleaseEvent(QMouseEvent *);

private:
};

/**
 * Turn a normal QComboBox into one that accepts mouse wheel
 * events to change the value
 */
class RosegardenComboBox : public QComboBox
{
Q_OBJECT
public:
RosegardenComboBox(bool /* reverse */, QWidget *parent=0, const char *name=0):
        QComboBox(parent, name) {;}

    RosegardenComboBox(bool reverse, bool rw,
                       QWidget *parent=0, const char *name=0):
        QComboBox(rw, parent, name), m_reverse(reverse) {;}


protected:
    virtual void wheelEvent(QWheelEvent *e);

signals:
    void propagate(int); // update the Segment with new value

private:
    bool m_reverse;

};

// A label that emits a double click signal
//
class RosegardenLabel : public QLabel
{
Q_OBJECT
public:
    RosegardenLabel(QWidget *parent = 0, const char *name=0):
        QLabel(parent, name) {;}

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent * /*e*/)
        { emit doubleClicked(); }

signals:
    void doubleClicked();

};

/**
 * A Combobox that just about handles doubles - you have
 * to set the precision outside of this class if you're
 * using it with Qt designer.  Urch.
 */
class RosegardenSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    RosegardenSpinBox(QWidget *parent = 0, const char *name=0);

    double getDoubleValue() const { return m_doubleValue; }

protected:
    virtual QString mapValueToText (int value);
    virtual int mapTextToValue(bool *ok);

signals:
    double m_doubleValue;
};


/**
 * Specialisation of QGroupBox that selects a slightly-smaller-
 * than-normal font size and draws its title in bold.  Not
 * terrifically exciting.
 */
class RosegardenParameterBox : public QGroupBox
{
    Q_OBJECT
public:
    RosegardenParameterBox(QString label,
			   QWidget *parent = 0,
			   const char *name = 0);

    QFont getFont() const { return m_font; }

private:
    QFont m_font;
};

class RosegardenProgressDialog : public QProgressDialog,
                                 public Rosegarden::Progress
{
    Q_OBJECT
public:
    RosegardenProgressDialog(QApplication *app,
                             QWidget * creator = 0,
                             const char * name = 0,
                             bool modal = FALSE,
                             WFlags f = 0);

    RosegardenProgressDialog(QApplication *app,
                             const QString &labelText,
                             const QString &cancelButtonText,
                             int totalSteps,
                             QWidget *creator = 0,
                             const char *name = 0,
                             bool modal = FALSE,
                             WFlags f = 0);

    ~RosegardenProgressDialog();

    // Set the progress
    //
    virtual void setCompleted(int value);

    // Process some X events - gets called by the file access (say) class
    // to ensure our gui is still working.
    //
    virtual void processEvents();

public slots:
    // After a timeout, judge whether we should show ourselves yet
    // and do so if so.
    // Hence we only appear for long operations.
    // 
    void slotTimerElapsed();
    
    // Show ourselves.
    // 
    void slotShowMyself();

private:

    // Application handle
    //
    QApplication *m_app;

    bool m_firstTimeout;

};

#endif // _WIDGETS_H_
