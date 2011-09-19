
/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.
    Copyright 2000-2010 the Rosegarden development team.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_SEGMENTLINKTOCOPYCOMMAND_H
#define _RG_SEGMENTLINKTOCOPYCOMMAND_H

#include "document/Command.h"
#include <QString>
#include <QCoreApplication>

namespace Rosegarden
{

class Segment;
class Composition;

class SegmentLinkToCopyCommand : public NamedCommand
{
    Q_DECLARE_TR_FUNCTIONS(Rosegarden::SegmentLinkToCopyCommand)

public:
    SegmentLinkToCopyCommand(Segment *segment);
    virtual ~SegmentLinkToCopyCommand();

    virtual void execute();
    virtual void unexecute();

    static QString getGlobalName() { return tr("Turn Links into Copies"); }

private:
    Composition *m_composition;
    Segment     *m_Segment;
    Segment     *m_SegmentCopy;
};

}

#endif // _RG_SEGMENTLINKTOCOPYCOMMAND_H
