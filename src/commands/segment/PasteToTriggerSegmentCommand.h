
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

#ifndef _RG_PASTETOTRIGGERSEGMENTCOMMAND_H_
#define _RG_PASTETOTRIGGERSEGMENTCOMMAND_H_

#include "base/TriggerSegment.h"
#include "document/Command.h"
#include <QString>




namespace Rosegarden
{

class Segment;
class Composition;
class Clipboard;


class PasteToTriggerSegmentCommand : public NamedCommand
{
public:
    /// If basePitch is -1, the first pitch in the selection will be used
    PasteToTriggerSegmentCommand(Composition *composition,
                                 Clipboard *clipboard,
                                 QString label,
                                 int basePitch = -1,
                                 int baseVelocity = -1);
    virtual ~PasteToTriggerSegmentCommand();

    virtual void execute();
    virtual void unexecute();

protected:
    Composition *m_composition;
    Clipboard *m_clipboard;
    QString m_label;
    int m_basePitch;
    int m_baseVelocity;
    Segment *m_segment;
    TriggerSegmentId m_id;
    bool m_detached;
};



}

#endif
