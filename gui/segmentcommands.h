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


#ifndef _SEGMENTCOMMANDS_H_
#define _SEGMENTCOMMANDS_H_

#include <klocale.h>
#include <kcommand.h>

#include "Segment.h"
#include "Composition.h"
#include "NotationTypes.h"
#include "rosegardenguidoc.h"

#ifdef RGKDE3
typedef KNamedCommand XKCommand;
#else
typedef KCommand XKCommand;
#endif

/**
 * Base class for commands from the SegmentParameterBox
 */
class SegmentCommand : public XKCommand
{
public:
    SegmentCommand(QString name, const std::vector<Rosegarden::Segment*>&);

    typedef std::vector<Rosegarden::Segment*> segmentlist;
    
protected:
    segmentlist m_segments;
};

/**
 * Repeat segment command from the SegmentParameterBox
 */
class SegmentCommandRepeat : public SegmentCommand
{
public:
    SegmentCommandRepeat(const std::vector<Rosegarden::Segment*>&,
                         bool repeat);

    virtual void execute();
    virtual void unexecute();

protected:
    bool m_repeatState;
};

// Disabled until we find a better solution
//
// As it is, command creation happens on every change of the text
// value of the combo box
//
//
// class SegmentCommandChangeTransposeValue : public SegmentCommand
// {
// public:
//     SegmentCommandChangeTransposeValue(const std::vector<Rosegarden::Segment*>&,
//                                        int transposeValue);

//     virtual void execute();
//     virtual void unexecute();

// protected:
//     int m_transposeValue;
//     std::vector<int> m_savedValues;
// };


////////////////////////////////////////////////////////////

class SegmentEraseCommand : public XKCommand
{
public:
    SegmentEraseCommand(Rosegarden::Segment *segment);
    virtual ~SegmentEraseCommand();

    virtual void execute();
    virtual void unexecute();
    
private:
    Rosegarden::Composition *m_composition;
    Rosegarden::Segment *m_segment;
    bool m_detached;
};

class SegmentQuickCopyCommand : public XKCommand
{
public:
    SegmentQuickCopyCommand(Rosegarden::Segment *segment);
    virtual ~SegmentQuickCopyCommand();

    virtual void execute();
    virtual void unexecute();

    // return pointer to new copy
    Rosegarden::Segment* getCopy() { return m_segment; }

private:
    Rosegarden::Composition *m_composition;
    Rosegarden::Segment     *m_segmentToCopy;
    Rosegarden::Segment     *m_segment;
    bool m_detached;
};


class SegmentInsertCommand : public XKCommand
{
public:
    SegmentInsertCommand(RosegardenGUIDoc *doc,
                         Rosegarden::TrackId track,
                         Rosegarden::timeT startTime,
                         Rosegarden::timeT endTime);
    virtual ~SegmentInsertCommand();

    virtual void execute();
    virtual void unexecute();
    
private:
    Rosegarden::Composition *m_composition;
    Rosegarden::Studio      *m_studio;
    Rosegarden::Segment     *m_segment;
    int                      m_track;
    Rosegarden::timeT        m_startTime;
    Rosegarden::timeT        m_endTime;
    bool m_detached;
};


/**
 * SegmentRecordCommand pretends to insert a Segment that is actually
 * already in the Composition (the currently-being-recorded one).  It's
 * used at the end of recording, to ensure that GUI updates happen
 * correctly, and it provides the ability to undo recording.  (The
 * unexecute does remove the segment, it doesn't just pretend to.)
 */
class SegmentRecordCommand : public XKCommand
{
public:
    SegmentRecordCommand(Rosegarden::Segment *segment);
    virtual ~SegmentRecordCommand();

    virtual void execute();
    virtual void unexecute();

private:
    Rosegarden::Composition *m_composition;
    Rosegarden::Segment *m_segment;
    bool m_detached;
};


/**
 * SegmentReconfigureCommand is a general-purpose command for
 * moving, resizing or changing the track of one or more segments
 */
class SegmentReconfigureCommand : public XKCommand
{
public:
    SegmentReconfigureCommand(QString name);
    virtual ~SegmentReconfigureCommand();

    struct SegmentRec {
        Rosegarden::Segment *segment;
        Rosegarden::timeT startTime;
        Rosegarden::timeT endTime;
        Rosegarden::TrackId track;
    };
    typedef std::vector<SegmentRec> SegmentRecSet;

    void addSegment(Rosegarden::Segment *segment,
                    Rosegarden::timeT startTime,
                    Rosegarden::timeT endTime,
                    Rosegarden::TrackId track);

    void addSegments(const SegmentRecSet &records);

    void execute();
    void unexecute();

private:
    SegmentRecSet m_records;
    void swap();
};


class SegmentSplitCommand : public XKCommand
{
public:
    SegmentSplitCommand(Rosegarden::Segment *segment,
                        Rosegarden::timeT splitTime);
    virtual ~SegmentSplitCommand();

    virtual void execute();
    virtual void unexecute();

private:
    Rosegarden::Segment *m_segment;
    Rosegarden::Segment *m_newSegment;
    Rosegarden::timeT m_splitTime;
    bool m_detached;
};


class SegmentAutoSplitCommand : public XKCommand
{
public:
    SegmentAutoSplitCommand(Rosegarden::Segment *segment);
    virtual ~SegmentAutoSplitCommand();

    virtual void execute();
    virtual void unexecute();
    
    static QString getGlobalName() { return "&Auto-Split"; }

private:
    Rosegarden::Segment *m_segment;
    Rosegarden::Composition *m_composition;
    std::vector<Rosegarden::Segment *> m_newSegments;
    bool m_detached;

    bool isBarEmpty(int barNo);
};


class SegmentRescaleCommand : public XKCommand
{
public:
    SegmentRescaleCommand(Rosegarden::Segment *segment,
			  int multiplier,
			  int divisor);
    virtual ~SegmentRescaleCommand();

    virtual void execute();
    virtual void unexecute();
    
    static QString getGlobalName() { return "&Rescale..."; }

private:
    Rosegarden::Segment *m_segment;
    Rosegarden::Segment *m_newSegment;
    int m_multiplier;
    int m_divisor;
    bool m_detached;
};


class SegmentChangeQuantizationCommand : public XKCommand
{
public:
    /// Set quantization on segments.  If sq is null, switch quantization off.
    SegmentChangeQuantizationCommand(Rosegarden::StandardQuantization *sq);
    virtual ~SegmentChangeQuantizationCommand();

    void addSegment(Rosegarden::Segment *s);

    virtual void execute();
    virtual void unexecute();

    static QString getGlobalName(Rosegarden::StandardQuantization *sq);

private:
    struct SegmentRec {
        Rosegarden::Segment *segment;
        Rosegarden::Quantizer *oldQuantizer;
        bool wasQuantized;
    };
    typedef std::vector<SegmentRec> SegmentRecSet;
    SegmentRecSet m_records;

    Rosegarden::StandardQuantization *m_quantization;
};


class AddTimeSignatureCommand : public XKCommand
{
public:
    AddTimeSignatureCommand(Rosegarden::Composition *composition,
                            Rosegarden::timeT time,
                            Rosegarden::TimeSignature timeSig);
    virtual ~AddTimeSignatureCommand();

    static QString getGlobalName() { return i18n("Add Time Si&gnature Change..."); }

    virtual void execute();
    virtual void unexecute();

protected:
    Rosegarden::Composition *m_composition;
    Rosegarden::timeT m_time;
    Rosegarden::TimeSignature m_timeSignature;

    Rosegarden::TimeSignature *m_oldTimeSignature; // for undo
    int m_timeSigIndex; // for undo
};    



class AddTimeSignatureAndNormalizeCommand : public KMacroCommand
{
public:
    AddTimeSignatureAndNormalizeCommand(Rosegarden::Composition *composition,
					Rosegarden::timeT time,
					Rosegarden::TimeSignature timeSig);
    virtual ~AddTimeSignatureAndNormalizeCommand();
};



class ModifyDefaultTempoCommand : public XKCommand
{
public:
    ModifyDefaultTempoCommand(Rosegarden::Composition *composition,
                              double tempo):
	XKCommand(getGlobalName()),
        m_composition(composition),
        m_tempo(tempo) {}

    virtual ~ModifyDefaultTempoCommand() {}

    static QString getGlobalName() { return i18n("Modify &Default Tempo..."); }

    virtual void execute();
    virtual void unexecute();

private:
    Rosegarden::Composition *m_composition;
    double                   m_tempo;
    double                   m_oldTempo;
};


class RemoveTempoChangeCommand : public XKCommand
{
public:
    RemoveTempoChangeCommand(Rosegarden::Composition *composition,
                             int index):
	XKCommand(getGlobalName()),
        m_composition(composition),
        m_tempoChangeIndex(index),
        m_oldTime(0),
        m_oldTempo(0){}

    virtual ~RemoveTempoChangeCommand() {}

    static QString getGlobalName() { return i18n("Remove &Tempo Change..."); }

    virtual void execute();
    virtual void unexecute();

private:
    Rosegarden::Composition *m_composition;
    int                      m_tempoChangeIndex;
    Rosegarden::timeT        m_oldTime;
    long                     m_oldTempo; // bph
};



class AddTempoChangeCommand : public XKCommand
{
public:
    AddTempoChangeCommand(Rosegarden::Composition *composition,
                          Rosegarden::timeT time,
                          double tempo):
	XKCommand(getGlobalName()),
        m_composition(composition),
        m_time(time),
        m_tempo(tempo),
        m_oldTempo(0),
        m_tempoChangeIndex(0) {}

    virtual ~AddTempoChangeCommand();

    static QString getGlobalName() { return i18n("Add Te&mpo Change..."); }

    virtual void execute();
    virtual void unexecute();

private:
    Rosegarden::Composition *m_composition;
    Rosegarden::timeT m_time;
    double m_tempo; // bpm
    int m_oldTempo; // bph
    int m_tempoChangeIndex;
};


class AddTracksCommand : public XKCommand
{
public:
    AddTracksCommand(Rosegarden::Composition *composition,
                     unsigned int nbTracks): 
        XKCommand(getGlobalName()),
        m_composition(composition),
        m_nbNewTracks(nbTracks) {}

    static QString getGlobalName() { return i18n("Add Tracks..."); }

    virtual void execute();
    virtual void unexecute();

private:
    Rosegarden::Composition *m_composition;
    unsigned int m_nbNewTracks;
};



#endif  // _SEGMENTCOMMANDS_H_
