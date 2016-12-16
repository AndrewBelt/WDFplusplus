//==============================================================================
/**
    WDF++ "A easy-to-use collection of C++ classes for Wave Digital Filter WDF"
    ----------------------------------------------------------------------------
    Author : Maxime Coorevits (Nord, France).
    ----------------------------------------------------------------------------
    Official project location: no hub yet.
    ----------------------------------------------------------------------------
    License: MIT License (http://www.opensource.org/licenses/mit-license.php)
    Copyright (c) 2013 by Maxime Coorevits.
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
    ----------------------------------------------------------------------------
*/
//==============================================================================
#ifndef __WDF_DEFINITION_HPP_870F9F26__
#define __WDF_DEFINITION_HPP_870F9F26__
//==============================================================================
#include "../JuceLibraryCode/JuceHeader.h"
//==============================================================================
namespace WDF {
//==============================================================================
// ** 1-PORT ** (base class for every WDF classes)
//==============================================================================
template <typename T>
class OnePort
{
    public:
        OnePort (T R, String n = String::empty)
            : name (n), Rp (R), a (0), b (0), port(this) {}
        //----------------------------------------------------------------------
        virtual String name () const { return name.isEmpty() ? label() : _name; }
        virtual String label () const = 0;
        //----------------------------------------------------------------------
        virtual inline void incident (T wave) = 0;
        //----------------------------------------------------------------------
        virtual inline T reflected () = 0;
        //----------------------------------------------------------------------
        virtual T R () { return port->Rp; }       // Port resistance
        virtual T G () { return 1.0 / port->Rp; } // Port conductance (inv.Rp)
        //----------------------------------------------------------------------
        virtual void connect (OnePort<T>* other)
        {
            port = other;
            other->port = this;
        }
        //----------------------------------------------------------------------
        T voltage () // v
        {
            return (port->a + port->b) / 2.0;
        }
        //----------------------------------------------------------------------
        T current () // i
        {
            return (port->a - port->b) / (port->Rp + port->Rp);
        }
        //----------------------------------------------------------------------
    protected:
        //----------------------------------------------------------------------
        String _name; // Port name
        T Rp; // Port resistance
        //----------------------------------------------------------------------
        T a; // incident wave (incoming wave)
        T b; // reflected wave (outgoing wave)
        //----------------------------------------------------------------------
        OnePort<T>* port; // internal pointer (used for direct connect form)
        //----------------------------------------------------------------------
};
//==============================================================================
// ** 2-PORT **
//==============================================================================
template <typename T>
class TwoPort : public OnePort<T> // parent
{
    public:
        OnePort<T>* child;
        //----------------------------------------------------------------------
        TwoPort (String name = String::empty)
            : OnePort (1.0, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "2P"; }
        //----------------------------------------------------------------------
        virtual void connectParent (OnePort<T>* parent)
        {
            OnePort::connect (parent);
        }
        //----------------------------------------------------------------------
        virtual void connectChild (OnePort<T>* port) = 0;
        //----------------------------------------------------------------------
        virtual void connect (OnePort<T>* p, OnePort<T>* c)
        {
            OnePort::connect (p);
            connectChild (c);
        }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            child.port->a = child.port->reflected ();
            computeParentB ();
            return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            port->a = wave;
            computeChildB ();
            child.port->incident (child.port->b);
        }
        //----------------------------------------------------------------------
        virtual inline void computeChildB () = 0;
        virtual inline void computeParentB () = 0;
        //----------------------------------------------------------------------
};
//==============================================================================
// ** 3-PORT **
//==============================================================================
template <typename T>
class ThreePort : public OnePort<T> // adapted
{
    public:
        OnePort<T> *left, *right;
        //----------------------------------------------------------------------
        ThreePort (String name = String::empty)
            : left (nullptr), right (nullptr), OnePort (1.0, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "3P"; }
        //----------------------------------------------------------------------
        virtual void connect (OnePort<T>* left, OnePort<T>* right) = 0;
        //----------------------------------------------------------------------
        virtual inline T reflected () = 0;
        virtual inline void incident (T wave) = 0;
        //----------------------------------------------------------------------
};
//==============================================================================
// ** SERIE **
//==============================================================================
template <typename T>
class Serie : public ThreePort<T>
{
    public:
        Serie (String name = "--")
            : ThreePort (name)
        {}
        //----------------------------------------------------------------------
        virtual String label () const { return "--"; }
        //----------------------------------------------------------------------
        virtual void connect (OnePort<T>* l, OnePort<T>* r)
        {
            left = l; right = r;
            port->Rp = (left->R() + right->R());
        }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            port->b = -(left->port->reflected()
                     + right->port->reflected());
            return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            register T lrW = (wave + left->port->b + right->port->b);
             left->port->incident ( left->port->b - ( left->R()/port->R()) * lrW);
            right->port->incident (right->port->b - (right->R()/port->R()) * lrW);
            port->a = wave;
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** PARALLEL **
//==============================================================================
template <typename T>
class Parallel : public ThreePort<T>
{
    public:
        Parallel (String name = "||")
            : ThreePort (name)
        {}
        //----------------------------------------------------------------------
        virtual String label () const { return "||"; }
        //----------------------------------------------------------------------
        virtual void connect (OnePort<T>* l, OnePort<T>* r)
        {
            left = l; right = r;
            port->Rp = (left->R() * right->R())
                     / (left->R() + right->R());
        }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            register T lrG = left->G() + right->G();
            port->b = ( left->G()/lrG) *  left->port->reflected() +
                      (right->G()/lrG) * right->port->reflected();
            return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            register T lrG = left->G() + right->G();
            register T lrW = (wave + left->port->b + right->port->b);
             left->port->incident ( left->port->b - ( left->G()/lrG) * lrW);
            right->port->incident (right->port->b - (right->G()/lrG) * lrW);
            port->a = wave;
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** RESISTOR **
//==============================================================================
template <typename T>
class Resistor : public OnePort<T>
{
    public:
        Resistor (T R, String name = String::empty)
            : OnePort (R, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "R"; }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            port->b = 0; return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            port->a = wave;
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** CAPACITOR **
//==============================================================================
template <typename T>
class Capacitor : public OnePort<T>
{
    public:
        Capacitor (T C, T Fs, String name = String::empty)
            : OnePort (Fs/2.0*C, name), state (0) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "C"; }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            port->b = state; return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            port->a = wave; state = port->a;
        }
        //----------------------------------------------------------------------
    private:
        T state;
        //----------------------------------------------------------------------
};
//==============================================================================
// ** INDUCTOR **
//==============================================================================
template <typename T>
class Inductor : public OnePort<T>
{
    public:
        Inductor (T L, T Fs, String name = String::empty)
            : OnePort (2.0*L/Fs, name), state (0) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "L"; }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            port->b = -state; return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            port->a = wave; state = port->a;
        }
        //----------------------------------------------------------------------
    private:
        T state;
        //----------------------------------------------------------------------
};
//==============================================================================
// ** OPEN CIRCUIT **
//==============================================================================
template <typename T>
class OpenCircuit : public OnePort<T>
{
    public:
        OpenCircuit (T R, String name = String::empty)
            : OnePort (R, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "Oc"; }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            port->b = port->a; return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            port->a = wave;
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** SHORT CIRCUIT **
//==============================================================================
template <typename T>
class ShortCircuit : public OnePort<T>
{
    public:
        ShortCircuit (T R, String name = String::empty)
            : OnePort (R, name) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "Sc"; }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            port->b = -port->a; return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            port->a = wave;
        }
        //----------------------------------------------------------------------
};
//==============================================================================
// ** VOLTAGE SOURCE **
//==============================================================================
template <typename T>
class VoltageSource : public OnePort<T>
{
    public:
        VoltageSource (T V, T R, String name = String::empty)
            : OnePort (R, name), Vs (V) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "Vs"; }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            port->b = -port->a + 2.0 * Vs; return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            port->a = wave;
        }
        //----------------------------------------------------------------------
    private:
        T Vs;
        //----------------------------------------------------------------------
};
//==============================================================================
// ** CURRENT SOURCE **
//==============================================================================
template <typename T>
class CurrentSource : public OnePort<T>
{
    public:
        CurrentSource (T I, T R, String name = String::empty)
            : OnePort (R, name), Is (I) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "Is"; }
        //----------------------------------------------------------------------
        virtual inline T reflected ()
        {
            port->b = port->a + 2.0 * R() * Is; return port->b;
        }
        //----------------------------------------------------------------------
        virtual inline void incident (T wave)
        {
            port->a = wave;
        }
        //----------------------------------------------------------------------
    private:
        T Is;
        //----------------------------------------------------------------------
};
//==============================================================================
// ** IDEAL TRANSFORMER **
//==============================================================================
template <typename T>
class IdealTransformer : public TwoPort<T>
{
    public:
        IdealTransformer (T ratio, String name = String::empty)
            : TwoPort (1.0, name), N (ratio) {}
        //----------------------------------------------------------------------
        virtual String label () const { return "][" }
        //----------------------------------------------------------------------
        virtual void connectChild (OnePort<T>* port)
        {
            T Rs = port.R();
            port->Rp = Rs / (n*n);
            child.Rp = Rs;
            TwoPort::connectChild (port);
        }
        //----------------------------------------------------------------------
        virtual inline void computeChildB ()
        {
            child.port->b = port->a * (1.0/N);
        }
        //----------------------------------------------------------------------
        virtual inline void computeParentB ()
        {
            port->b = child.port->a * N;
        }
        //----------------------------------------------------------------------
    private:
        T N;
        //----------------------------------------------------------------------
};
//==============================================================================
/**
    EXTRA TEMPLATES
    ---------------

    Not direct WDF classes but interfaces for your non-linear blackboxes.

**/
//==============================================================================
// ** Newton/Raphson ** (implicit equation solver)
//==============================================================================
template <typename T>
class NewtonRaphson
{
    public:
        NewtonRaphson (T guess = 100.0) : xguess (guess) {}
        //----------------------------------------------------------------------
        inline T solve (int max_iter = 100, T epsilon = 1e-9)
        {
            T x = xguess;
            T err = 1e6;
            int iteration = 0;
            while (fabs(err) / fabs(x) > epsilon)
            {
                xguess = iterate (x);
                err = x - xguess;
                x = xguess;
                if (iteration > max_iter) break;
                ++iteration;
            }
            return x;
        }
        //----------------------------------------------------------------------
        inline T iterate (T x, T dx = 1e-6)
        {
            T F = evaluate (x);
            T xNew = x - dx*F / (evaluate (x + dx) - F);
            return xNew;
        }
        //----------------------------------------------------------------------
        virtual inline T evaluate (T x) = 0; // declare your implicit equation
        //----------------------------------------------------------------------
    private :
        T xguess;
        //----------------------------------------------------------------------
};
//==============================================================================
} // namespace WDF
//==============================================================================
#endif  // __WDF_DEFINITION_HPP_870F9F26__
//==============================================================================