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
#ifndef __F670L_TRANSFORMER_HPP_362E05E5__
#define __F670L_TRANSFORMER_HPP_362E05E5__
//==============================================================================
#include "WDF++.hpp"
//==============================================================================
namespace Wavechild670 {
//==============================================================================
template <typename T>
class NonIdealTransformer : public WDF::TwoPort<T>
{
    public:
        NonIdealTransformer (T Fs,
                             T Nt,
                             T Lp_, T Rp_,
                             T Lm_, T Rc_,
                             T Ls_, T Rs_, T Cw_,
                             String name = String::empty)
            : OnePort (input.R, name),
              //----------------------------------------------------------------
              // Components
              //----------------------------------------------------------------
              Lp (Lp_, Fs, "Lp"),
              Lm (Lm_, Fs, "Lm"),
              Ls (Ls_, Fs, "Ls"),
              Cw (Cw_, Fs, "Cw"),
              Rp (Rp_,     "Rp"),
              Rc (Rc_,     "Rc"),
              Rs (Rs_,     "Rs"),
              //----------------------------------------------------------------
              transfo (Nt, "][")
              //----------------------------------------------------------------
        {}
        //----------------------------------------------------------------------
        virtual String label () const { "][" };
        //----------------------------------------------------------------------
        virtual void connect (OnePort<T>* parent, OnePort<T>* child)
        {
            root.connect (parent); wiring (child);
        }
        //----------------------------------------------------------------------
        virtual void connectParent (OnePort<T>* parent)
        {
            root.connect (parent);
        }
        //----------------------------------------------------------------------
        virtual void connectChild (OnePort<T>* child)
        {
            wiring (child);
        }
        //----------------------------------------------------------------------
        virtual inline void computeChildB ()
        {
        }
        //----------------------------------------------------------------------
        virtual inline void computeParentB ()
        {
        }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            return root.reflected ();
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            root.incident (wave);
        }
        //----------------------------------------------------------------------
        virtual T R () { return root.R (); }
        //----------------------------------------------------------------------
        inline T Vout () { return Cw.voltage(); }
        //----------------------------------------------------------------------
    protected:
        //----------------------------------------------------------------------
        WDF::IdealTransformer<T>    transfo;
        //----------------------------------------------------------------------
        WDF::Inductor<T>            Lp, Lm, Ls;
        WDF::Resistor<T>            Rp, Rc, Rs;
        WDF::Capacitor<T>           Cw;
        //----------------------------------------------------------------------
        WDF::Serie<T>               root;
        //----------------------------------------------------------------------
        WDF::Serie<T>               serie_A;
        WDF::Serie<T>               serie_B;
        WDF::Serie<T>               serie_C;
        WDF::Parallel<T>            paral_A;
        WDF::Parallel<T>            paral_B;
        WDF::Parallel<T>            paral_C;
        //----------------------------------------------------------------------
        inline void wiring (OnePort<T>* child)
        {
            serie_A.connect (&Lp,       &Rp);
            serie_B.connect (&Rs,       &Ls);
            paral_A.connect (&Lm,       &Rc);
            //------------------------------------------------------------------
            paral_B.connect (child,     &Cw);
            serie_C.connect (&paral_B,  &serie_B);
            //------------------------------------------------------------------
            transfo.connectChild (&serie_C);
            //------------------------------------------------------------------
            paral_C.connect (&transfo,  &paral_A);
               root.connect (&serie_A,  &paral_C);
        }
        //----------------------------------------------------------------------
};
//==============================================================================
template <typename T>
class InputCoupledTransformer : public WDF::OnePort<T>
{
    public:
        InputCoupledTransformer ()
            : OnePort (1.0)
        {}
        //----------------------------------------------------------------------
        virtual String label () const { "][" };
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            a = serie.reflected ();
            b = -a; // short circuit rules
            return transformer.Vout();
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            Vin.Vs = wave;
            serie.incident (b);
        }
        //----------------------------------------------------------------------
    protected:
        WDF::VoltageSource<T>       Vin;
        WDF::Resistor<T>            Rload;
        WDF::Resistor<T>            Rterm;
        //----------------------------------------------------------------------
	NonIdealTransformer<T>	    transformer;
        //----------------------------------------------------------------------
        WDF::Serie<T>               serie;
        WDF::Parallel<T>            paral;
        //----------------------------------------------------------------------
        inline void wiring ()
        {
            transformer.connectChild (&Rload);
            paral.connect (&transformer, &Rterm);
            serie.connect (&paral,       &Vin);
        }
        //----------------------------------------------------------------------
};
//==============================================================================
//
//                  SIGNAL AMPLIFIER TRANSFORMERS
//                  -----------------------------
//
//               INPUT (Tx10)             OUTPUT (Tx20)
//      --------------------------------------------------------
//      Rc       10 kOhms                 10 kOhms
//      Lm       35.7 H                   35.7 H
//      Rp       10 Ohms                  5 Ohms
//      Lp       4 mH                     100 uH
//      Rs       50 Ohms                  50 Ohms
//      Ls       1 mH                     400 uH
//      Cw       210 pF                   1 pF
//      --------------------------------------------------------
//      Ratio    1:9                      9:1
//      --------------------------------------------------------
//
//      T101    - Fairchild signal amp input    (mono)
//      T102    - Fairchild signal amp input    (stereo)
//      --------------------------------------------------------
//      T201    - Fairchild signal amp output   (mono)
//      T202    - Fairchild signal amp output   (stereo)
//
//      --------------------------------------------------------
//
//==============================================================================
/**
template <typename T>
class Tx10 : public Transformer<T> { public:
      Tx10 (T Fs, String name = String::empty) :
      Transformer (Fs, 9.0, 600.0, 1360.0, 10e3, 35.7,
        10.0, 4e-3, 50.0, 1e-3, 1000e3, 210e-12, name)
{} };
//==============================================================================
template <typename T>
class Tx20 : public Transformer<T> { public:
      Tx20 (T Fs, String name = String::empty) :
      Transformer (Fs, 1.0/9.0, 600.0, 1360.0, 10e3, 35.7,
        5.0, 100e-6, 50.0, 400e-6, 1000e3, 1e-12, name)
{} };
*/
//==============================================================================
//
//                SIDECHAIN AMPLIFIER TRANSFORMERS
//                --------------------------------
//
//               INPUT (T10x)             OUTPUT (T20x)
//      --------------------------------------------------------
//      Rc       10 kOhms                 10 kOhms
//      Lm       35.7 H                   35.7 H
//      Rp       10 Ohms                  5 Ohms
//      Lp       4 mH                     100 uH
//      Rs       50 Ohms                  50 Ohms
//      Ls       1 mH                     400 uH
//      Cw       210 pF                   1 pF
//      --------------------------------------------------------
//      Ratio    1:9                      9:1
//      --------------------------------------------------------
//
//      T101    - Fairchild signal amp input    (mono)
//      T102    - Fairchild signal amp input    (stereo)
//      --------------------------------------------------------
//      T201    - Fairchild signal amp output   (mono)
//      T202    - Fairchild signal amp output   (stereo)
//
//      --------------------------------------------------------
//
//==============================================================================
/**
template <typename T>
class Tx30 : public Transformer<T> { public:
      Tx30 (T Fs, String name = String::empty) :
      Transformer (Fs, 9.0, 600.0, 1360.0, 10e3, 35.7,
        10.0, 4e-3, 50.0, 1e-3, 1000e3, 210e-12, name)
{} };
*/
//==============================================================================
} // namespace Wavechild670
//==============================================================================
#endif  // __F670L_TRANSFORMER_HPP_362E05E5__
//==============================================================================
// TRANSFORMERS
//==============================================================================
// Fairchild 670 Signal amp input           T101/201    600 ohms/50k ohms. Ratio 1+1:9+9.
//------------------------------------------------------------------------------
// Fairchild 670 Signal amp output          T102/202    600 ohms/60k ct/ Ratio 9+9:1+1
//------------------------------------------------------------------------------
// Fairchild 670 Control amp input          T103/203    600 ohms/170k ohms. Ratio 17+17:1+1
//------------------------------------------------------------------------------
// Fairchild 670 Control amp output         T104/204    600 ohms/10k ct/ Ratio 4:1
//                                                      Feedback winding ratio 9.5:1
//------------------------------------------------------------------------------
// Fairchild 670 Bias supply Transformer.   T301        50/60 Hz
//               Secondary windings:                    375-0-375 V at 200 mA,
//                                                          6.3V CT at 5A,
//                                                          5.0V at 2A
//------------------------------------------------------------------------------
// Fairchild 670 Mains Transformer.         T302        50/60 Hz
//               Secondary windings:                    26.8V (for Selenium bridge rectifier)
//                                              tapped at 24V (for Silicon bridge rectifier) at 200 mA.
//------------------------------------------------------------------------------
// Fairchild 670 Heater Transformer.        T303/304    50/60 Hz
//               Secondary windings:                    6.3 V CT at 3 A
//                                                      6.3 at 2.6A
//------------------------------------------------------------------------------
// Fairchild 670 Bias supply Choke.         L301        71 Ohms 10H at 200 mA
//------------------------------------------------------------------------------
// Fairchild 670 Bias supply Choke.         L302        85 Ohms  5H at 200 mA
//==============================================================================