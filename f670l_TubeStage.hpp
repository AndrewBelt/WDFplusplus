//==============================================================================
/**
    Wavechild 670
    -------------
    Wave digital filter based emulation of a famous 1950's tube stereo limiter

    WDF++ based source code by Maxime Coorevits (Nord, France) in 2013

    Some part are inspired by the Peter Raffensperger project: Wavechild670,
    a command line with python WDF generator that produce C++ code of the circuits.

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
#ifndef __F670L_TUBE_STAGE_HPP_F155F0B__
#define __F670L_TUBE_STAGE_HPP_F155F0B__
//==============================================================================
#include "WDF++.hpp"
//------------------------------------------------------------------------------
#include "f670l_NonIdealTransformer.hpp"
//==============================================================================
namespace Wavechild670 {
//==============================================================================
template <typename T>
class TubeStage : public WDF::OnePort<T>, WDF::NewtonRaphson<T>
{
    public:
        TubeStage (T Fs)
            : //----------------------------------------------------------------
              // Components
              //----------------------------------------------------------------
              Rout (600.0,      "Rout"),      // signal output
              Rsc (1000.0,      "Rsc"),       // sidechain input
              //----------------------------------------------------------------
              Ck (2.0*4e-6, Fs, "2C1"),       // cathode capacitor (twice)
              Vk (-3.1,  705.0, "Vbal R11"),  // cathode (balance)
              Vp (240.0,  33.0, "240V R12"),  // plate (power supply)
              //----------------------------------------------------------------
              Vgk (0.0), Iak (0.0), lVk (0.0) // lVk = last Vk (cathode voltage)
              //----------------------------------------------------------------
        {
        }
        //----------------------------------------------------------------------
        virtual String label () const { return "Tube"; }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            a = root.reflected (); return a;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            b = nonlinear (wave);
            root.incident (b);
            lVk = Vk.voltage (); // z-1
        }
        //----------------------------------------------------------------------
        inline T process (T Vgate)
        {
            reflected ();
            incident (Vgate);
            return transformer.Vout();
        }
        //----------------------------------------------------------------------
        // Fairchild 670 Class-A Signal Amplifier (with Push/Pull topology)
        //----------------------------------------------------------------------
        inline void wiring (WDF::OnePort<T>* coupled)
        {
            paral_O.connect (&Rout,    &Rsc);
            transfo.connect (&serie_T, &paral_O);
            serie_T.connect (&transfo, &Vp);
            //------------------------------------------------------------------
            serie_K.connect (&Ck,       coupled);
            paral_K.connect (&Vk,      &serie_K);
            //------------------------------------------------------------------
               root.connect (&serie_T, &paral_K);
        }
        //----------------------------------------------------------------------
    protected:
        //----------------------------------------------------------------------
        WDF::VoltageSource<T>   Vk;
        WDF::VoltageSource<T>   Vp;
        WDF::Capacitor<T>       Ck;
        WDF::Resistor<T>        Rout;
        WDF::Resistor<T>        Rsc;
        //----------------------------------------------------------------------
        NonIdealTransformer<T>  transfo;
        //----------------------------------------------------------------------
        WDF::Serie<T>           root;
        WDF::Serie<T>           serie_K;
        WDF::Serie<T>           serie_T;
        WDF::Parallel<T>        paral_O;
        WDF::Parallel<T>        paral_K;
        //----------------------------------------------------------------------
    private:
        //----------------------------------------------------------------------
        // nonlinearity is isolated at trunk (wdf requirement)
        //----------------------------------------------------------------------
        inline T nonlinear (T Vgate)
        {
            Vgk = Vgate - lVk;              // grid-cathode voltage
            //------------------------------------------------------------------
            Iak = 0.0;                      // computed by solve()
            T Vak = solve ();               // Newton/Raphson iterations
            //------------------------------------------------------------------
            return Vak - root.R()*Iak;      // estimate reflected
        }
        //----------------------------------------------------------------------
        // implicit equation will be evaluate by Newton/Raphson solver
        //----------------------------------------------------------------------
        virtual inline T evaluate (T Vak)
        {
            Iak = Ia (Vgk, Vak) * NTI;
            return Vak + R*Iak - a;         // [ Vak + R*Iak - a = 0 ]
        }
        //----------------------------------------------------------------------
        // GE 6386 Remote Cutoff Triode
        //----------------------------------------------------------------------
        // The model parameters were calculated using Levenberg-Marquardt least
        // squares estimation and hand tuning to fit the 6386 characteristics
        // as given in the General Electric 6386 datasheet.
        // by Peter Raffensperger (2012)
        //----------------------------------------------------------------------
        inline T Ia (T Vgk, T Vak)          // Ia = anode current (in amps)
        {
            if (Vak < 0.0) Vak = 0.0;
            if (Vgk > 0.0) Vgk = 0.0;
            //------------------------------------------------------------------
            return (3.981e-8 * pow(Vak, 2.383))
                 / (pow((0.5 - 0.1*Vgk), 1.8)
                 * (0.5 + exp((-0.03922*Vak)
                 - (0.2*Vgk))));
        }
        //----------------------------------------------------------------------
    private:
        T Vgk, Iak, lVk;
        //----------------------------------------------------------------------
};
//==============================================================================
} // namespace Wavechild670
//==============================================================================
#endif  // __F670L_TUBE_STAGE_HPP_F155F0B__
//==============================================================================