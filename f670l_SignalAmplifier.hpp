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
template <typename T> struct UnitDelay { T a, b; };
//==============================================================================
template <typename T>
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
        UnitDelay<T> unit1;
        UnitDelay<T> unit2;
        //----------------------------------------------------------------------
};
//==============================================================================
template <typename T>
class TransformerInputCircuit
{
	public:
		TransformerInputCircuit ()
		{
		}
		//----------------------------------------------------------------------
};
//==============================================================================
template <typename T>
class SignalAmplifier : public WDF::OnePort<T>
{
    public:
        SignalAmplifier (T Fs)
            : //----------------------------------------------------------------
              WDF::OnePort<T> (1.0),
		cathodeTocathode (new BidirectionnalUnitDelay<T>()),
		transformer (new InputCoupledTransformer<T>()),
		push (new TubeStage<T>(Fs)),
		pull (new TubeStage<T>(Fs)),
		VgateBias (-7.2)
              //----------------------------------------------------------------
        {
	    push->wiring (pull);
	    pull->wiring (push);
	}
        //----------------------------------------------------------------------
        virtual String label () const { return "Amp"; }
	//----------------------------------------------------------------------
        virtual inline T process (T Vin, T VlevelCap)
        {
            T Vgate = transformer->process (Vin);
            T VoutPush = push->process (VgateBias - VlevelCap + Vgate);
            T VoutPull = pull->process (VgateBias - VlevelCap + Vgate);
            cathodeTocathode->process ();
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
        ScopedPointer<BidirectionnalUnitDelay<T>> cathodeTocathode;
        ScopedPointer<InputCoupledTransformer<T>> transformer;
        ScopedPointer<TubeStage<T>> push; // GE 6386
        ScopedPointer<TubeStage<T>> pull; // GE 6386
	T VgateBias;
        //----------------------------------------------------------------------
};
//==============================================================================
} // namespace Wavechild670
//==============================================================================
#endif  // __F670L_SIGNALAMPLIFIER_HPP_F90DF875__
//==============================================================================