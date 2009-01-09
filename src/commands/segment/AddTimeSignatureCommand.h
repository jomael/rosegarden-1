
/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2009 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_ADDTIMESIGNATURECOMMAND_H_
#define _RG_ADDTIMESIGNATURECOMMAND_H_

#include "base/NotationTypes.h"
#include "document/Command.h"
#include <QString>
#include "base/Event.h"
#include <klocale.h>




namespace Rosegarden
{

class Composition;


class AddTimeSignatureCommand : public NamedCommand
{
public:
    AddTimeSignatureCommand(Composition *composition,
                            timeT time,
                            TimeSignature timeSig);
    virtual ~AddTimeSignatureCommand();

    static QString getGlobalName() { return i18n("Add Time Si&gnature Change..."); }

    virtual void execute();
    virtual void unexecute();

protected:
    Composition *m_composition;
    timeT m_time;
    TimeSignature m_timeSignature;

    TimeSignature *m_oldTimeSignature; // for undo
    int m_timeSigIndex; // for undo
};    




}

#endif
