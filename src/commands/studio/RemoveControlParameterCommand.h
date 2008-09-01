
/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2008 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_REMOVECONTROLPARAMETERCOMMAND_H_
#define _RG_REMOVECONTROLPARAMETERCOMMAND_H_

#include "base/ControlParameter.h"
#include "base/Device.h"
#include "document/Command.h"
#include <QString>
#include <klocale.h>




namespace Rosegarden
{

class Studio;


class RemoveControlParameterCommand : public NamedCommand
{
public:
    RemoveControlParameterCommand(Studio *studio,
                                  DeviceId device,
                                  int id):
        NamedCommand(getGlobalName()),
        m_studio(studio),
        m_device(device),
        m_id(id) { }

    ~RemoveControlParameterCommand();

    virtual void execute();
    virtual void unexecute();

    static QString getGlobalName() { return i18n("&Remove Control Parameter"); }

protected:
    Studio              *m_studio;
    DeviceId             m_device;
    int                              m_id;
    ControlParameter     m_oldControl;

};


}

#endif
