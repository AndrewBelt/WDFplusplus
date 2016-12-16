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
#ifndef __F670L_SIDECHAINAMPLIFIER_HPP_77745FAF__
#define __F670L_SIDECHAINAMPLIFIER_HPP_77745FAF__
//==============================================================================
#include "WDF++.hpp"
//------------------------------------------------------------------------------
#include "f670l_NonIdealTransformer.hpp"
//==============================================================================
namespace Wavechild670 {
//==============================================================================
template &lt;typename T&gt;
class SidechainAmplifier
{
    public:
        SidechainAmplifier (T Fs)
            : AC (0.5), DC (0.1)
        {}
        //----------------------------------------------------------------------
        void parameters (T ACThreshold, T DCThreshold)
        {
            DC = 12.2 * (DCThreshold + 0.1);
            AC = 0.5 * ACThreshold * ACThreshold;
        }
        //----------------------------------------------------------------------
        // Fairchild 670 Class-B Sidechain Amplifier model
	//----------------------------------------------------------------------
        virtual inline T process (T Vsc, T VlevelCap)
        { 
	    //------------------------------------------------------------------
	    // AC Threshold Input Transformer
	    //------------------------------------------------------------------
            Vpot = AC * transformer.process (Vsc);
	    //------------------------------------------------------------------
            // DC Threshold Vsc Stage, 12AX7 amplifier
	    //------------------------------------------------------------------
            Vs1 = -6.0 * ((log(1.0 + exp( Vpot - DC)))
                        - (log(1.0 + exp(-Vpot - DC))));
	    //------------------------------------------------------------------
            // Drive stage, 12BH7 + 6973 amplifier stages
	    //------------------------------------------------------------------
	    Vdiff = fabs (hardclip (8.4 * Vs1, -100.0, 100.0)) - VlevelCap; 
	    //------------------------------------------------------------------
            // The nominal output current through the bridge rectifier 
            // is calculated using a diode model in series with a resistance.
	    //------------------------------------------------------------------
	    Inom = 0.000375 * log(1.0 + exp(((10.0 * Vdiff) / 0.6) - 10.0)) * 0.0125;
	    //------------------------------------------------------------------
	    // One side-saturation (does not saturate negatives)
	    //------------------------------------------------------------------
	    return Inom - 0.05 * log(1.0 + exp(((10.0 * Inom) / 0.5) - 10.0));
        }
	//----------------------------------------------------------------------
        inline T hardclip (T x, T min, T max) { return (x &lt; min) ? min 
                                                     : (x &gt; max) ? max 
                                                     :             x; }
        //----------------------------------------------------------------------
    protected:
        T DC, AC;
        T Vpot, Vs1, Vdiff, Inom;
        //----------------------------------------------------------------------
};
//==============================================================================
} // namespace Wavechild670 
//==============================================================================
#endif  // __F670L_SIDECHAINAMPLIFIER_HPP_77745FAF__
//==============================================================================