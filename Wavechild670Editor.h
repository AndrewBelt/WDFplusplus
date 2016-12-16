//==============================================================================
/**
    Wavechild 670
    -------------
    Wave digital filter based emulation of a famous 1950's tube stereo limiter

    WDF++ based source code by Maxime Coorevits (Nord, France) in 2013

    Some part are inspired by the Peter Raffensperger project: Wavechild670,
    a command line with python WDF generator that produce C++ code of the circuit.

    Major restructuration:
    ----------------------
        * WDF++ based project (single WDF++.hpp file)
        * full C++, zero-dependencies except JUCE (core API, AudioProcessor).
        * JUCE Plugin wrapper processor (VST, AU ...)
        * Photo-Realistic GUI

    Reference:
    ----------
    Toward a Wave Digital Filter Model of the Fairchild 670 Limiter,
    Raffensperger, P. A., (2012).
    Proc. of the 15th International Conference on Digital Audio Effects (DAFx-12),
    York, UK, September 17-21, 2012.
    Note:
    -----
    Fairchild (R) a registered trademark of Avid Technology, Inc.,
    which is in no way associated or affiliated with the author.

**/
//==============================================================================
#ifndef __WAVECHILD670EDITOR_H_6B9A985__
#define __WAVECHILD670EDITOR_H_6B9A985__
//==============================================================================
#include "../JuceLibraryCode/JuceHeader.h"
//==============================================================================
class Wavechild670Processor;
//==============================================================================
class Wavechild670Editor : public AudioProcessorEditor
{
    public:
        Wavechild670Editor (Wavechild670Processor* ownerFilter);
        ~Wavechild670Editor ();
        //----------------------------------------------------------------------
        void resized ();
        void paint (Graphics& g);
        //======================================================================
    private:
        ScopedPointer<Wavechild670Processor> processor;
        //======================================================================
};
//==============================================================================
#endif  // __WAVECHILD670EDITOR_H_6B9A985__
//==============================================================================