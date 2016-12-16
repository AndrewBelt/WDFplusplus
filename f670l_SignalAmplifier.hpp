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
#ifndef __F670L_SIGNALAMPLIFIER_HPP_F90DF875__
#define __F670L_SIGNALAMPLIFIER_HPP_F90DF875__
//==============================================================================
#include "WDF++.hpp"
//------------------------------------------------------------------------------
#include "f670l_NonIdealTransformer.hpp"
#include "f670l_TubeStage.hpp"
//==============================================================================
namespace Wavechild670 {
//==============================================================================
template &lt;typename T&gt; struct UnitDelay { T a, b; };
//==============================================================================
template &lt;typename T&gt;
class BidirectionnalUnitDelay
{
    public:
        //----------------------------------------------------------------------
        void process () 
        {
            unit1.b = unit2.a; 
            unit2.b = unit1.a; 
        }
        //----------------------------------------------------------------------
        UnitDelay&lt;T&gt; unit1;
        UnitDelay&lt;T&gt; unit2;
        //----------------------------------------------------------------------
};
//==============================================================================
template &lt;typename T&gt;
class TransformerInputCircuit
{
	public:
		TransformerInputCircuit ()
		{
		}
		//----------------------------------------------------------------------
};
//==============================================================================
template &lt;typename T&gt;
class SignalAmplifier : public WDF::OnePort&lt;T&gt;
{
    public:
        SignalAmplifier (T Fs)
            : //----------------------------------------------------------------
              WDF::OnePort&lt;T&gt; (1.0),
		cathodeTocathode (new BidirectionnalUnitDelay&lt;T&gt;()),
		transformer (new InputCoupledTransformer&lt;T&gt;()),
		push (new TubeStage&lt;T&gt;(Fs)),
		pull (new TubeStage&lt;T&gt;(Fs)),
		VgateBias (-7.2)
              //----------------------------------------------------------------
        {
	    push-&gt;wiring (pull);
	    pull-&gt;wiring (push);
	}
        //----------------------------------------------------------------------
        virtual String label () const { return "Amp"; }
	//----------------------------------------------------------------------
        virtual inline T process (T Vin, T VlevelCap)
        {
            T Vgate = transformer-&gt;process (Vin);
            T VoutPush = push-&gt;process (VgateBias - VlevelCap + Vgate);
            T VoutPull = pull-&gt;process (VgateBias - VlevelCap + Vgate);
            cathodeTocathode-&gt;process ();
            return VoutPush - VoutPull;
        }
	//----------------------------------------------------------------------
        virtual inline T reflected ()
        { 
            b = 0.0; return b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T value) 
        { 
	    a = value;
        }
        //----------------------------------------------------------------------
    protected:
        //----------------------------------------------------------------------
        // Fairchild 670 Class-A Signal Amplifier model
        //----------------------------------------------------------------------
        ScopedPointer&lt;BidirectionnalUnitDelay&lt;T&gt;&gt; cathodeTocathode;
        ScopedPointer&lt;InputCoupledTransformer&lt;T&gt;&gt; transformer;
        ScopedPointer&lt;TubeStage&lt;T&gt;&gt; push; // GE 6386
        ScopedPointer&lt;TubeStage&lt;T&gt;&gt; pull; // GE 6386
	T VgateBias;
        //----------------------------------------------------------------------
};
//==============================================================================
} // namespace Wavechild670 
//==============================================================================
#endif  // __F670L_SIGNALAMPLIFIER_HPP_F90DF875__
//==============================================================================