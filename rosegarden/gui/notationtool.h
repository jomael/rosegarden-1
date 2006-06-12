// -*- c-basic-offset: 4 -*-

/*
  Rosegarden-4
  A sequencer and musical notation editor.

  This program is Copyright 2000-2006
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

#ifndef NOTATIONTOOL_H
#define NOTATIONTOOL_H

#include "NotationTypes.h"
#include "Segment.h"

#include "edittool.h"
#include "notationelement.h"
#include "notestyle.h"
#include "notationstaff.h"
#include "guitar/guitartabselector.h"

class QCanvasRectangle;

namespace Rosegarden { class EventSelection; }

class NotationView;
class QPopupMenu;

//////////////////////////////////////////////////////////////////////
//               Notation Tools
//////////////////////////////////////////////////////////////////////

class NotationTool;

/**
 * NotationToolBox : maintains a single instance of each registered tool
 *
 * Tools are fetched from a name
 */
class NotationToolBox : public EditToolBox
{
public:
    NotationToolBox(NotationView* parent);

protected:
    virtual EditTool* createTool(const QString& toolName);

    //--------------- Data members ---------------------------------

    NotationView* m_nParentView;
};


/**
 * Notation tool base class.
 *
 * A NotationTool represents one of the items on the notation toolbars
 * (notes, rests, clefs, eraser, etc...). It handle mouse click events
 * for the NotationView ('State' design pattern).
 *
 * A NotationTool can have a menu, normally activated through a right
 * mouse button click. This menu is defined in an XML file, see
 * NoteInserter and noteinserter.rc for an example.
 *
 * This class is a "semi-singleton", that is, only one instance per
 * NotationView window is created. This is because menu creation is
 * slow, and the fact that a tool can trigger the setting of another
 * tool through a menu choice). This is maintained with the
 * NotationToolBox class This means we can't rely on the ctor/dtor to
 * perform setting up, like mouse cursor changes for instance. Use the
 * ready() and stow() method for this.
 *
 * @see NotationView#setTool()
 * @see NotationToolBox
 */
class NotationTool : public EditTool
{
    friend class NotationToolBox;

public:
    virtual ~NotationTool();

    /**
     * Is called by NotationView when the tool is set as current
     * Add any setup here
     */
    virtual void ready();

protected:
    /**
     * Create a new NotationTool
     *
     * \a menuName : the name of the menu defined in the XML rc file
     */
    NotationTool(const QString& menuName, NotationView*);

    //--------------- Data members ---------------------------------

    NotationView* m_nParentView;
};

namespace Rosegarden { class SegmentNotationHelper; }

/**
 * This tool will insert notes on mouse click events
 */
class NoteInserter : public NotationTool
{
    Q_OBJECT

    friend class NotationToolBox;

public:
    ~NoteInserter();

    virtual void handleLeftButtonPress(Rosegarden::timeT,
                                       int height,
                                       int staffNo,
                                       QMouseEvent*,
                                       Rosegarden::ViewElement* el);

    virtual int handleMouseMove(Rosegarden::timeT time,
                                int height,
                                QMouseEvent*);

    virtual void handleMouseRelease(Rosegarden::timeT time,
                                    int height,
                                    QMouseEvent*);

    virtual void ready();

    Rosegarden::Note getCurrentNote() {
	return Rosegarden::Note(m_noteType, m_noteDots);
    }

    /// Insert a note as if the user has clicked at the given time & pitch
    void insertNote(Rosegarden::Segment &segment,
		    Rosegarden::timeT insertionTime,
		    int pitch,
		    Rosegarden::Accidental accidental,
		    bool suppressPreview = false);

    static const QString ToolName;

public slots:
    /// Set the type of note (quaver, breve...) which will be inserted
    void slotSetNote(Rosegarden::Note::Type);

    /// Set the nb of dots the inserted note will have
    void slotSetDots(unsigned int dots);
 
    /// Set the accidental for the notes which will be inserted
    void slotSetAccidental(Rosegarden::Accidental, bool follow);

protected:
    NoteInserter(NotationView*);

    /// this ctor is used by RestInserter
    NoteInserter(const QString& menuName, NotationView*);

    Rosegarden::timeT getOffsetWithinRest(int staffNo,
					  const NotationElementList::iterator&,
					  double &canvasX);

    int getOttavaShift(Rosegarden::Segment &segment, Rosegarden::timeT time);

    virtual Rosegarden::Event *doAddCommand(Rosegarden::Segment &,
					    Rosegarden::timeT time,
					    Rosegarden::timeT endTime,
					    const Rosegarden::Note &,
					    int pitch, Rosegarden::Accidental);

    virtual bool computeLocationAndPreview(QMouseEvent *e);
    virtual void showPreview();
    virtual void clearPreview();

protected slots:
    // RMB menu slots
    void slotNoAccidental();
    void slotFollowAccidental();
    void slotSharp();
    void slotFlat();
    void slotNatural();
    void slotDoubleSharp();
    void slotDoubleFlat();
    void slotToggleDot();
    void slotToggleAutoBeam();

    void slotEraseSelected();
    void slotSelectSelected();
    void slotRestsSelected();

protected:
    //--------------- Data members ---------------------------------

    Rosegarden::Note::Type m_noteType;
    unsigned int m_noteDots;
    bool m_autoBeam;
    bool m_matrixInsertType;
    NoteStyleName m_defaultStyle;

    bool m_clickHappened;
    Rosegarden::timeT m_clickTime;
    int m_clickPitch;
    int m_clickHeight;
    int m_clickStaffNo;
    double m_clickInsertX;

    Rosegarden::Accidental m_accidental;
    Rosegarden::Accidental m_lastAccidental;
    bool m_followAccidental;

    static const char* m_actionsAccidental[][4];
};

/**
 * This tool will insert rests on mouse click events
 */
class RestInserter : public NoteInserter
{
    Q_OBJECT
    
    friend class NotationToolBox;

public:

    static const QString ToolName;

protected:
    RestInserter(NotationView*);

    virtual Rosegarden::Event *doAddCommand(Rosegarden::Segment &,
					    Rosegarden::timeT time,
					    Rosegarden::timeT endTime,
					    const Rosegarden::Note &,
					    int pitch, Rosegarden::Accidental);
    virtual void showPreview();

protected slots:
    void slotToggleDot();
    void slotNotesSelected();
};

/**
 * This tool will insert clefs on mouse click events
 */
class ClefInserter : public NotationTool
{
    Q_OBJECT
    
    friend class NotationToolBox;

public:
    void setClef(std::string clefType);

    virtual void ready();

    virtual void handleLeftButtonPress(Rosegarden::timeT,
                                       int height,
                                       int staffNo,
                                       QMouseEvent*,
                                       Rosegarden::ViewElement* el);
    static const QString ToolName;

protected slots:
    void slotNotesSelected();
    void slotEraseSelected();
    void slotSelectSelected();

protected:
    ClefInserter(NotationView*);
    
    //--------------- Data members ---------------------------------

    Rosegarden::Clef m_clef;
};


/**
 * This tool will request and insert text on mouse click events
 */
class TextInserter : public NotationTool
{
    Q_OBJECT
    
    friend class NotationToolBox;

public:
    virtual void ready();

    virtual void handleLeftButtonPress(Rosegarden::timeT,
                                       int height,
                                       int staffNo,
                                       QMouseEvent*,
                                       Rosegarden::ViewElement* el);
    static const QString ToolName;

protected slots:
    void slotNotesSelected();
    void slotEraseSelected();
    void slotSelectSelected();

protected:
    TextInserter(NotationView*);
    Rosegarden::Text m_text;
};


/**
 * This tool will erase a note on mouse click events
 */
class NotationEraser : public NotationTool
{
    Q_OBJECT

    friend class NotationToolBox;

public:

    virtual void ready();

    virtual void handleLeftButtonPress(Rosegarden::timeT,
                                       int height,
                                       int staffNo,
                                       QMouseEvent*,
                                       Rosegarden::ViewElement* el);
    static const QString ToolName;

public slots:
    void slotToggleRestCollapse();
    
    void slotInsertSelected();
    void slotSelectSelected();

protected:
    NotationEraser(NotationView*);

    //--------------- Data members ---------------------------------

    bool m_collapseRest;
};

/**
 * Rectangular note selection
 */
class NotationSelector : public NotationTool
{
    Q_OBJECT

    friend class NotationToolBox;

public:

    ~NotationSelector();

    virtual void handleLeftButtonPress(Rosegarden::timeT,
                                       int height,
                                       int staffNo,
                                       QMouseEvent*,
                                       Rosegarden::ViewElement* el);

    virtual int handleMouseMove(Rosegarden::timeT,
                                int height,
                                QMouseEvent*);

    virtual void handleMouseRelease(Rosegarden::timeT time,
                                    int height,
                                    QMouseEvent*);

    virtual void handleMouseDoubleClick(Rosegarden::timeT,
					int height,
					int staffNo,
					QMouseEvent*,
					Rosegarden::ViewElement*);

    virtual void handleMouseTripleClick(Rosegarden::timeT,
					int height,
					int staffNo,
					QMouseEvent*,
					Rosegarden::ViewElement*);

    /**
     * Create the selection rect
     *
     * We need this because NotationView deletes all QCanvasItems
     * along with it. This happens before the NotationSelector is
     * deleted, so we can't delete the selection rect in
     * ~NotationSelector because that leads to double deletion.
     */
    virtual void ready();

    /**
     * Delete the selection rect.
     */
    virtual void stow();

    /**
     * Returns the currently selected events
     *
     * The returned result is owned by the caller
     */
    Rosegarden::EventSelection* getSelection();

    /**
     * Respond to an event being deleted -- it may be the one the tool
     * is remembering as the current event.
     */
    virtual void handleEventRemoved(Rosegarden::Event *event) {
	if (m_clickedElement && m_clickedElement->event() == event) {
	    m_clickedElement = 0;
	}
    }

    static const QString ToolName;

signals:
    void editElement(NotationStaff *, NotationElement *, bool advanced);

public slots:
    /**
     * Hide the selection rectangle
     *
     * Should be called after a cut or a copy has been
     * performed
     */
    void slotHideSelection();
    
    void slotInsertSelected();
    void slotEraseSelected();
//    void slotCollapseRests();
    void slotCollapseRestsHard();
    void slotRespellFlat();
    void slotRespellSharp();
    void slotRespellNatural();
    void slotCollapseNotes();
    void slotInterpret();
    void slotMakeInvisible();
    void slotMakeVisible();

    void slotClickTimeout();

protected:
    NotationSelector(NotationView*);

    /**
     * Set the current selection on the parent NotationView
     */
    void setViewCurrentSelection(bool preview);

    /**
     * Look up the staff containing the given notation element
     */
    NotationStaff *getStaffForElement(NotationElement *elt);

    void drag(int x, int y, bool final);
    void dragFine(int x, int y, bool final);

    //--------------- Data members ---------------------------------

    QCanvasRectangle* m_selectionRect;
    bool m_updateRect;

    NotationStaff *m_selectedStaff;
    NotationElement *m_clickedElement;
    bool m_clickedShift;
    bool m_startedFineDrag;

    Rosegarden::EventSelection *m_selectionToMerge;

    long m_lastDragPitch;
    Rosegarden::timeT m_lastDragTime;

    bool m_justSelectedBar;
    bool m_wholeStaffSelectionComplete;
};


/**
 * Selection pasting - unused at the moment
 */
class NotationSelectionPaster : public NotationTool
{
public:

    ~NotationSelectionPaster();
    
    virtual void handleLeftButtonPress(Rosegarden::timeT,
                                       int height, int staffNo,
                                       QMouseEvent*,
                                       Rosegarden::ViewElement* el);

protected:
    NotationSelectionPaster(Rosegarden::EventSelection&,
                            NotationView*);

    //--------------- Data members ---------------------------------

    Rosegarden::EventSelection& m_selection;

};


/**
 * This tool will insert fretboard on mouse click events
*/
class FretboardInserter : public NotationTool
{
    Q_OBJECT

    friend class NotationToolBox;

public:

    virtual void handleLeftButtonPress(Rosegarden::timeT t,
                                       int height,
                                       int staffNo,
                                       QMouseEvent* e,
                                       Rosegarden::ViewElement *element);

/*
    virtual void handleMouseDoubleClick(Rosegarden::timeT,
                                        int height, int staffNo,
                                        QMouseEvent*,
                                        Rosegarden::ViewElement* el);
*/

    static const QString ToolName;

protected slots:
    void slotFretboardSelected();
    void slotEraseSelected();
    void slotSelectSelected();

    GuitarTabSelectorDialog m_guitarChord_ref;

protected:
    FretboardInserter(NotationView*);

private:
    void handleSelectedFretboard (Rosegarden::ViewElement* element,
                                  Rosegarden::Staff *staff);

    void createNewFretboard (Rosegarden::ViewElement* element,
                             Rosegarden::Staff *staff,
                             QMouseEvent* e);

    bool processDialog (Rosegarden::Staff *staff,
                        Rosegarden::timeT& insertionTime);
};

#endif
