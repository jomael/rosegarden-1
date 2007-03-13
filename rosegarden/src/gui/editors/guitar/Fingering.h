/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*- vi:set ts=8 sts=4 sw=4: */

/*
    Rosegarden
    A MIDI and audio sequencer and musical notation editor.

    This program is Copyright 2000-2007
        Guillaume Laurent   <glaurent@telegraph-road.org>,
        Chris Cannam        <cannam@all-day-breakfast.com>,
        Richard Bown        <richard.bown@ferventsoftware.com>

    The moral rights of Guillaume Laurent, Chris Cannam, and Richard
    Bown to claim authorship of this work have been asserted.

    Other copyrights also apply to some parts of this work.  Please
    see the AUTHORS file and individual file headers for details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _RG_FINGERING_H_
#define _RG_FINGERING_H_

#include <vector>
#include <qstring.h>
#include "base/Event.h"

namespace Rosegarden
{

namespace Guitar
{

class Fingering
{
public:
    friend bool operator<(const Fingering&, const Fingering&);    

    typedef std::vector<int>::iterator iterator;
    typedef std::vector<int>::const_iterator const_iterator;
    
    struct Barre {
        unsigned int fret;
        unsigned int start;
        unsigned int end;
    };
    
    static const unsigned int DEFAULT_NB_STRINGS = 6;
    
	Fingering(unsigned int nbStrings = DEFAULT_NB_STRINGS);

    enum { MUTED = -1, OPEN = 0 };
    
    /**
     * returns the fret number on which the string is pressed, or one of MUTED and OPEN  
     * 
     */
    int  getStringStatus(int stringNb) const       { return m_strings[stringNb]; } 
    void setStringStatus(int stringNb, int status) { m_strings[stringNb] = status; } 
    unsigned int getStartFret() const;
    unsigned int getNbStrings() const { return m_strings.size(); }
        
    bool hasBarre() const;
    Barre getBarre() const;
    
    int operator[](int i) const { return m_strings[i]; }
    int& operator[](int i) { return m_strings[i]; }
    
    iterator begin() { return m_strings.begin(); }
    iterator end()   { return m_strings.end();   }
    const_iterator begin() const { return m_strings.begin(); }
    const_iterator end()   const { return m_strings.end(); }
    
    static Fingering parseFingering(const QString&, QString& errorString);
    std::string toString() const;
    
protected:

    std::vector<int> m_strings;
};

bool operator<(const Fingering&, const Fingering&);    

}

}

#endif /*_RG_FINGERING2_H_*/
