/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define floor(x) ((long)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class rnbomatic : public PatcherInterfaceImpl {
public:

rnbomatic()
{
}

~rnbomatic()
{
}

rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

inline number cubicinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = z - y - w + x;
    number f1 = w - x - f0;
    number f2 = y - w;
    number f3 = x;
    return f0 * a * a2 + f1 * a2 + f2 * a + f3;
}

inline number splineinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = -0.5 * w + 1.5 * x - 1.5 * y + 0.5 * z;
    number f1 = w - 2.5 * x + 2 * y - 0.5 * z;
    number f2 = -0.5 * w + 0.5 * y;
    return f0 * a * a2 + f1 * a2 + f2 * a + x;
}

inline number cosT8(number r) {
    number t84 = 56.0;
    number t83 = 1680.0;
    number t82 = 20160.0;
    number t81 = 2.4801587302e-05;
    number t73 = 42.0;
    number t72 = 840.0;
    number t71 = 1.9841269841e-04;

    if (r < 0.785398163397448309615660845819875721 && r > -0.785398163397448309615660845819875721) {
        number rr = r * r;
        return 1.0 - rr * t81 * (t82 - rr * (t83 - rr * (t84 - rr)));
    } else if (r > 0.0) {
        r -= 1.57079632679489661923132169163975144;
        number rr = r * r;
        return -r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    } else {
        r += 1.57079632679489661923132169163975144;
        number rr = r * r;
        return r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    }
}

inline number cosineinterp(number frac, number x, number y) {
    number a2 = (1.0 - this->cosT8(frac * 3.14159265358979323846)) / (number)2.0;
    return x * (1.0 - a2) + y * a2;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

number samplerate() {
    return this->sr;
}

Index vectorsize() {
    return this->vs;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number wrap(number x, number low, number high) {
    number lo;
    number hi;

    if (low == high)
        return low;

    if (low > high) {
        hi = low;
        lo = high;
    } else {
        lo = low;
        hi = high;
    }

    number range = hi - lo;

    if (x >= lo && x < hi)
        return x;

    if (range <= 0.000000001)
        return lo;

    long numWraps = (long)(rnbo_trunc((x - lo) / range));
    numWraps = numWraps - ((x < lo ? 1 : 0));
    number result = x - range * numWraps;

    if (result >= hi)
        return result - range;
    else
        return result;
}

MillisecondTime currenttime() {
    return this->_currentTime;
}

number tempo() {
    return this->getTopLevelPatcher()->globaltransport_getTempo(this->currenttime());
}

number mstobeats(number ms) {
    return ms * this->tempo() * 0.008 / (number)480;
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    const SampleValue * in2 = (numInputs >= 2 && inputs[1] ? inputs[1] : this->zeroBuffer);
    this->dspexpr_02_perform(in1, this->dspexpr_02_in2, this->signals[0], n);
    this->dspexpr_04_perform(in2, this->dspexpr_04_in2, this->signals[1], n);

    this->gen_01_perform(
        this->signals[0],
        this->signals[1],
        this->gen_01_modRate,
        this->gen_01_modAmount,
        this->gen_01_roomSize,
        this->gen_01_deDiff1,
        this->gen_01_damping,
        this->gen_01_bandwidth,
        this->gen_01_decay,
        this->gen_01_inDiff2,
        this->gen_01_predelay,
        this->gen_01_sizeSpeed,
        this->gen_01_inDiff1,
        this->signals[2],
        this->signals[3],
        n
    );

    this->dspexpr_01_perform(this->signals[2], this->dspexpr_01_in2, out1, n);
    this->dspexpr_03_perform(this->signals[3], this->dspexpr_03_in2, out2, n);
    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 4; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->gen_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->gen_01_delay_13_bufferobj);
        break;
        }
    case 1:
        {
        return addressOf(this->gen_01_delay_12_bufferobj);
        break;
        }
    case 2:
        {
        return addressOf(this->gen_01_delay_11_bufferobj);
        break;
        }
    case 3:
        {
        return addressOf(this->gen_01_delay_10_bufferobj);
        break;
        }
    case 4:
        {
        return addressOf(this->gen_01_delay_9_bufferobj);
        break;
        }
    case 5:
        {
        return addressOf(this->gen_01_delay_8_bufferobj);
        break;
        }
    case 6:
        {
        return addressOf(this->gen_01_delay_7_bufferobj);
        break;
        }
    case 7:
        {
        return addressOf(this->gen_01_delay_6_bufferobj);
        break;
        }
    case 8:
        {
        return addressOf(this->gen_01_delay_5_bufferobj);
        break;
        }
    case 9:
        {
        return addressOf(this->gen_01_delay_4_bufferobj);
        break;
        }
    case 10:
        {
        return addressOf(this->gen_01_delay_3_bufferobj);
        break;
        }
    case 11:
        {
        return addressOf(this->gen_01_delay_2_bufferobj);
        break;
        }
    case 12:
        {
        return addressOf(this->gen_01_delay_1_bufferobj);
        break;
        }
    case 13:
        {
        return addressOf(this->RNBODefaultSinus);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 14;
}

void fillRNBODefaultSinus(DataRef& ref) {
    Float64BufferRef buffer;
    buffer = new Float64Buffer(ref);
    number bufsize = buffer->getSize();

    for (Index i = 0; i < bufsize; i++) {
        buffer[i] = rnbo_cos(i * 3.14159265358979323846 * 2. / bufsize);
    }
}

void fillDataRef(DataRefIndex index, DataRef& ref) {
    switch (index) {
    case 13:
        {
        this->fillRNBODefaultSinus(ref);
        break;
        }
    }
}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->gen_01_delay_13_buffer = new Float64Buffer(this->gen_01_delay_13_bufferobj);
    }

    if (index == 1) {
        this->gen_01_delay_12_buffer = new Float64Buffer(this->gen_01_delay_12_bufferobj);
    }

    if (index == 2) {
        this->gen_01_delay_11_buffer = new Float64Buffer(this->gen_01_delay_11_bufferobj);
    }

    if (index == 3) {
        this->gen_01_delay_10_buffer = new Float64Buffer(this->gen_01_delay_10_bufferobj);
    }

    if (index == 4) {
        this->gen_01_delay_9_buffer = new Float64Buffer(this->gen_01_delay_9_bufferobj);
    }

    if (index == 5) {
        this->gen_01_delay_8_buffer = new Float64Buffer(this->gen_01_delay_8_bufferobj);
    }

    if (index == 6) {
        this->gen_01_delay_7_buffer = new Float64Buffer(this->gen_01_delay_7_bufferobj);
    }

    if (index == 7) {
        this->gen_01_delay_6_buffer = new Float64Buffer(this->gen_01_delay_6_bufferobj);
    }

    if (index == 8) {
        this->gen_01_delay_5_buffer = new Float64Buffer(this->gen_01_delay_5_bufferobj);
    }

    if (index == 9) {
        this->gen_01_delay_4_buffer = new Float64Buffer(this->gen_01_delay_4_bufferobj);
    }

    if (index == 10) {
        this->gen_01_delay_3_buffer = new Float64Buffer(this->gen_01_delay_3_bufferobj);
    }

    if (index == 11) {
        this->gen_01_delay_2_buffer = new Float64Buffer(this->gen_01_delay_2_bufferobj);
    }

    if (index == 12) {
        this->gen_01_delay_1_buffer = new Float64Buffer(this->gen_01_delay_1_bufferobj);
    }

    if (index == 13) {
        this->gen_01_cycle_187_buffer = new Float64Buffer(this->RNBODefaultSinus);
        this->gen_01_cycle_214_buffer = new Float64Buffer(this->RNBODefaultSinus);
    }
}

void initialize() {
    this->gen_01_delay_13_bufferobj = initDataRef("gen_01_delay_13_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_12_bufferobj = initDataRef("gen_01_delay_12_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_11_bufferobj = initDataRef("gen_01_delay_11_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_10_bufferobj = initDataRef("gen_01_delay_10_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_9_bufferobj = initDataRef("gen_01_delay_9_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_8_bufferobj = initDataRef("gen_01_delay_8_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_7_bufferobj = initDataRef("gen_01_delay_7_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_6_bufferobj = initDataRef("gen_01_delay_6_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_5_bufferobj = initDataRef("gen_01_delay_5_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_4_bufferobj = initDataRef("gen_01_delay_4_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_3_bufferobj = initDataRef("gen_01_delay_3_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_2_bufferobj = initDataRef("gen_01_delay_2_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delay_1_bufferobj = initDataRef("gen_01_delay_1_bufferobj", true, nullptr, "buffer~");
    this->RNBODefaultSinus = initDataRef("RNBODefaultSinus", true, nullptr, "buffer~");
    this->assign_defaults();
    this->setState();
    this->gen_01_delay_13_bufferobj->setIndex(0);
    this->gen_01_delay_13_buffer = new Float64Buffer(this->gen_01_delay_13_bufferobj);
    this->gen_01_delay_12_bufferobj->setIndex(1);
    this->gen_01_delay_12_buffer = new Float64Buffer(this->gen_01_delay_12_bufferobj);
    this->gen_01_delay_11_bufferobj->setIndex(2);
    this->gen_01_delay_11_buffer = new Float64Buffer(this->gen_01_delay_11_bufferobj);
    this->gen_01_delay_10_bufferobj->setIndex(3);
    this->gen_01_delay_10_buffer = new Float64Buffer(this->gen_01_delay_10_bufferobj);
    this->gen_01_delay_9_bufferobj->setIndex(4);
    this->gen_01_delay_9_buffer = new Float64Buffer(this->gen_01_delay_9_bufferobj);
    this->gen_01_delay_8_bufferobj->setIndex(5);
    this->gen_01_delay_8_buffer = new Float64Buffer(this->gen_01_delay_8_bufferobj);
    this->gen_01_delay_7_bufferobj->setIndex(6);
    this->gen_01_delay_7_buffer = new Float64Buffer(this->gen_01_delay_7_bufferobj);
    this->gen_01_delay_6_bufferobj->setIndex(7);
    this->gen_01_delay_6_buffer = new Float64Buffer(this->gen_01_delay_6_bufferobj);
    this->gen_01_delay_5_bufferobj->setIndex(8);
    this->gen_01_delay_5_buffer = new Float64Buffer(this->gen_01_delay_5_bufferobj);
    this->gen_01_delay_4_bufferobj->setIndex(9);
    this->gen_01_delay_4_buffer = new Float64Buffer(this->gen_01_delay_4_bufferobj);
    this->gen_01_delay_3_bufferobj->setIndex(10);
    this->gen_01_delay_3_buffer = new Float64Buffer(this->gen_01_delay_3_bufferobj);
    this->gen_01_delay_2_bufferobj->setIndex(11);
    this->gen_01_delay_2_buffer = new Float64Buffer(this->gen_01_delay_2_bufferobj);
    this->gen_01_delay_1_bufferobj->setIndex(12);
    this->gen_01_delay_1_buffer = new Float64Buffer(this->gen_01_delay_1_bufferobj);
    this->RNBODefaultSinus->setIndex(13);
    this->gen_01_cycle_187_buffer = new Float64Buffer(this->RNBODefaultSinus);
    this->gen_01_cycle_214_buffer = new Float64Buffer(this->RNBODefaultSinus);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "modRate"));
    this->param_02_getPresetValue(getSubState(preset, "modAmount"));
    this->param_03_getPresetValue(getSubState(preset, "sizeSpeed"));
    this->param_04_getPresetValue(getSubState(preset, "decay"));
    this->param_05_getPresetValue(getSubState(preset, "damping"));
    this->param_06_getPresetValue(getSubState(preset, "deDiff1"));
    this->param_07_getPresetValue(getSubState(preset, "inDiff2"));
    this->param_08_getPresetValue(getSubState(preset, "inDiff1"));
    this->param_09_getPresetValue(getSubState(preset, "roomSize"));
    this->param_10_getPresetValue(getSubState(preset, "bandwidth"));
    this->param_11_getPresetValue(getSubState(preset, "predelay"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "modRate"));
    this->param_02_setPresetValue(getSubState(preset, "modAmount"));
    this->param_03_setPresetValue(getSubState(preset, "sizeSpeed"));
    this->param_04_setPresetValue(getSubState(preset, "decay"));
    this->param_05_setPresetValue(getSubState(preset, "damping"));
    this->param_06_setPresetValue(getSubState(preset, "deDiff1"));
    this->param_07_setPresetValue(getSubState(preset, "inDiff2"));
    this->param_08_setPresetValue(getSubState(preset, "inDiff1"));
    this->param_09_setPresetValue(getSubState(preset, "roomSize"));
    this->param_10_setPresetValue(getSubState(preset, "bandwidth"));
    this->param_11_setPresetValue(getSubState(preset, "predelay"));
}

void processTempoEvent(MillisecondTime time, Tempo tempo) {
    this->updateTime(time);

    if (this->globaltransport_setTempo(this->_currentTime, tempo, false))
        {}
}

void processTransportEvent(MillisecondTime time, TransportState state) {
    this->updateTime(time);

    if (this->globaltransport_setState(this->_currentTime, state, false))
        {}
}

void processBeatTimeEvent(MillisecondTime time, BeatTime beattime) {
    this->updateTime(time);

    if (this->globaltransport_setBeatTime(this->_currentTime, beattime, false))
        {}
}

void onSampleRateChanged(double ) {}

void processTimeSignatureEvent(MillisecondTime time, int numerator, int denominator) {
    this->updateTime(time);

    if (this->globaltransport_setTimeSignature(this->_currentTime, numerator, denominator, false))
        {}
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
        break;
        }
    case 4:
        {
        this->param_05_value_set(v);
        break;
        }
    case 5:
        {
        this->param_06_value_set(v);
        break;
        }
    case 6:
        {
        this->param_07_value_set(v);
        break;
        }
    case 7:
        {
        this->param_08_value_set(v);
        break;
        }
    case 8:
        {
        this->param_09_value_set(v);
        break;
        }
    case 9:
        {
        this->param_10_value_set(v);
        break;
        }
    case 10:
        {
        this->param_11_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
        }
    case 4:
        {
        return this->param_05_value;
        }
    case 5:
        {
        return this->param_06_value;
        }
    case 6:
        {
        return this->param_07_value;
        }
    case 7:
        {
        return this->param_08_value;
        }
    case 8:
        {
        return this->param_09_value;
        }
    case 9:
        {
        return this->param_10_value;
        }
    case 10:
        {
        return this->param_11_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 11;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "modRate";
        }
    case 1:
        {
        return "modAmount";
        }
    case 2:
        {
        return "sizeSpeed";
        }
    case 3:
        {
        return "decay";
        }
    case 4:
        {
        return "damping";
        }
    case 5:
        {
        return "deDiff1";
        }
    case 6:
        {
        return "inDiff2";
        }
    case 7:
        {
        return "inDiff1";
        }
    case 8:
        {
        return "roomSize";
        }
    case 9:
        {
        return "bandwidth";
        }
    case 10:
        {
        return "predelay";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "modRate";
        }
    case 1:
        {
        return "modAmount";
        }
    case 2:
        {
        return "sizeSpeed";
        }
    case 3:
        {
        return "decay";
        }
    case 4:
        {
        return "damping";
        }
    case 5:
        {
        return "deDiff1";
        }
    case 6:
        {
        return "inDiff2";
        }
    case 7:
        {
        return "inDiff1";
        }
    case 8:
        {
        return "roomSize";
        }
    case 9:
        {
        return "bandwidth";
        }
    case 10:
        {
        return "predelay";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.6;
            info->min = 0.01;
            info->max = 5;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.269;
            info->min = 0.01;
            info->max = 10;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1024;
            info->min = 0;
            info->max = 1024;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.5;
            info->min = 0;
            info->max = 0.9999;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.0005;
            info->min = 0;
            info->max = 0.9999;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 5:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.7;
            info->min = 0;
            info->max = 0.9999;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 6:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.625;
            info->min = 0;
            info->max = 0.9999;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 7:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.75;
            info->min = 0;
            info->max = 0.9999;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 8:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 1;
            info->max = 10;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 9:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.9995;
            info->min = 0;
            info->max = 0.9999;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 10:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 30;
            info->min = 0;
            info->max = 1000000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 1024 ? 1024 : value));
            ParameterValue normalizedValue = (value - 0) / (1024 - 0);
            return normalizedValue;
        }
        }
    case 10:
        {
        {
            value = (value < 0 ? 0 : (value > 1000000 ? 1000000 : value));
            ParameterValue normalizedValue = (value - 0) / (1000000 - 0);
            return normalizedValue;
        }
        }
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
        {
        {
            value = (value < 0 ? 0 : (value > 0.9999 ? 0.9999 : value));
            ParameterValue normalizedValue = (value - 0) / (0.9999 - 0);
            return normalizedValue;
        }
        }
    case 8:
        {
        {
            value = (value < 1 ? 1 : (value > 10 ? 10 : value));
            ParameterValue normalizedValue = (value - 1) / (10 - 1);
            return normalizedValue;
        }
        }
    case 0:
        {
        {
            value = (value < 0.01 ? 0.01 : (value > 5 ? 5 : value));
            ParameterValue normalizedValue = (value - 0.01) / (5 - 0.01);
            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < 0.01 ? 0.01 : (value > 10 ? 10 : value));
            ParameterValue normalizedValue = (value - 0.01) / (10 - 0.01);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (1024 - 0);
            }
        }
        }
    case 10:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (1000000 - 0);
            }
        }
        }
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (0.9999 - 0);
            }
        }
        }
    case 8:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 1 + value * (10 - 1);
            }
        }
        }
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0.01 + value * (5 - 0.01);
            }
        }
        }
    case 1:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0.01 + value * (10 - 0.01);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
        }
    case 4:
        {
        return this->param_05_value_constrain(value);
        }
    case 5:
        {
        return this->param_06_value_constrain(value);
        }
    case 6:
        {
        return this->param_07_value_constrain(value);
        }
    case 7:
        {
        return this->param_08_value_constrain(value);
        }
    case 8:
        {
        return this->param_09_value_constrain(value);
        }
    case 9:
        {
        return this->param_10_value_constrain(value);
        }
    case 10:
        {
        return this->param_11_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterChange(
            this->paramInitIndices[i],
            this->getParameterValue(this->paramInitIndices[i]),
            0
        );
    }
}

void processClockEvent(MillisecondTime , ClockId , bool , ParameterValue ) {}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {

    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->gen_01_modRate_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->gen_01_modAmount_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->gen_01_sizeSpeed_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        this->getEngine()->presetTouched();
        this->param_04_lastValue = this->param_04_value;
    }

    this->gen_01_decay_set(v);
}

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        this->getEngine()->presetTouched();
        this->param_05_lastValue = this->param_05_value;
    }

    this->gen_01_damping_set(v);
}

void param_06_value_set(number v) {
    v = this->param_06_value_constrain(v);
    this->param_06_value = v;
    this->sendParameter(5, false);

    if (this->param_06_value != this->param_06_lastValue) {
        this->getEngine()->presetTouched();
        this->param_06_lastValue = this->param_06_value;
    }

    this->gen_01_deDiff1_set(v);
}

void param_07_value_set(number v) {
    v = this->param_07_value_constrain(v);
    this->param_07_value = v;
    this->sendParameter(6, false);

    if (this->param_07_value != this->param_07_lastValue) {
        this->getEngine()->presetTouched();
        this->param_07_lastValue = this->param_07_value;
    }

    this->gen_01_inDiff2_set(v);
}

void param_08_value_set(number v) {
    v = this->param_08_value_constrain(v);
    this->param_08_value = v;
    this->sendParameter(7, false);

    if (this->param_08_value != this->param_08_lastValue) {
        this->getEngine()->presetTouched();
        this->param_08_lastValue = this->param_08_value;
    }

    this->gen_01_inDiff1_set(v);
}

void param_09_value_set(number v) {
    v = this->param_09_value_constrain(v);
    this->param_09_value = v;
    this->sendParameter(8, false);

    if (this->param_09_value != this->param_09_lastValue) {
        this->getEngine()->presetTouched();
        this->param_09_lastValue = this->param_09_value;
    }

    this->gen_01_roomSize_set(v);
}

void param_10_value_set(number v) {
    v = this->param_10_value_constrain(v);
    this->param_10_value = v;
    this->sendParameter(9, false);

    if (this->param_10_value != this->param_10_lastValue) {
        this->getEngine()->presetTouched();
        this->param_10_lastValue = this->param_10_value;
    }

    this->gen_01_bandwidth_set(v);
}

void param_11_value_set(number v) {
    v = this->param_11_value_constrain(v);
    this->param_11_value = v;
    this->sendParameter(10, false);

    if (this->param_11_value != this->param_11_lastValue) {
        this->getEngine()->presetTouched();
        this->param_11_lastValue = this->param_11_value;
    }

    this->gen_01_predelay_set(v);
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

Index getNumInputChannels() const {
    return 2;
}

Index getNumOutputChannels() const {
    return 2;
}

void allocateDataRefs() {
    this->gen_01_cycle_187_buffer->requestSize(16384, 1);
    this->gen_01_cycle_187_buffer->setSampleRate(this->sr);
    this->gen_01_cycle_214_buffer->requestSize(16384, 1);
    this->gen_01_cycle_214_buffer->setSampleRate(this->sr);
    this->gen_01_delay_13_buffer = this->gen_01_delay_13_buffer->allocateIfNeeded();

    if (this->gen_01_delay_13_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_13_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_13_bufferobj);

        this->getEngine()->sendDataRefUpdated(0);
    }

    this->gen_01_delay_12_buffer = this->gen_01_delay_12_buffer->allocateIfNeeded();

    if (this->gen_01_delay_12_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_12_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_12_bufferobj);

        this->getEngine()->sendDataRefUpdated(1);
    }

    this->gen_01_delay_11_buffer = this->gen_01_delay_11_buffer->allocateIfNeeded();

    if (this->gen_01_delay_11_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_11_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_11_bufferobj);

        this->getEngine()->sendDataRefUpdated(2);
    }

    this->gen_01_delay_10_buffer = this->gen_01_delay_10_buffer->allocateIfNeeded();

    if (this->gen_01_delay_10_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_10_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_10_bufferobj);

        this->getEngine()->sendDataRefUpdated(3);
    }

    this->gen_01_delay_9_buffer = this->gen_01_delay_9_buffer->allocateIfNeeded();

    if (this->gen_01_delay_9_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_9_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_9_bufferobj);

        this->getEngine()->sendDataRefUpdated(4);
    }

    this->gen_01_delay_8_buffer = this->gen_01_delay_8_buffer->allocateIfNeeded();

    if (this->gen_01_delay_8_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_8_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_8_bufferobj);

        this->getEngine()->sendDataRefUpdated(5);
    }

    this->gen_01_delay_7_buffer = this->gen_01_delay_7_buffer->allocateIfNeeded();

    if (this->gen_01_delay_7_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_7_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_7_bufferobj);

        this->getEngine()->sendDataRefUpdated(6);
    }

    this->gen_01_delay_6_buffer = this->gen_01_delay_6_buffer->allocateIfNeeded();

    if (this->gen_01_delay_6_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_6_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_6_bufferobj);

        this->getEngine()->sendDataRefUpdated(7);
    }

    this->gen_01_delay_5_buffer = this->gen_01_delay_5_buffer->allocateIfNeeded();

    if (this->gen_01_delay_5_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_5_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_5_bufferobj);

        this->getEngine()->sendDataRefUpdated(8);
    }

    this->gen_01_delay_4_buffer = this->gen_01_delay_4_buffer->allocateIfNeeded();

    if (this->gen_01_delay_4_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_4_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_4_bufferobj);

        this->getEngine()->sendDataRefUpdated(9);
    }

    this->gen_01_delay_3_buffer = this->gen_01_delay_3_buffer->allocateIfNeeded();

    if (this->gen_01_delay_3_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_3_bufferobj);

        this->getEngine()->sendDataRefUpdated(10);
    }

    this->gen_01_delay_2_buffer = this->gen_01_delay_2_buffer->allocateIfNeeded();

    if (this->gen_01_delay_2_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_2_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_2_bufferobj);

        this->getEngine()->sendDataRefUpdated(11);
    }

    this->gen_01_delay_1_buffer = this->gen_01_delay_1_buffer->allocateIfNeeded();

    if (this->gen_01_delay_1_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delay_1_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delay_1_bufferobj);

        this->getEngine()->sendDataRefUpdated(12);
    }

    this->gen_01_cycle_187_buffer = this->gen_01_cycle_187_buffer->allocateIfNeeded();
    this->gen_01_cycle_214_buffer = this->gen_01_cycle_214_buffer->allocateIfNeeded();

    if (this->RNBODefaultSinus->hasRequestedSize()) {
        if (this->RNBODefaultSinus->wantsFill())
            this->fillRNBODefaultSinus(this->RNBODefaultSinus);

        this->getEngine()->sendDataRefUpdated(13);
    }
}

void initializeObjects() {
    this->gen_01_history_16_init();
    this->gen_01_history_15_init();
    this->gen_01_history_14_init();
    this->gen_01_delay_13_init();
    this->gen_01_delay_12_init();
    this->gen_01_delay_11_init();
    this->gen_01_delay_10_init();
    this->gen_01_delay_9_init();
    this->gen_01_delay_8_init();
    this->gen_01_delay_7_init();
    this->gen_01_delay_6_init();
    this->gen_01_delay_5_init();
    this->gen_01_delay_4_init();
    this->gen_01_delay_3_init();
    this->gen_01_delay_2_init();
    this->gen_01_delay_1_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    {
        this->scheduleParamInit(4, 0);
    }

    {
        this->scheduleParamInit(5, 0);
    }

    {
        this->scheduleParamInit(6, 0);
    }

    {
        this->scheduleParamInit(7, 0);
    }

    {
        this->scheduleParamInit(8, 0);
    }

    {
        this->scheduleParamInit(9, 0);
    }

    {
        this->scheduleParamInit(10, 0);
    }

    this->processParamInitEvents();
}

static number param_01_value_constrain(number v) {
    v = (v > 5 ? 5 : (v < 0.01 ? 0.01 : v));
    return v;
}

static number gen_01_modRate_constrain(number v) {
    if (v < 0.01)
        v = 0.01;

    if (v > 5)
        v = 5;

    return v;
}

void gen_01_modRate_set(number v) {
    v = this->gen_01_modRate_constrain(v);
    this->gen_01_modRate = v;
}

static number param_02_value_constrain(number v) {
    v = (v > 10 ? 10 : (v < 0.01 ? 0.01 : v));
    return v;
}

static number gen_01_modAmount_constrain(number v) {
    if (v < 0.01)
        v = 0.01;

    if (v > 10)
        v = 10;

    return v;
}

void gen_01_modAmount_set(number v) {
    v = this->gen_01_modAmount_constrain(v);
    this->gen_01_modAmount = v;
}

static number param_03_value_constrain(number v) {
    v = (v > 1024 ? 1024 : (v < 0 ? 0 : v));
    return v;
}

static number gen_01_sizeSpeed_constrain(number v) {
    if (v < 0)
        v = 0;

    if (v > 1024)
        v = 1024;

    return v;
}

void gen_01_sizeSpeed_set(number v) {
    v = this->gen_01_sizeSpeed_constrain(v);
    this->gen_01_sizeSpeed = v;
}

static number param_04_value_constrain(number v) {
    v = (v > 0.9999 ? 0.9999 : (v < 0 ? 0 : v));
    return v;
}

static number gen_01_decay_constrain(number v) {
    if (v < 0)
        v = 0;

    if (v > 0.9999)
        v = 0.9999;

    return v;
}

void gen_01_decay_set(number v) {
    v = this->gen_01_decay_constrain(v);
    this->gen_01_decay = v;
}

static number param_05_value_constrain(number v) {
    v = (v > 0.9999 ? 0.9999 : (v < 0 ? 0 : v));
    return v;
}

static number gen_01_damping_constrain(number v) {
    if (v < 0)
        v = 0;

    if (v > 0.9999)
        v = 0.9999;

    return v;
}

void gen_01_damping_set(number v) {
    v = this->gen_01_damping_constrain(v);
    this->gen_01_damping = v;
}

static number param_06_value_constrain(number v) {
    v = (v > 0.9999 ? 0.9999 : (v < 0 ? 0 : v));
    return v;
}

static number gen_01_deDiff1_constrain(number v) {
    if (v < 0)
        v = 0;

    if (v > 0.9999)
        v = 0.9999;

    return v;
}

void gen_01_deDiff1_set(number v) {
    v = this->gen_01_deDiff1_constrain(v);
    this->gen_01_deDiff1 = v;
}

static number param_07_value_constrain(number v) {
    v = (v > 0.9999 ? 0.9999 : (v < 0 ? 0 : v));
    return v;
}

static number gen_01_inDiff2_constrain(number v) {
    if (v < 0)
        v = 0;

    if (v > 0.9999)
        v = 0.9999;

    return v;
}

void gen_01_inDiff2_set(number v) {
    v = this->gen_01_inDiff2_constrain(v);
    this->gen_01_inDiff2 = v;
}

static number param_08_value_constrain(number v) {
    v = (v > 0.9999 ? 0.9999 : (v < 0 ? 0 : v));
    return v;
}

static number gen_01_inDiff1_constrain(number v) {
    if (v < 0)
        v = 0;

    if (v > 0.9999)
        v = 0.9999;

    return v;
}

void gen_01_inDiff1_set(number v) {
    v = this->gen_01_inDiff1_constrain(v);
    this->gen_01_inDiff1 = v;
}

static number param_09_value_constrain(number v) {
    v = (v > 10 ? 10 : (v < 1 ? 1 : v));
    return v;
}

static number gen_01_roomSize_constrain(number v) {
    if (v < 1)
        v = 1;

    if (v > 10)
        v = 10;

    return v;
}

void gen_01_roomSize_set(number v) {
    v = this->gen_01_roomSize_constrain(v);
    this->gen_01_roomSize = v;
}

static number param_10_value_constrain(number v) {
    v = (v > 0.9999 ? 0.9999 : (v < 0 ? 0 : v));
    return v;
}

static number gen_01_bandwidth_constrain(number v) {
    if (v < 0)
        v = 0;

    if (v > 0.9999)
        v = 0.9999;

    return v;
}

void gen_01_bandwidth_set(number v) {
    v = this->gen_01_bandwidth_constrain(v);
    this->gen_01_bandwidth = v;
}

static number param_11_value_constrain(number v) {
    v = (v > 1000000 ? 1000000 : (v < 0 ? 0 : v));
    return v;
}

static number gen_01_predelay_constrain(number v) {
    if (v < 0)
        v = 0;

    return v;
}

void gen_01_predelay_set(number v) {
    v = this->gen_01_predelay_constrain(v);
    this->gen_01_predelay = v;
}

void dspexpr_02_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 0.4;//#map:_###_obj_###_:1
    }
}

void dspexpr_04_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 0.4;//#map:_###_obj_###_:1
    }
}

void gen_01_perform(
    const Sample * in1,
    const Sample * in2,
    number modRate,
    number modAmount,
    number roomSize,
    number deDiff1,
    number damping,
    number bandwidth,
    number decay,
    number inDiff2,
    number predelay,
    number sizeSpeed,
    number inDiff1,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    auto __gen_01_history_14_value = this->gen_01_history_14_value;
    auto __gen_01_history_16_value = this->gen_01_history_16_value;
    auto __gen_01_history_15_value = this->gen_01_history_15_value;
    number rsub_19_2 = 1 - bandwidth;
    auto mstosamps_21_4 = this->mstosamps(predelay);
    number dampingSend_25_8 = damping;
    number rsub_26_9 = 1 - dampingSend_25_8;
    number dampingSend_27_10 = dampingSend_25_8;
    number rsub_28_11 = 1 - dampingSend_27_10;
    number decaySend_163_170 = decay;
    number add_164_171 = decaySend_163_170 + 0.15;
    number clamp_165_172 = (add_164_171 > 0.5 ? 0.5 : (add_164_171 < 0.25 ? 0.25 : add_164_171));
    number rsub_166_173 = 0 - clamp_165_172;
    number decaySend_171_178 = decaySend_163_170;
    number add_172_179 = decaySend_171_178 + 0.15;
    number clamp_173_180 = (add_172_179 > 0.5 ? 0.5 : (add_172_179 < 0.25 ? 0.25 : add_172_179));
    number rsub_174_181 = 0 - clamp_173_180;
    number modRate_179_186 = modRate;
    number deDiff1Send_182_189 = deDiff1;
    number rsub_183_190 = 0 - deDiff1Send_182_189;
    number deDiff1Send_184_191 = deDiff1Send_182_189;
    number rsub_185_192 = 0 - deDiff1Send_184_191;
    number modAmount_186_193 = modAmount;
    auto mstosamps_187_194 = this->mstosamps(modAmount_186_193);
    number modAmount_188_195 = modAmount_186_193;
    auto mstosamps_189_196 = this->mstosamps(modAmount_188_195);
    number modRate_202_212 = modRate_179_186;
    number add_203_213 = modRate_202_212 + 0.1;
    number inDiff2Send_210_220 = inDiff2;
    number rsub_211_221 = 0 - inDiff2Send_210_220;
    number inDiff1Send_224_236 = inDiff1;
    number rsub_225_237 = 0 - inDiff1Send_224_236;
    Index i;

    for (i = 0; i < n; i++) {
        number mul_17_0 = __gen_01_history_15_value * damping;
        number mul_18_1 = __gen_01_history_16_value * damping;
        number mul_20_3 = __gen_01_history_14_value * rsub_19_2;
        number tap_22_5 = this->gen_01_delay_1_read(mstosamps_21_4, 0);
        number mul_23_6 = tap_22_5 * bandwidth;
        number add_24_7 = mul_23_6 + mul_20_3;
        number add_29_12 = in1[(Index)i] + in2[(Index)i];
        number mul_30_13 = add_29_12 * 0.5;
        number float_31_14 = float(125);
        number mul_32_15 = float_31_14 * roomSize;
        number slide_33_17 = this->gen_01_slide_16_next(mul_32_15, sizeSpeed, sizeSpeed);
        auto mstosamps_34_18 = this->mstosamps(slide_33_17);
        number float_35_19 = float(35.82);
        number mul_36_20 = float_35_19 * roomSize;
        number slide_37_22 = this->gen_01_slide_21_next(mul_36_20, sizeSpeed, sizeSpeed);
        auto mstosamps_38_23 = this->mstosamps(slide_37_22);
        number float_39_24 = float(89.82);
        number mul_40_25 = float_39_24 * roomSize;
        number slide_41_27 = this->gen_01_slide_26_next(mul_40_25, sizeSpeed, sizeSpeed);
        auto mstosamps_42_28 = this->mstosamps(slide_41_27);
        number tap_43_29 = this->gen_01_delay_2_read(mstosamps_34_18, 0);
        number tap_44_30 = this->gen_01_delay_2_read(mstosamps_38_23, 0);
        number tap_45_31 = this->gen_01_delay_2_read(mstosamps_42_28, 0);
        number mul_46_32 = tap_44_30 * 0.6;
        number mul_47_33 = tap_45_31 * 0.6;
        number float_48_34 = float(141.7);
        number mul_49_35 = float_48_34 * roomSize;
        number slide_50_37 = this->gen_01_slide_36_next(mul_49_35, sizeSpeed, sizeSpeed);
        auto mstosamps_51_38 = this->mstosamps(slide_50_37);
        number float_52_39 = float(8.94);
        number mul_53_40 = float_52_39 * roomSize;
        number slide_54_43 = this->gen_01_slide_42_next(mul_53_40, sizeSpeed, sizeSpeed);
        auto mstosamps_55_44 = this->mstosamps(slide_54_43);
        number float_56_45 = float(99.93);
        number mul_57_46 = float_56_45 * roomSize;
        number slide_58_48 = this->gen_01_slide_47_next(mul_57_46, sizeSpeed, sizeSpeed);
        auto mstosamps_59_49 = this->mstosamps(slide_58_48);
        number float_60_50 = float(70.93);
        number mul_61_51 = float_60_50 * roomSize;
        number slide_62_53 = this->gen_01_slide_52_next(mul_61_51, sizeSpeed, sizeSpeed);
        auto mstosamps_63_54 = this->mstosamps(slide_62_53);
        number tap_64_55 = this->gen_01_delay_3_read(mstosamps_51_38, 0);
        number tap_65_56 = this->gen_01_delay_3_read(mstosamps_55_44, 0);
        number tap_66_57 = this->gen_01_delay_3_read(mstosamps_59_49, 0);
        number tap_67_58 = this->gen_01_delay_3_read(mstosamps_63_54, 0);
        number mul_68_59 = tap_64_55 * rsub_26_9;
        number add_69_60 = mul_68_59 + mul_17_0;
        number mul_70_61 = tap_67_58 * -0.6;
        number mul_71_62 = tap_65_56 * 0.6;
        number mul_72_63 = tap_66_57 * 0.6;
        number add_73_64 = mul_71_62 + mul_72_63;
        number float_74_65 = float(149.63);
        number mul_75_66 = float_74_65 * roomSize;
        number slide_76_68 = this->gen_01_slide_67_next(mul_75_66, sizeSpeed, sizeSpeed);
        auto mstosamps_77_69 = this->mstosamps(slide_76_68);
        number float_78_70 = float(66.87);
        number mul_79_71 = float_78_70 * roomSize;
        number slide_80_73 = this->gen_01_slide_72_next(mul_79_71, sizeSpeed, sizeSpeed);
        auto mstosamps_81_74 = this->mstosamps(slide_80_73);
        number float_82_75 = float(11.86);
        number mul_83_76 = float_82_75 * roomSize;
        number slide_84_78 = this->gen_01_slide_77_next(mul_83_76, sizeSpeed, sizeSpeed);
        auto mstosamps_85_79 = this->mstosamps(slide_84_78);
        number float_86_80 = float(121.87);
        number mul_87_81 = float_86_80 * roomSize;
        number slide_88_83 = this->gen_01_slide_82_next(mul_87_81, sizeSpeed, sizeSpeed);
        auto mstosamps_89_84 = this->mstosamps(slide_88_83);
        number tap_90_85 = this->gen_01_delay_4_read(mstosamps_77_69, 0);
        number tap_91_86 = this->gen_01_delay_4_read(mstosamps_81_74, 0);
        number tap_92_87 = this->gen_01_delay_4_read(mstosamps_85_79, 0);
        number tap_93_88 = this->gen_01_delay_4_read(mstosamps_89_84, 0);
        number mul_94_89 = tap_90_85 * rsub_28_11;
        number add_95_90 = mul_94_89 + mul_18_1;
        number mul_96_91 = tap_91_86 * -0.6;
        number mul_97_92 = tap_92_87 * 0.6;
        number mul_98_93 = tap_93_88 * 0.6;
        number add_99_94 = mul_97_92 + mul_98_93;
        number float_100_95 = float(106.28);
        number mul_101_96 = float_100_95 * roomSize;
        number slide_102_98 = this->gen_01_slide_97_next(mul_101_96, sizeSpeed, sizeSpeed);
        auto mstosamps_103_99 = this->mstosamps(slide_102_98);
        number float_104_100 = float(67.07);
        number mul_105_101 = float_104_100 * roomSize;
        number slide_106_104 = this->gen_01_slide_103_next(mul_105_101, sizeSpeed, sizeSpeed);
        auto mstosamps_107_105 = this->mstosamps(slide_106_104);
        number float_108_106 = float(4.07);
        number mul_109_107 = float_108_106 * roomSize;
        number slide_110_109 = this->gen_01_slide_108_next(mul_109_107, sizeSpeed, sizeSpeed);
        auto mstosamps_111_110 = this->mstosamps(slide_110_109);
        number tap_112_111 = this->gen_01_delay_5_read(mstosamps_103_99, 0);
        number tap_113_112 = this->gen_01_delay_5_read(mstosamps_107_105, 0);
        number tap_114_113 = this->gen_01_delay_5_read(mstosamps_111_110, 0);
        number mul_115_114 = tap_114_113 * 0.6;
        number mul_116_115 = tap_113_112 * 0.6;
        number float_117_116 = float(60.48);
        number mul_118_117 = float_117_116 * roomSize;
        number slide_119_119 = this->gen_01_slide_118_next(mul_118_117, sizeSpeed, sizeSpeed);
        auto mstosamps_120_120 = this->mstosamps(slide_119_119);
        number float_121_121 = float(6.28);
        number mul_122_122 = float_121_121 * roomSize;
        number slide_123_125 = this->gen_01_slide_124_next(mul_122_122, sizeSpeed, sizeSpeed);
        auto mstosamps_124_126 = this->mstosamps(slide_123_125);
        number float_125_127 = float(41.26);
        number mul_126_128 = float_125_127 * roomSize;
        number slide_127_130 = this->gen_01_slide_129_next(mul_126_128, sizeSpeed, sizeSpeed);
        auto mstosamps_128_131 = this->mstosamps(slide_127_130);
        number tap_129_132 = this->gen_01_delay_6_read(mstosamps_120_120, 0);
        number tap_130_133 = this->gen_01_delay_6_read(mstosamps_124_126, 0);
        number tap_131_134 = this->gen_01_delay_6_read(mstosamps_128_131, 0);
        number mul_132_135 = tap_130_133 * -0.6;
        number mul_133_136 = tap_131_134 * -0.6;
        number add_134_137 = add_99_94 + mul_133_136;
        number add_135_138 = add_134_137 + mul_47_33;
        number add_136_139 = add_135_138 + mul_70_61;
        number mul_137_140 = tap_43_29 * decay;
        number mul_138_141 = tap_112_111 * decay;
        number float_139_142 = float(89.24);
        number mul_140_143 = float_139_142 * roomSize;
        number slide_141_145 = this->gen_01_slide_144_next(mul_140_143, sizeSpeed, sizeSpeed);
        auto mstosamps_142_146 = this->mstosamps(slide_141_145);
        number float_143_147 = float(64.28);
        number mul_144_148 = float_143_147 * roomSize;
        number slide_145_151 = this->gen_01_slide_150_next(mul_144_148, sizeSpeed, sizeSpeed);
        auto mstosamps_146_152 = this->mstosamps(slide_145_151);
        number float_147_153 = float(11.26);
        number mul_148_154 = float_147_153 * roomSize;
        number slide_149_156 = this->gen_01_slide_155_next(mul_148_154, sizeSpeed, sizeSpeed);
        auto mstosamps_150_157 = this->mstosamps(slide_149_156);
        number tap_151_158 = this->gen_01_delay_7_read(mstosamps_142_146, 0);
        number tap_152_159 = this->gen_01_delay_7_read(mstosamps_146_152, 0);
        number tap_153_160 = this->gen_01_delay_7_read(mstosamps_150_157, 0);
        number mul_154_161 = tap_153_160 * -0.6;
        number add_155_162 = add_136_139 + mul_154_161;
        number sub_156_163 = add_155_162 - mul_115_114;
        out2[(Index)i] = sub_156_163;
        number mul_157_164 = tap_152_159 * -0.6;
        number add_158_165 = add_73_64 + mul_157_164;
        number add_159_166 = add_158_165 + mul_116_115;
        number add_160_167 = add_159_166 + mul_96_91;
        number add_161_168 = add_160_167 + mul_132_135;
        number sub_162_169 = add_161_168 - mul_46_32;
        out1[(Index)i] = sub_162_169;
        number mul_167_174 = tap_151_158 * rsub_166_173;
        number add_168_175 = add_69_60 + mul_167_174;
        number mul_169_176 = add_168_175 * clamp_165_172;
        number add_170_177 = tap_151_158 + mul_169_176;
        number mul_175_182 = tap_129_132 * rsub_174_181;
        number add_176_183 = add_95_90 + mul_175_182;
        number mul_177_184 = add_176_183 * clamp_173_180;
        number add_178_185 = tap_129_132 + mul_177_184;
        number cycle_180 = 0;
        number cycleindex_181 = 0;
        array<number, 2> result_188 = this->gen_01_cycle_187_next(modRate_179_186, 0);
        cycleindex_181 = result_188[1];
        cycle_180 = result_188[0];
        number mul_190_197 = cycle_180 * mstosamps_189_196;
        number float_191_198 = float(30.51);
        number mul_192_199 = float_191_198 * roomSize;
        auto mstosamps_193_200 = this->mstosamps(mul_192_199);
        number slide_194_203 = this->gen_01_slide_202_next(mstosamps_193_200, sizeSpeed, sizeSpeed);
        number add_195_204 = slide_194_203 + mul_190_197;
        number tap_196_205 = this->gen_01_delay_8_read(add_195_204, 0);
        number mul_197_206 = tap_196_205 * rsub_185_192;
        number float_198_207 = float(22.58);
        number mul_199_208 = float_198_207 * roomSize;
        auto mstosamps_200_209 = this->mstosamps(mul_199_208);
        number slide_201_211 = this->gen_01_slide_210_next(mstosamps_200_209, sizeSpeed, sizeSpeed);
        number cycle_204 = 0;
        number cycleindex_205 = 0;
        array<number, 2> result_215 = this->gen_01_cycle_214_next(add_203_213, 0);
        cycleindex_205 = result_215[1];
        cycle_204 = result_215[0];
        number mul_206_216 = cycle_204 * mstosamps_187_194;
        number add_207_217 = slide_201_211 + mul_206_216;
        number tap_208_218 = this->gen_01_delay_9_read(add_207_217, 0);
        number mul_209_219 = tap_208_218 * rsub_183_190;
        number float_212_222 = float(9.31);
        number mul_213_223 = float_212_222 * roomSize;
        number slide_214_225 = this->gen_01_slide_224_next(mul_213_223, sizeSpeed, sizeSpeed);
        auto mstosamps_215_226 = this->mstosamps(slide_214_225);
        number tap_216_227 = this->gen_01_delay_10_read(mstosamps_215_226, 0);
        number mul_217_228 = tap_216_227 * rsub_211_221;
        number float_218_229 = float(12.73);
        number mul_219_230 = float_218_229 * roomSize;
        number slide_220_232 = this->gen_01_slide_231_next(mul_219_230, sizeSpeed, sizeSpeed);
        auto mstosamps_221_233 = this->mstosamps(slide_220_232);
        number tap_222_234 = this->gen_01_delay_11_read(mstosamps_221_233, 0);
        number mul_223_235 = tap_222_234 * rsub_211_221;
        number float_226_238 = float(3.6);
        number mul_227_239 = float_226_238 * roomSize;
        number slide_228_241 = this->gen_01_slide_240_next(mul_227_239, sizeSpeed, sizeSpeed);
        auto mstosamps_229_242 = this->mstosamps(slide_228_241);
        number tap_230_243 = this->gen_01_delay_12_read(mstosamps_229_242, 0);
        number mul_231_244 = tap_230_243 * rsub_225_237;
        number float_232_245 = float(4.77);
        number mul_233_246 = float_232_245 * roomSize;
        number slide_234_248 = this->gen_01_slide_247_next(mul_233_246, sizeSpeed, sizeSpeed);
        auto mstosamps_235_249 = this->mstosamps(slide_234_248);
        number tap_236_250 = this->gen_01_delay_13_read(mstosamps_235_249, 0);
        number mul_237_251 = tap_236_250 * rsub_225_237;
        number add_238_252 = add_24_7 + mul_237_251;
        number mul_239_253 = add_238_252 * inDiff1;
        number add_240_254 = tap_236_250 + mul_239_253;
        number add_241_255 = add_240_254 + mul_231_244;
        number mul_242_256 = add_241_255 * inDiff1;
        number add_243_257 = tap_230_243 + mul_242_256;
        number add_244_258 = add_243_257 + mul_223_235;
        number mul_245_259 = add_244_258 * inDiff2;
        number add_246_260 = tap_222_234 + mul_245_259;
        number add_247_261 = add_246_260 + mul_217_228;
        number mul_248_262 = add_247_261 * inDiff2;
        number add_249_263 = tap_216_227 + mul_248_262;
        number add_250_264 = add_249_263 + mul_137_140;
        number add_251_265 = add_250_264 + mul_197_206;
        number mul_252_266 = add_251_265 * deDiff1;
        number add_253_267 = tap_196_205 + mul_252_266;
        number add_254_268 = add_249_263 + mul_138_141;
        number add_255_269 = add_254_268 + mul_209_219;
        number mul_256_270 = add_255_269 * deDiff1;
        number add_257_271 = tap_208_218 + mul_256_270;
        number history_14_next_258_272 = fixdenorm(add_24_7);
        number history_15_next_259_273 = fixdenorm(add_69_60);
        number history_16_next_260_274 = fixdenorm(add_95_90);
        this->gen_01_delay_1_write(mul_30_13);
        __gen_01_history_16_value = history_16_next_260_274;
        __gen_01_history_15_value = history_15_next_259_273;
        __gen_01_history_14_value = history_14_next_258_272;
        this->gen_01_delay_13_write(add_238_252);
        this->gen_01_delay_12_write(add_241_255);
        this->gen_01_delay_11_write(add_244_258);
        this->gen_01_delay_10_write(add_247_261);
        this->gen_01_delay_9_write(add_255_269);
        this->gen_01_delay_8_write(add_251_265);
        this->gen_01_delay_7_write(add_168_175);
        this->gen_01_delay_6_write(add_176_183);
        this->gen_01_delay_5_write(add_170_177);
        this->gen_01_delay_4_write(add_257_271);
        this->gen_01_delay_3_write(add_253_267);
        this->gen_01_delay_2_write(add_178_185);
        this->gen_01_delay_13_step();
        this->gen_01_delay_12_step();
        this->gen_01_delay_11_step();
        this->gen_01_delay_10_step();
        this->gen_01_delay_9_step();
        this->gen_01_delay_8_step();
        this->gen_01_delay_7_step();
        this->gen_01_delay_6_step();
        this->gen_01_delay_5_step();
        this->gen_01_delay_4_step();
        this->gen_01_delay_3_step();
        this->gen_01_delay_2_step();
        this->gen_01_delay_1_step();
    }

    this->gen_01_history_15_value = __gen_01_history_15_value;
    this->gen_01_history_16_value = __gen_01_history_16_value;
    this->gen_01_history_14_value = __gen_01_history_14_value;
}

void dspexpr_01_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 0.6;//#map:_###_obj_###_:1
    }
}

void dspexpr_03_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 0.6;//#map:_###_obj_###_:1
    }
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

number gen_01_history_16_getvalue() {
    return this->gen_01_history_16_value;
}

void gen_01_history_16_setvalue(number val) {
    this->gen_01_history_16_value = val;
}

void gen_01_history_16_reset() {
    this->gen_01_history_16_value = 0;
}

void gen_01_history_16_init() {
    this->gen_01_history_16_value = 0;
}

number gen_01_history_15_getvalue() {
    return this->gen_01_history_15_value;
}

void gen_01_history_15_setvalue(number val) {
    this->gen_01_history_15_value = val;
}

void gen_01_history_15_reset() {
    this->gen_01_history_15_value = 0;
}

void gen_01_history_15_init() {
    this->gen_01_history_15_value = 0;
}

number gen_01_history_14_getvalue() {
    return this->gen_01_history_14_value;
}

void gen_01_history_14_setvalue(number val) {
    this->gen_01_history_14_value = val;
}

void gen_01_history_14_reset() {
    this->gen_01_history_14_value = 0;
}

void gen_01_history_14_init() {
    this->gen_01_history_14_value = 0;
}

void gen_01_delay_13_step() {
    this->gen_01_delay_13_reader++;

    if (this->gen_01_delay_13_reader >= (int)(this->gen_01_delay_13_buffer->getSize()))
        this->gen_01_delay_13_reader = 0;
}

number gen_01_delay_13_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_13_buffer->getSize()) + this->gen_01_delay_13_reader - ((size > this->gen_01_delay_13__maxdelay ? this->gen_01_delay_13__maxdelay : (size < (this->gen_01_delay_13_reader != this->gen_01_delay_13_writer) ? this->gen_01_delay_13_reader != this->gen_01_delay_13_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_13_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_13_wrap))
        ), this->gen_01_delay_13_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_13_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_13_buffer->getSize()) + this->gen_01_delay_13_reader - ((size > this->gen_01_delay_13__maxdelay ? this->gen_01_delay_13__maxdelay : (size < (this->gen_01_delay_13_reader != this->gen_01_delay_13_writer) ? this->gen_01_delay_13_reader != this->gen_01_delay_13_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_13_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_13_wrap))
    );
}

void gen_01_delay_13_write(number v) {
    this->gen_01_delay_13_writer = this->gen_01_delay_13_reader;
    this->gen_01_delay_13_buffer[(Index)this->gen_01_delay_13_writer] = v;
}

number gen_01_delay_13_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_13__maxdelay : size);
    number val = this->gen_01_delay_13_read(effectiveSize, 0);
    this->gen_01_delay_13_write(v);
    this->gen_01_delay_13_step();
    return val;
}

array<Index, 2> gen_01_delay_13_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_13_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_13_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_13_init() {
    auto result = this->gen_01_delay_13_calcSizeInSamples();
    this->gen_01_delay_13__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_13_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_13_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_13_clear() {
    this->gen_01_delay_13_buffer->setZero();
}

void gen_01_delay_13_reset() {
    auto result = this->gen_01_delay_13_calcSizeInSamples();
    this->gen_01_delay_13__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_13_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_13_buffer);
    this->gen_01_delay_13_wrap = this->gen_01_delay_13_buffer->getSize() - 1;
    this->gen_01_delay_13_clear();

    if (this->gen_01_delay_13_reader >= this->gen_01_delay_13__maxdelay || this->gen_01_delay_13_writer >= this->gen_01_delay_13__maxdelay) {
        this->gen_01_delay_13_reader = 0;
        this->gen_01_delay_13_writer = 0;
    }
}

void gen_01_delay_13_dspsetup() {
    this->gen_01_delay_13_reset();
}

number gen_01_delay_13_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 4580;
}

number gen_01_delay_13_size() {
    return this->gen_01_delay_13__maxdelay;
}

void gen_01_delay_12_step() {
    this->gen_01_delay_12_reader++;

    if (this->gen_01_delay_12_reader >= (int)(this->gen_01_delay_12_buffer->getSize()))
        this->gen_01_delay_12_reader = 0;
}

number gen_01_delay_12_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_12_buffer->getSize()) + this->gen_01_delay_12_reader - ((size > this->gen_01_delay_12__maxdelay ? this->gen_01_delay_12__maxdelay : (size < (this->gen_01_delay_12_reader != this->gen_01_delay_12_writer) ? this->gen_01_delay_12_reader != this->gen_01_delay_12_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_12_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_12_wrap))
        ), this->gen_01_delay_12_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_12_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_12_buffer->getSize()) + this->gen_01_delay_12_reader - ((size > this->gen_01_delay_12__maxdelay ? this->gen_01_delay_12__maxdelay : (size < (this->gen_01_delay_12_reader != this->gen_01_delay_12_writer) ? this->gen_01_delay_12_reader != this->gen_01_delay_12_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_12_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_12_wrap))
    );
}

void gen_01_delay_12_write(number v) {
    this->gen_01_delay_12_writer = this->gen_01_delay_12_reader;
    this->gen_01_delay_12_buffer[(Index)this->gen_01_delay_12_writer] = v;
}

number gen_01_delay_12_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_12__maxdelay : size);
    number val = this->gen_01_delay_12_read(effectiveSize, 0);
    this->gen_01_delay_12_write(v);
    this->gen_01_delay_12_step();
    return val;
}

array<Index, 2> gen_01_delay_12_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_12_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_12_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_12_init() {
    auto result = this->gen_01_delay_12_calcSizeInSamples();
    this->gen_01_delay_12__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_12_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_12_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_12_clear() {
    this->gen_01_delay_12_buffer->setZero();
}

void gen_01_delay_12_reset() {
    auto result = this->gen_01_delay_12_calcSizeInSamples();
    this->gen_01_delay_12__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_12_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_12_buffer);
    this->gen_01_delay_12_wrap = this->gen_01_delay_12_buffer->getSize() - 1;
    this->gen_01_delay_12_clear();

    if (this->gen_01_delay_12_reader >= this->gen_01_delay_12__maxdelay || this->gen_01_delay_12_writer >= this->gen_01_delay_12__maxdelay) {
        this->gen_01_delay_12_reader = 0;
        this->gen_01_delay_12_writer = 0;
    }
}

void gen_01_delay_12_dspsetup() {
    this->gen_01_delay_12_reset();
}

number gen_01_delay_12_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 3456;
}

number gen_01_delay_12_size() {
    return this->gen_01_delay_12__maxdelay;
}

void gen_01_delay_11_step() {
    this->gen_01_delay_11_reader++;

    if (this->gen_01_delay_11_reader >= (int)(this->gen_01_delay_11_buffer->getSize()))
        this->gen_01_delay_11_reader = 0;
}

number gen_01_delay_11_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_11_buffer->getSize()) + this->gen_01_delay_11_reader - ((size > this->gen_01_delay_11__maxdelay ? this->gen_01_delay_11__maxdelay : (size < (this->gen_01_delay_11_reader != this->gen_01_delay_11_writer) ? this->gen_01_delay_11_reader != this->gen_01_delay_11_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_11_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_11_wrap))
        ), this->gen_01_delay_11_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_11_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_11_buffer->getSize()) + this->gen_01_delay_11_reader - ((size > this->gen_01_delay_11__maxdelay ? this->gen_01_delay_11__maxdelay : (size < (this->gen_01_delay_11_reader != this->gen_01_delay_11_writer) ? this->gen_01_delay_11_reader != this->gen_01_delay_11_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_11_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_11_wrap))
    );
}

void gen_01_delay_11_write(number v) {
    this->gen_01_delay_11_writer = this->gen_01_delay_11_reader;
    this->gen_01_delay_11_buffer[(Index)this->gen_01_delay_11_writer] = v;
}

number gen_01_delay_11_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_11__maxdelay : size);
    number val = this->gen_01_delay_11_read(effectiveSize, 0);
    this->gen_01_delay_11_write(v);
    this->gen_01_delay_11_step();
    return val;
}

array<Index, 2> gen_01_delay_11_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_11_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_11_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_11_init() {
    auto result = this->gen_01_delay_11_calcSizeInSamples();
    this->gen_01_delay_11__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_11_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_11_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_11_clear() {
    this->gen_01_delay_11_buffer->setZero();
}

void gen_01_delay_11_reset() {
    auto result = this->gen_01_delay_11_calcSizeInSamples();
    this->gen_01_delay_11__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_11_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_11_buffer);
    this->gen_01_delay_11_wrap = this->gen_01_delay_11_buffer->getSize() - 1;
    this->gen_01_delay_11_clear();

    if (this->gen_01_delay_11_reader >= this->gen_01_delay_11__maxdelay || this->gen_01_delay_11_writer >= this->gen_01_delay_11__maxdelay) {
        this->gen_01_delay_11_reader = 0;
        this->gen_01_delay_11_writer = 0;
    }
}

void gen_01_delay_11_dspsetup() {
    this->gen_01_delay_11_reset();
}

number gen_01_delay_11_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 12221;
}

number gen_01_delay_11_size() {
    return this->gen_01_delay_11__maxdelay;
}

void gen_01_delay_10_step() {
    this->gen_01_delay_10_reader++;

    if (this->gen_01_delay_10_reader >= (int)(this->gen_01_delay_10_buffer->getSize()))
        this->gen_01_delay_10_reader = 0;
}

number gen_01_delay_10_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_10_buffer->getSize()) + this->gen_01_delay_10_reader - ((size > this->gen_01_delay_10__maxdelay ? this->gen_01_delay_10__maxdelay : (size < (this->gen_01_delay_10_reader != this->gen_01_delay_10_writer) ? this->gen_01_delay_10_reader != this->gen_01_delay_10_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_10_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_10_wrap))
        ), this->gen_01_delay_10_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_10_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_10_buffer->getSize()) + this->gen_01_delay_10_reader - ((size > this->gen_01_delay_10__maxdelay ? this->gen_01_delay_10__maxdelay : (size < (this->gen_01_delay_10_reader != this->gen_01_delay_10_writer) ? this->gen_01_delay_10_reader != this->gen_01_delay_10_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_10_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_10_wrap))
    );
}

void gen_01_delay_10_write(number v) {
    this->gen_01_delay_10_writer = this->gen_01_delay_10_reader;
    this->gen_01_delay_10_buffer[(Index)this->gen_01_delay_10_writer] = v;
}

number gen_01_delay_10_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_10__maxdelay : size);
    number val = this->gen_01_delay_10_read(effectiveSize, 0);
    this->gen_01_delay_10_write(v);
    this->gen_01_delay_10_step();
    return val;
}

array<Index, 2> gen_01_delay_10_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_10_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_10_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_10_init() {
    auto result = this->gen_01_delay_10_calcSizeInSamples();
    this->gen_01_delay_10__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_10_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_10_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_10_clear() {
    this->gen_01_delay_10_buffer->setZero();
}

void gen_01_delay_10_reset() {
    auto result = this->gen_01_delay_10_calcSizeInSamples();
    this->gen_01_delay_10__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_10_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_10_buffer);
    this->gen_01_delay_10_wrap = this->gen_01_delay_10_buffer->getSize() - 1;
    this->gen_01_delay_10_clear();

    if (this->gen_01_delay_10_reader >= this->gen_01_delay_10__maxdelay || this->gen_01_delay_10_writer >= this->gen_01_delay_10__maxdelay) {
        this->gen_01_delay_10_reader = 0;
        this->gen_01_delay_10_writer = 0;
    }
}

void gen_01_delay_10_dspsetup() {
    this->gen_01_delay_10_reset();
}

number gen_01_delay_10_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 8938;
}

number gen_01_delay_10_size() {
    return this->gen_01_delay_10__maxdelay;
}

void gen_01_delay_9_step() {
    this->gen_01_delay_9_reader++;

    if (this->gen_01_delay_9_reader >= (int)(this->gen_01_delay_9_buffer->getSize()))
        this->gen_01_delay_9_reader = 0;
}

number gen_01_delay_9_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_9_buffer->getSize()) + this->gen_01_delay_9_reader - ((size > this->gen_01_delay_9__maxdelay ? this->gen_01_delay_9__maxdelay : (size < (this->gen_01_delay_9_reader != this->gen_01_delay_9_writer) ? this->gen_01_delay_9_reader != this->gen_01_delay_9_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_9_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_9_wrap))
        ), this->gen_01_delay_9_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_9_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_9_buffer->getSize()) + this->gen_01_delay_9_reader - ((size > this->gen_01_delay_9__maxdelay ? this->gen_01_delay_9__maxdelay : (size < (this->gen_01_delay_9_reader != this->gen_01_delay_9_writer) ? this->gen_01_delay_9_reader != this->gen_01_delay_9_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_9_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_9_wrap))
    );
}

void gen_01_delay_9_write(number v) {
    this->gen_01_delay_9_writer = this->gen_01_delay_9_reader;
    this->gen_01_delay_9_buffer[(Index)this->gen_01_delay_9_writer] = v;
}

number gen_01_delay_9_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_9__maxdelay : size);
    number val = this->gen_01_delay_9_read(effectiveSize, 0);
    this->gen_01_delay_9_write(v);
    this->gen_01_delay_9_step();
    return val;
}

array<Index, 2> gen_01_delay_9_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_9_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_9_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_9_init() {
    auto result = this->gen_01_delay_9_calcSizeInSamples();
    this->gen_01_delay_9__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_9_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_9_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_9_clear() {
    this->gen_01_delay_9_buffer->setZero();
}

void gen_01_delay_9_reset() {
    auto result = this->gen_01_delay_9_calcSizeInSamples();
    this->gen_01_delay_9__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_9_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_9_buffer);
    this->gen_01_delay_9_wrap = this->gen_01_delay_9_buffer->getSize() - 1;
    this->gen_01_delay_9_clear();

    if (this->gen_01_delay_9_reader >= this->gen_01_delay_9__maxdelay || this->gen_01_delay_9_writer >= this->gen_01_delay_9__maxdelay) {
        this->gen_01_delay_9_reader = 0;
        this->gen_01_delay_9_writer = 0;
    }
}

void gen_01_delay_9_dspsetup() {
    this->gen_01_delay_9_reset();
}

number gen_01_delay_9_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 21677;
}

number gen_01_delay_9_size() {
    return this->gen_01_delay_9__maxdelay;
}

void gen_01_delay_8_step() {
    this->gen_01_delay_8_reader++;

    if (this->gen_01_delay_8_reader >= (int)(this->gen_01_delay_8_buffer->getSize()))
        this->gen_01_delay_8_reader = 0;
}

number gen_01_delay_8_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_8_buffer->getSize()) + this->gen_01_delay_8_reader - ((size > this->gen_01_delay_8__maxdelay ? this->gen_01_delay_8__maxdelay : (size < (this->gen_01_delay_8_reader != this->gen_01_delay_8_writer) ? this->gen_01_delay_8_reader != this->gen_01_delay_8_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_8_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_8_wrap))
        ), this->gen_01_delay_8_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_8_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_8_buffer->getSize()) + this->gen_01_delay_8_reader - ((size > this->gen_01_delay_8__maxdelay ? this->gen_01_delay_8__maxdelay : (size < (this->gen_01_delay_8_reader != this->gen_01_delay_8_writer) ? this->gen_01_delay_8_reader != this->gen_01_delay_8_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_8_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_8_wrap))
    );
}

void gen_01_delay_8_write(number v) {
    this->gen_01_delay_8_writer = this->gen_01_delay_8_reader;
    this->gen_01_delay_8_buffer[(Index)this->gen_01_delay_8_writer] = v;
}

number gen_01_delay_8_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_8__maxdelay : size);
    number val = this->gen_01_delay_8_read(effectiveSize, 0);
    this->gen_01_delay_8_write(v);
    this->gen_01_delay_8_step();
    return val;
}

array<Index, 2> gen_01_delay_8_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_8_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_8_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_8_init() {
    auto result = this->gen_01_delay_8_calcSizeInSamples();
    this->gen_01_delay_8__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_8_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_8_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_8_clear() {
    this->gen_01_delay_8_buffer->setZero();
}

void gen_01_delay_8_reset() {
    auto result = this->gen_01_delay_8_calcSizeInSamples();
    this->gen_01_delay_8__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_8_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_8_buffer);
    this->gen_01_delay_8_wrap = this->gen_01_delay_8_buffer->getSize() - 1;
    this->gen_01_delay_8_clear();

    if (this->gen_01_delay_8_reader >= this->gen_01_delay_8__maxdelay || this->gen_01_delay_8_writer >= this->gen_01_delay_8__maxdelay) {
        this->gen_01_delay_8_reader = 0;
        this->gen_01_delay_8_writer = 0;
    }
}

void gen_01_delay_8_dspsetup() {
    this->gen_01_delay_8_reset();
}

number gen_01_delay_8_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 29290;
}

number gen_01_delay_8_size() {
    return this->gen_01_delay_8__maxdelay;
}

void gen_01_delay_7_step() {
    this->gen_01_delay_7_reader++;

    if (this->gen_01_delay_7_reader >= (int)(this->gen_01_delay_7_buffer->getSize()))
        this->gen_01_delay_7_reader = 0;
}

number gen_01_delay_7_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_7_buffer->getSize()) + this->gen_01_delay_7_reader - ((size > this->gen_01_delay_7__maxdelay ? this->gen_01_delay_7__maxdelay : (size < (this->gen_01_delay_7_reader != this->gen_01_delay_7_writer) ? this->gen_01_delay_7_reader != this->gen_01_delay_7_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_7_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_7_wrap))
        ), this->gen_01_delay_7_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_7_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_7_buffer->getSize()) + this->gen_01_delay_7_reader - ((size > this->gen_01_delay_7__maxdelay ? this->gen_01_delay_7__maxdelay : (size < (this->gen_01_delay_7_reader != this->gen_01_delay_7_writer) ? this->gen_01_delay_7_reader != this->gen_01_delay_7_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_7_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_7_wrap))
    );
}

void gen_01_delay_7_write(number v) {
    this->gen_01_delay_7_writer = this->gen_01_delay_7_reader;
    this->gen_01_delay_7_buffer[(Index)this->gen_01_delay_7_writer] = v;
}

number gen_01_delay_7_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_7__maxdelay : size);
    number val = this->gen_01_delay_7_read(effectiveSize, 0);
    this->gen_01_delay_7_write(v);
    this->gen_01_delay_7_step();
    return val;
}

array<Index, 2> gen_01_delay_7_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_7_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_7_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_7_init() {
    auto result = this->gen_01_delay_7_calcSizeInSamples();
    this->gen_01_delay_7__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_7_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_7_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_7_clear() {
    this->gen_01_delay_7_buffer->setZero();
}

void gen_01_delay_7_reset() {
    auto result = this->gen_01_delay_7_calcSizeInSamples();
    this->gen_01_delay_7__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_7_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_7_buffer);
    this->gen_01_delay_7_wrap = this->gen_01_delay_7_buffer->getSize() - 1;
    this->gen_01_delay_7_clear();

    if (this->gen_01_delay_7_reader >= this->gen_01_delay_7__maxdelay || this->gen_01_delay_7_writer >= this->gen_01_delay_7__maxdelay) {
        this->gen_01_delay_7_reader = 0;
        this->gen_01_delay_7_writer = 0;
    }
}

void gen_01_delay_7_dspsetup() {
    this->gen_01_delay_7_reset();
}

number gen_01_delay_7_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 85671;
}

number gen_01_delay_7_size() {
    return this->gen_01_delay_7__maxdelay;
}

void gen_01_delay_6_step() {
    this->gen_01_delay_6_reader++;

    if (this->gen_01_delay_6_reader >= (int)(this->gen_01_delay_6_buffer->getSize()))
        this->gen_01_delay_6_reader = 0;
}

number gen_01_delay_6_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_6_buffer->getSize()) + this->gen_01_delay_6_reader - ((size > this->gen_01_delay_6__maxdelay ? this->gen_01_delay_6__maxdelay : (size < (this->gen_01_delay_6_reader != this->gen_01_delay_6_writer) ? this->gen_01_delay_6_reader != this->gen_01_delay_6_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_6_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_6_wrap))
        ), this->gen_01_delay_6_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_6_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_6_buffer->getSize()) + this->gen_01_delay_6_reader - ((size > this->gen_01_delay_6__maxdelay ? this->gen_01_delay_6__maxdelay : (size < (this->gen_01_delay_6_reader != this->gen_01_delay_6_writer) ? this->gen_01_delay_6_reader != this->gen_01_delay_6_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_6_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_6_wrap))
    );
}

void gen_01_delay_6_write(number v) {
    this->gen_01_delay_6_writer = this->gen_01_delay_6_reader;
    this->gen_01_delay_6_buffer[(Index)this->gen_01_delay_6_writer] = v;
}

number gen_01_delay_6_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_6__maxdelay : size);
    number val = this->gen_01_delay_6_read(effectiveSize, 0);
    this->gen_01_delay_6_write(v);
    this->gen_01_delay_6_step();
    return val;
}

array<Index, 2> gen_01_delay_6_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_6_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_6_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_6_init() {
    auto result = this->gen_01_delay_6_calcSizeInSamples();
    this->gen_01_delay_6__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_6_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_6_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_6_clear() {
    this->gen_01_delay_6_buffer->setZero();
}

void gen_01_delay_6_reset() {
    auto result = this->gen_01_delay_6_calcSizeInSamples();
    this->gen_01_delay_6__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_6_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_6_buffer);
    this->gen_01_delay_6_wrap = this->gen_01_delay_6_buffer->getSize() - 1;
    this->gen_01_delay_6_clear();

    if (this->gen_01_delay_6_reader >= this->gen_01_delay_6__maxdelay || this->gen_01_delay_6_writer >= this->gen_01_delay_6__maxdelay) {
        this->gen_01_delay_6_reader = 0;
        this->gen_01_delay_6_writer = 0;
    }
}

void gen_01_delay_6_dspsetup() {
    this->gen_01_delay_6_reset();
}

number gen_01_delay_6_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 58061;
}

number gen_01_delay_6_size() {
    return this->gen_01_delay_6__maxdelay;
}

void gen_01_delay_5_step() {
    this->gen_01_delay_5_reader++;

    if (this->gen_01_delay_5_reader >= (int)(this->gen_01_delay_5_buffer->getSize()))
        this->gen_01_delay_5_reader = 0;
}

number gen_01_delay_5_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_5_buffer->getSize()) + this->gen_01_delay_5_reader - ((size > this->gen_01_delay_5__maxdelay ? this->gen_01_delay_5__maxdelay : (size < (this->gen_01_delay_5_reader != this->gen_01_delay_5_writer) ? this->gen_01_delay_5_reader != this->gen_01_delay_5_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_5_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_5_wrap))
        ), this->gen_01_delay_5_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_5_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_5_buffer->getSize()) + this->gen_01_delay_5_reader - ((size > this->gen_01_delay_5__maxdelay ? this->gen_01_delay_5__maxdelay : (size < (this->gen_01_delay_5_reader != this->gen_01_delay_5_writer) ? this->gen_01_delay_5_reader != this->gen_01_delay_5_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_5_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_5_wrap))
    );
}

void gen_01_delay_5_write(number v) {
    this->gen_01_delay_5_writer = this->gen_01_delay_5_reader;
    this->gen_01_delay_5_buffer[(Index)this->gen_01_delay_5_writer] = v;
}

number gen_01_delay_5_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_5__maxdelay : size);
    number val = this->gen_01_delay_5_read(effectiveSize, 0);
    this->gen_01_delay_5_write(v);
    this->gen_01_delay_5_step();
    return val;
}

array<Index, 2> gen_01_delay_5_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_5_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_5_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_5_init() {
    auto result = this->gen_01_delay_5_calcSizeInSamples();
    this->gen_01_delay_5__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_5_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_5_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_5_clear() {
    this->gen_01_delay_5_buffer->setZero();
}

void gen_01_delay_5_reset() {
    auto result = this->gen_01_delay_5_calcSizeInSamples();
    this->gen_01_delay_5__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_5_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_5_buffer);
    this->gen_01_delay_5_wrap = this->gen_01_delay_5_buffer->getSize() - 1;
    this->gen_01_delay_5_clear();

    if (this->gen_01_delay_5_reader >= this->gen_01_delay_5__maxdelay || this->gen_01_delay_5_writer >= this->gen_01_delay_5__maxdelay) {
        this->gen_01_delay_5_reader = 0;
        this->gen_01_delay_5_writer = 0;
    }
}

void gen_01_delay_5_dspsetup() {
    this->gen_01_delay_5_reset();
}

number gen_01_delay_5_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 102029;
}

number gen_01_delay_5_size() {
    return this->gen_01_delay_5__maxdelay;
}

void gen_01_delay_4_step() {
    this->gen_01_delay_4_reader++;

    if (this->gen_01_delay_4_reader >= (int)(this->gen_01_delay_4_buffer->getSize()))
        this->gen_01_delay_4_reader = 0;
}

number gen_01_delay_4_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_4_buffer->getSize()) + this->gen_01_delay_4_reader - ((size > this->gen_01_delay_4__maxdelay ? this->gen_01_delay_4__maxdelay : (size < (this->gen_01_delay_4_reader != this->gen_01_delay_4_writer) ? this->gen_01_delay_4_reader != this->gen_01_delay_4_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_4_wrap))
        ), this->gen_01_delay_4_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_4_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_4_buffer->getSize()) + this->gen_01_delay_4_reader - ((size > this->gen_01_delay_4__maxdelay ? this->gen_01_delay_4__maxdelay : (size < (this->gen_01_delay_4_reader != this->gen_01_delay_4_writer) ? this->gen_01_delay_4_reader != this->gen_01_delay_4_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_4_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_4_wrap))
    );
}

void gen_01_delay_4_write(number v) {
    this->gen_01_delay_4_writer = this->gen_01_delay_4_reader;
    this->gen_01_delay_4_buffer[(Index)this->gen_01_delay_4_writer] = v;
}

number gen_01_delay_4_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_4__maxdelay : size);
    number val = this->gen_01_delay_4_read(effectiveSize, 0);
    this->gen_01_delay_4_write(v);
    this->gen_01_delay_4_step();
    return val;
}

array<Index, 2> gen_01_delay_4_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_4_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_4_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_4_init() {
    auto result = this->gen_01_delay_4_calcSizeInSamples();
    this->gen_01_delay_4__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_4_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_4_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_4_clear() {
    this->gen_01_delay_4_buffer->setZero();
}

void gen_01_delay_4_reset() {
    auto result = this->gen_01_delay_4_calcSizeInSamples();
    this->gen_01_delay_4__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_4_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_4_buffer);
    this->gen_01_delay_4_wrap = this->gen_01_delay_4_buffer->getSize() - 1;
    this->gen_01_delay_4_clear();

    if (this->gen_01_delay_4_reader >= this->gen_01_delay_4__maxdelay || this->gen_01_delay_4_writer >= this->gen_01_delay_4__maxdelay) {
        this->gen_01_delay_4_reader = 0;
        this->gen_01_delay_4_writer = 0;
    }
}

void gen_01_delay_4_dspsetup() {
    this->gen_01_delay_4_reset();
}

number gen_01_delay_4_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 143645;
}

number gen_01_delay_4_size() {
    return this->gen_01_delay_4__maxdelay;
}

void gen_01_delay_3_step() {
    this->gen_01_delay_3_reader++;

    if (this->gen_01_delay_3_reader >= (int)(this->gen_01_delay_3_buffer->getSize()))
        this->gen_01_delay_3_reader = 0;
}

number gen_01_delay_3_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_3_buffer->getSize()) + this->gen_01_delay_3_reader - ((size > this->gen_01_delay_3__maxdelay ? this->gen_01_delay_3__maxdelay : (size < (this->gen_01_delay_3_reader != this->gen_01_delay_3_writer) ? this->gen_01_delay_3_reader != this->gen_01_delay_3_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_3_wrap))
        ), this->gen_01_delay_3_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_3_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_3_buffer->getSize()) + this->gen_01_delay_3_reader - ((size > this->gen_01_delay_3__maxdelay ? this->gen_01_delay_3__maxdelay : (size < (this->gen_01_delay_3_reader != this->gen_01_delay_3_writer) ? this->gen_01_delay_3_reader != this->gen_01_delay_3_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_3_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_3_wrap))
    );
}

void gen_01_delay_3_write(number v) {
    this->gen_01_delay_3_writer = this->gen_01_delay_3_reader;
    this->gen_01_delay_3_buffer[(Index)this->gen_01_delay_3_writer] = v;
}

number gen_01_delay_3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_3__maxdelay : size);
    number val = this->gen_01_delay_3_read(effectiveSize, 0);
    this->gen_01_delay_3_write(v);
    this->gen_01_delay_3_step();
    return val;
}

array<Index, 2> gen_01_delay_3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_3_init() {
    auto result = this->gen_01_delay_3_calcSizeInSamples();
    this->gen_01_delay_3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_3_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_3_clear() {
    this->gen_01_delay_3_buffer->setZero();
}

void gen_01_delay_3_reset() {
    auto result = this->gen_01_delay_3_calcSizeInSamples();
    this->gen_01_delay_3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_3_buffer);
    this->gen_01_delay_3_wrap = this->gen_01_delay_3_buffer->getSize() - 1;
    this->gen_01_delay_3_clear();

    if (this->gen_01_delay_3_reader >= this->gen_01_delay_3__maxdelay || this->gen_01_delay_3_writer >= this->gen_01_delay_3__maxdelay) {
        this->gen_01_delay_3_reader = 0;
        this->gen_01_delay_3_writer = 0;
    }
}

void gen_01_delay_3_dspsetup() {
    this->gen_01_delay_3_reset();
}

number gen_01_delay_3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 136032;
}

number gen_01_delay_3_size() {
    return this->gen_01_delay_3__maxdelay;
}

void gen_01_delay_2_step() {
    this->gen_01_delay_2_reader++;

    if (this->gen_01_delay_2_reader >= (int)(this->gen_01_delay_2_buffer->getSize()))
        this->gen_01_delay_2_reader = 0;
}

number gen_01_delay_2_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_2_buffer->getSize()) + this->gen_01_delay_2_reader - ((size > this->gen_01_delay_2__maxdelay ? this->gen_01_delay_2__maxdelay : (size < (this->gen_01_delay_2_reader != this->gen_01_delay_2_writer) ? this->gen_01_delay_2_reader != this->gen_01_delay_2_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_2_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_2_wrap))
        ), this->gen_01_delay_2_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_2_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_2_buffer->getSize()) + this->gen_01_delay_2_reader - ((size > this->gen_01_delay_2__maxdelay ? this->gen_01_delay_2__maxdelay : (size < (this->gen_01_delay_2_reader != this->gen_01_delay_2_writer) ? this->gen_01_delay_2_reader != this->gen_01_delay_2_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_2_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_2_wrap))
    );
}

void gen_01_delay_2_write(number v) {
    this->gen_01_delay_2_writer = this->gen_01_delay_2_reader;
    this->gen_01_delay_2_buffer[(Index)this->gen_01_delay_2_writer] = v;
}

number gen_01_delay_2_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_2__maxdelay : size);
    number val = this->gen_01_delay_2_read(effectiveSize, 0);
    this->gen_01_delay_2_write(v);
    this->gen_01_delay_2_step();
    return val;
}

array<Index, 2> gen_01_delay_2_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_2_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_2_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_2_init() {
    auto result = this->gen_01_delay_2_calcSizeInSamples();
    this->gen_01_delay_2__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_2_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_2_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_2_clear() {
    this->gen_01_delay_2_buffer->setZero();
}

void gen_01_delay_2_reset() {
    auto result = this->gen_01_delay_2_calcSizeInSamples();
    this->gen_01_delay_2__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_2_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_2_buffer);
    this->gen_01_delay_2_wrap = this->gen_01_delay_2_buffer->getSize() - 1;
    this->gen_01_delay_2_clear();

    if (this->gen_01_delay_2_reader >= this->gen_01_delay_2__maxdelay || this->gen_01_delay_2_writer >= this->gen_01_delay_2__maxdelay) {
        this->gen_01_delay_2_reader = 0;
        this->gen_01_delay_2_writer = 0;
    }
}

void gen_01_delay_2_dspsetup() {
    this->gen_01_delay_2_reset();
}

number gen_01_delay_2_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return 120000;
}

number gen_01_delay_2_size() {
    return this->gen_01_delay_2__maxdelay;
}

void gen_01_delay_1_step() {
    this->gen_01_delay_1_reader++;

    if (this->gen_01_delay_1_reader >= (int)(this->gen_01_delay_1_buffer->getSize()))
        this->gen_01_delay_1_reader = 0;
}

number gen_01_delay_1_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_delay_1_buffer->getSize()) + this->gen_01_delay_1_reader - ((size > this->gen_01_delay_1__maxdelay ? this->gen_01_delay_1__maxdelay : (size < (this->gen_01_delay_1_reader != this->gen_01_delay_1_writer) ? this->gen_01_delay_1_reader != this->gen_01_delay_1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_delay_1_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_1_wrap))
        ), this->gen_01_delay_1_buffer->getSample(
            0,
            (Index)((BinOpInt)((UBinOpInt)index2 & (UBinOpInt)this->gen_01_delay_1_wrap))
        ));
    }

    number r = (int)(this->gen_01_delay_1_buffer->getSize()) + this->gen_01_delay_1_reader - ((size > this->gen_01_delay_1__maxdelay ? this->gen_01_delay_1__maxdelay : (size < (this->gen_01_delay_1_reader != this->gen_01_delay_1_writer) ? this->gen_01_delay_1_reader != this->gen_01_delay_1_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delay_1_buffer->getSample(
        0,
        (Index)((BinOpInt)((UBinOpInt)index1 & (UBinOpInt)this->gen_01_delay_1_wrap))
    );
}

void gen_01_delay_1_write(number v) {
    this->gen_01_delay_1_writer = this->gen_01_delay_1_reader;
    this->gen_01_delay_1_buffer[(Index)this->gen_01_delay_1_writer] = v;
}

number gen_01_delay_1_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delay_1__maxdelay : size);
    number val = this->gen_01_delay_1_read(effectiveSize, 0);
    this->gen_01_delay_1_write(v);
    this->gen_01_delay_1_step();
    return val;
}

array<Index, 2> gen_01_delay_1_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delay_1_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_delay_1_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delay_1_init() {
    auto result = this->gen_01_delay_1_calcSizeInSamples();
    this->gen_01_delay_1__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_1_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delay_1_wrap = requestedSizeInSamples - 1;
}

void gen_01_delay_1_clear() {
    this->gen_01_delay_1_buffer->setZero();
}

void gen_01_delay_1_reset() {
    auto result = this->gen_01_delay_1_calcSizeInSamples();
    this->gen_01_delay_1__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delay_1_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delay_1_buffer);
    this->gen_01_delay_1_wrap = this->gen_01_delay_1_buffer->getSize() - 1;
    this->gen_01_delay_1_clear();

    if (this->gen_01_delay_1_reader >= this->gen_01_delay_1__maxdelay || this->gen_01_delay_1_writer >= this->gen_01_delay_1__maxdelay) {
        this->gen_01_delay_1_reader = 0;
        this->gen_01_delay_1_writer = 0;
    }
}

void gen_01_delay_1_dspsetup() {
    this->gen_01_delay_1_reset();
}

number gen_01_delay_1_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_delay_1_size() {
    return this->gen_01_delay_1__maxdelay;
}

number gen_01_slide_16_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_16_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_16_prev = this->gen_01_slide_16_prev + ((x > this->gen_01_slide_16_prev ? iup : idown)) * temp;
    return this->gen_01_slide_16_prev;
}

void gen_01_slide_16_reset() {
    this->gen_01_slide_16_prev = 0;
}

number gen_01_slide_21_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_21_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_21_prev = this->gen_01_slide_21_prev + ((x > this->gen_01_slide_21_prev ? iup : idown)) * temp;
    return this->gen_01_slide_21_prev;
}

void gen_01_slide_21_reset() {
    this->gen_01_slide_21_prev = 0;
}

number gen_01_slide_26_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_26_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_26_prev = this->gen_01_slide_26_prev + ((x > this->gen_01_slide_26_prev ? iup : idown)) * temp;
    return this->gen_01_slide_26_prev;
}

void gen_01_slide_26_reset() {
    this->gen_01_slide_26_prev = 0;
}

number gen_01_slide_36_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_36_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_36_prev = this->gen_01_slide_36_prev + ((x > this->gen_01_slide_36_prev ? iup : idown)) * temp;
    return this->gen_01_slide_36_prev;
}

void gen_01_slide_36_reset() {
    this->gen_01_slide_36_prev = 0;
}

number gen_01_slide_42_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_42_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_42_prev = this->gen_01_slide_42_prev + ((x > this->gen_01_slide_42_prev ? iup : idown)) * temp;
    return this->gen_01_slide_42_prev;
}

void gen_01_slide_42_reset() {
    this->gen_01_slide_42_prev = 0;
}

number gen_01_slide_47_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_47_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_47_prev = this->gen_01_slide_47_prev + ((x > this->gen_01_slide_47_prev ? iup : idown)) * temp;
    return this->gen_01_slide_47_prev;
}

void gen_01_slide_47_reset() {
    this->gen_01_slide_47_prev = 0;
}

number gen_01_slide_52_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_52_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_52_prev = this->gen_01_slide_52_prev + ((x > this->gen_01_slide_52_prev ? iup : idown)) * temp;
    return this->gen_01_slide_52_prev;
}

void gen_01_slide_52_reset() {
    this->gen_01_slide_52_prev = 0;
}

number gen_01_slide_67_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_67_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_67_prev = this->gen_01_slide_67_prev + ((x > this->gen_01_slide_67_prev ? iup : idown)) * temp;
    return this->gen_01_slide_67_prev;
}

void gen_01_slide_67_reset() {
    this->gen_01_slide_67_prev = 0;
}

number gen_01_slide_72_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_72_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_72_prev = this->gen_01_slide_72_prev + ((x > this->gen_01_slide_72_prev ? iup : idown)) * temp;
    return this->gen_01_slide_72_prev;
}

void gen_01_slide_72_reset() {
    this->gen_01_slide_72_prev = 0;
}

number gen_01_slide_77_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_77_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_77_prev = this->gen_01_slide_77_prev + ((x > this->gen_01_slide_77_prev ? iup : idown)) * temp;
    return this->gen_01_slide_77_prev;
}

void gen_01_slide_77_reset() {
    this->gen_01_slide_77_prev = 0;
}

number gen_01_slide_82_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_82_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_82_prev = this->gen_01_slide_82_prev + ((x > this->gen_01_slide_82_prev ? iup : idown)) * temp;
    return this->gen_01_slide_82_prev;
}

void gen_01_slide_82_reset() {
    this->gen_01_slide_82_prev = 0;
}

number gen_01_slide_97_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_97_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_97_prev = this->gen_01_slide_97_prev + ((x > this->gen_01_slide_97_prev ? iup : idown)) * temp;
    return this->gen_01_slide_97_prev;
}

void gen_01_slide_97_reset() {
    this->gen_01_slide_97_prev = 0;
}

number gen_01_slide_103_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_103_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_103_prev = this->gen_01_slide_103_prev + ((x > this->gen_01_slide_103_prev ? iup : idown)) * temp;
    return this->gen_01_slide_103_prev;
}

void gen_01_slide_103_reset() {
    this->gen_01_slide_103_prev = 0;
}

number gen_01_slide_108_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_108_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_108_prev = this->gen_01_slide_108_prev + ((x > this->gen_01_slide_108_prev ? iup : idown)) * temp;
    return this->gen_01_slide_108_prev;
}

void gen_01_slide_108_reset() {
    this->gen_01_slide_108_prev = 0;
}

number gen_01_slide_118_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_118_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_118_prev = this->gen_01_slide_118_prev + ((x > this->gen_01_slide_118_prev ? iup : idown)) * temp;
    return this->gen_01_slide_118_prev;
}

void gen_01_slide_118_reset() {
    this->gen_01_slide_118_prev = 0;
}

number gen_01_slide_124_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_124_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_124_prev = this->gen_01_slide_124_prev + ((x > this->gen_01_slide_124_prev ? iup : idown)) * temp;
    return this->gen_01_slide_124_prev;
}

void gen_01_slide_124_reset() {
    this->gen_01_slide_124_prev = 0;
}

number gen_01_slide_129_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_129_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_129_prev = this->gen_01_slide_129_prev + ((x > this->gen_01_slide_129_prev ? iup : idown)) * temp;
    return this->gen_01_slide_129_prev;
}

void gen_01_slide_129_reset() {
    this->gen_01_slide_129_prev = 0;
}

number gen_01_slide_144_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_144_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_144_prev = this->gen_01_slide_144_prev + ((x > this->gen_01_slide_144_prev ? iup : idown)) * temp;
    return this->gen_01_slide_144_prev;
}

void gen_01_slide_144_reset() {
    this->gen_01_slide_144_prev = 0;
}

number gen_01_slide_150_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_150_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_150_prev = this->gen_01_slide_150_prev + ((x > this->gen_01_slide_150_prev ? iup : idown)) * temp;
    return this->gen_01_slide_150_prev;
}

void gen_01_slide_150_reset() {
    this->gen_01_slide_150_prev = 0;
}

number gen_01_slide_155_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_155_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_155_prev = this->gen_01_slide_155_prev + ((x > this->gen_01_slide_155_prev ? iup : idown)) * temp;
    return this->gen_01_slide_155_prev;
}

void gen_01_slide_155_reset() {
    this->gen_01_slide_155_prev = 0;
}

number gen_01_cycle_187_ph_next(number freq, number reset) {
    RNBO_UNUSED(reset);

    {
        {}
    }

    number pincr = freq * this->gen_01_cycle_187_ph_conv;

    if (this->gen_01_cycle_187_ph_currentPhase < 0.)
        this->gen_01_cycle_187_ph_currentPhase = 1. + this->gen_01_cycle_187_ph_currentPhase;

    if (this->gen_01_cycle_187_ph_currentPhase > 1.)
        this->gen_01_cycle_187_ph_currentPhase = this->gen_01_cycle_187_ph_currentPhase - 1.;

    number tmp = this->gen_01_cycle_187_ph_currentPhase;
    this->gen_01_cycle_187_ph_currentPhase += pincr;
    return tmp;
}

void gen_01_cycle_187_ph_reset() {
    this->gen_01_cycle_187_ph_currentPhase = 0;
}

void gen_01_cycle_187_ph_dspsetup() {
    this->gen_01_cycle_187_ph_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

array<number, 2> gen_01_cycle_187_next(number frequency, number phase_offset) {
    RNBO_UNUSED(phase_offset);

    {
        uint32_t uint_phase;

        {
            uint_phase = this->gen_01_cycle_187_phasei;
        }

        uint32_t idx = (uint32_t)(uint32_rshift(uint_phase, 18));
        number frac = ((BinOpInt)((UBinOpInt)uint_phase & (UBinOpInt)262143)) * 3.81471181759574e-6;
        number y0 = this->gen_01_cycle_187_buffer[(Index)idx];
        number y1 = this->gen_01_cycle_187_buffer[(Index)((UBinOpInt)(idx + 1) & (UBinOpInt)16383)];
        number y = y0 + frac * (y1 - y0);

        {
            uint32_t pincr = (uint32_t)(uint32_trunc(frequency * this->gen_01_cycle_187_f2i));
            this->gen_01_cycle_187_phasei = uint32_add(this->gen_01_cycle_187_phasei, pincr);
        }

        return {y, uint_phase * 0.232830643653869629e-9};
    }
}

void gen_01_cycle_187_dspsetup() {
    this->gen_01_cycle_187_phasei = 0;
    this->gen_01_cycle_187_f2i = (this->samplerate() == 0. ? 0. : (number)4294967296 / this->samplerate());
    this->gen_01_cycle_187_wrap = (long)(this->gen_01_cycle_187_buffer->getSize()) - 1;
}

void gen_01_cycle_187_bufferUpdated() {
    this->gen_01_cycle_187_wrap = (long)(this->gen_01_cycle_187_buffer->getSize()) - 1;
}

void gen_01_cycle_187_reset() {
    this->gen_01_cycle_187_ph_reset();
    this->gen_01_cycle_187_wrap = 0;
    this->gen_01_cycle_187_phasei = 0;
    this->gen_01_cycle_187_f2i = 0;
}

number gen_01_slide_202_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_202_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_202_prev = this->gen_01_slide_202_prev + ((x > this->gen_01_slide_202_prev ? iup : idown)) * temp;
    return this->gen_01_slide_202_prev;
}

void gen_01_slide_202_reset() {
    this->gen_01_slide_202_prev = 0;
}

number gen_01_slide_210_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_210_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_210_prev = this->gen_01_slide_210_prev + ((x > this->gen_01_slide_210_prev ? iup : idown)) * temp;
    return this->gen_01_slide_210_prev;
}

void gen_01_slide_210_reset() {
    this->gen_01_slide_210_prev = 0;
}

number gen_01_cycle_214_ph_next(number freq, number reset) {
    RNBO_UNUSED(reset);

    {
        {}
    }

    number pincr = freq * this->gen_01_cycle_214_ph_conv;

    if (this->gen_01_cycle_214_ph_currentPhase < 0.)
        this->gen_01_cycle_214_ph_currentPhase = 1. + this->gen_01_cycle_214_ph_currentPhase;

    if (this->gen_01_cycle_214_ph_currentPhase > 1.)
        this->gen_01_cycle_214_ph_currentPhase = this->gen_01_cycle_214_ph_currentPhase - 1.;

    number tmp = this->gen_01_cycle_214_ph_currentPhase;
    this->gen_01_cycle_214_ph_currentPhase += pincr;
    return tmp;
}

void gen_01_cycle_214_ph_reset() {
    this->gen_01_cycle_214_ph_currentPhase = 0;
}

void gen_01_cycle_214_ph_dspsetup() {
    this->gen_01_cycle_214_ph_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

array<number, 2> gen_01_cycle_214_next(number frequency, number phase_offset) {
    RNBO_UNUSED(phase_offset);

    {
        uint32_t uint_phase;

        {
            uint_phase = this->gen_01_cycle_214_phasei;
        }

        uint32_t idx = (uint32_t)(uint32_rshift(uint_phase, 18));
        number frac = ((BinOpInt)((UBinOpInt)uint_phase & (UBinOpInt)262143)) * 3.81471181759574e-6;
        number y0 = this->gen_01_cycle_214_buffer[(Index)idx];
        number y1 = this->gen_01_cycle_214_buffer[(Index)((UBinOpInt)(idx + 1) & (UBinOpInt)16383)];
        number y = y0 + frac * (y1 - y0);

        {
            uint32_t pincr = (uint32_t)(uint32_trunc(frequency * this->gen_01_cycle_214_f2i));
            this->gen_01_cycle_214_phasei = uint32_add(this->gen_01_cycle_214_phasei, pincr);
        }

        return {y, uint_phase * 0.232830643653869629e-9};
    }
}

void gen_01_cycle_214_dspsetup() {
    this->gen_01_cycle_214_phasei = 0;
    this->gen_01_cycle_214_f2i = (this->samplerate() == 0. ? 0. : (number)4294967296 / this->samplerate());
    this->gen_01_cycle_214_wrap = (long)(this->gen_01_cycle_214_buffer->getSize()) - 1;
}

void gen_01_cycle_214_bufferUpdated() {
    this->gen_01_cycle_214_wrap = (long)(this->gen_01_cycle_214_buffer->getSize()) - 1;
}

void gen_01_cycle_214_reset() {
    this->gen_01_cycle_214_ph_reset();
    this->gen_01_cycle_214_wrap = 0;
    this->gen_01_cycle_214_phasei = 0;
    this->gen_01_cycle_214_f2i = 0;
}

number gen_01_slide_224_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_224_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_224_prev = this->gen_01_slide_224_prev + ((x > this->gen_01_slide_224_prev ? iup : idown)) * temp;
    return this->gen_01_slide_224_prev;
}

void gen_01_slide_224_reset() {
    this->gen_01_slide_224_prev = 0;
}

number gen_01_slide_231_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_231_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_231_prev = this->gen_01_slide_231_prev + ((x > this->gen_01_slide_231_prev ? iup : idown)) * temp;
    return this->gen_01_slide_231_prev;
}

void gen_01_slide_231_reset() {
    this->gen_01_slide_231_prev = 0;
}

number gen_01_slide_240_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_240_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_240_prev = this->gen_01_slide_240_prev + ((x > this->gen_01_slide_240_prev ? iup : idown)) * temp;
    return this->gen_01_slide_240_prev;
}

void gen_01_slide_240_reset() {
    this->gen_01_slide_240_prev = 0;
}

number gen_01_slide_247_next(number x, number up, number down) {
    number temp = x - this->gen_01_slide_247_prev;
    auto iup = this->safediv(1., this->maximum(1., rnbo_abs(up)));
    auto idown = this->safediv(1., this->maximum(1., rnbo_abs(down)));
    this->gen_01_slide_247_prev = this->gen_01_slide_247_prev + ((x > this->gen_01_slide_247_prev ? iup : idown)) * temp;
    return this->gen_01_slide_247_prev;
}

void gen_01_slide_247_reset() {
    this->gen_01_slide_247_prev = 0;
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_delay_13_dspsetup();
    this->gen_01_delay_12_dspsetup();
    this->gen_01_delay_11_dspsetup();
    this->gen_01_delay_10_dspsetup();
    this->gen_01_delay_9_dspsetup();
    this->gen_01_delay_8_dspsetup();
    this->gen_01_delay_7_dspsetup();
    this->gen_01_delay_6_dspsetup();
    this->gen_01_delay_5_dspsetup();
    this->gen_01_delay_4_dspsetup();
    this->gen_01_delay_3_dspsetup();
    this->gen_01_delay_2_dspsetup();
    this->gen_01_delay_1_dspsetup();
    this->gen_01_cycle_187_ph_dspsetup();
    this->gen_01_cycle_187_dspsetup();
    this->gen_01_cycle_214_ph_dspsetup();
    this->gen_01_cycle_214_dspsetup();
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

void param_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_05_value;
}

void param_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_05_value_set(preset["value"]);
}

void param_06_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_06_value;
}

void param_06_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_06_value_set(preset["value"]);
}

void param_07_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_07_value;
}

void param_07_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_07_value_set(preset["value"]);
}

void param_08_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_08_value;
}

void param_08_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_08_value_set(preset["value"]);
}

void param_09_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_09_value;
}

void param_09_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_09_value_set(preset["value"]);
}

void param_10_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_10_value;
}

void param_10_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_10_value_set(preset["value"]);
}

void param_11_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_11_value;
}

void param_11_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_11_value_set(preset["value"]);
}

Index globaltransport_getSampleOffset(MillisecondTime time) {
    return this->mstosamps(this->maximum(0, time - this->getEngine()->getCurrentTime()));
}

number globaltransport_getTempoAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_tempo[(Index)sampleOffset] : this->globaltransport_lastTempo);
}

number globaltransport_getStateAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_state[(Index)sampleOffset] : this->globaltransport_lastState);
}

number globaltransport_getState(MillisecondTime time) {
    return this->globaltransport_getStateAtSample(this->globaltransport_getSampleOffset(time));
}

number globaltransport_getBeatTime(MillisecondTime time) {
    number i = 2;

    while (i < this->globaltransport_beatTimeChanges->length && this->globaltransport_beatTimeChanges[(Index)(i + 1)] <= time) {
        i += 2;
    }

    i -= 2;
    number beatTimeBase = this->globaltransport_beatTimeChanges[(Index)i];

    if (this->globaltransport_getState(time) == 0)
        return beatTimeBase;

    number beatTimeBaseMsTime = this->globaltransport_beatTimeChanges[(Index)(i + 1)];
    number diff = time - beatTimeBaseMsTime;
    return beatTimeBase + this->mstobeats(diff);
}

bool globaltransport_setTempo(MillisecondTime time, number tempo, bool notify) {
    if ((bool)(notify)) {
        this->processTempoEvent(time, tempo);
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getTempoAtSample(offset) != tempo) {
            this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
            this->globaltransport_beatTimeChanges->push(time);
            fillSignal(this->globaltransport_tempo, this->vs, tempo, offset);
            this->globaltransport_lastTempo = tempo;
            this->globaltransport_tempoNeedsReset = true;
            return true;
        }
    }

    return false;
}

number globaltransport_getTempo(MillisecondTime time) {
    return this->globaltransport_getTempoAtSample(this->globaltransport_getSampleOffset(time));
}

bool globaltransport_setState(MillisecondTime time, number state, bool notify) {
    if ((bool)(notify)) {
        this->processTransportEvent(time, TransportState(state));
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getStateAtSample(offset) != state) {
            fillSignal(this->globaltransport_state, this->vs, state, offset);
            this->globaltransport_lastState = TransportState(state);
            this->globaltransport_stateNeedsReset = true;

            if (state == 0) {
                this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
                this->globaltransport_beatTimeChanges->push(time);
            }

            return true;
        }
    }

    return false;
}

bool globaltransport_setBeatTime(MillisecondTime time, number beattime, bool notify) {
    if ((bool)(notify)) {
        this->processBeatTimeEvent(time, beattime);
        this->globaltransport_notify = true;
        return false;
    } else {
        bool beatTimeHasChanged = false;
        float oldBeatTime = (float)(this->globaltransport_getBeatTime(time));
        float newBeatTime = (float)(beattime);

        if (oldBeatTime != newBeatTime) {
            beatTimeHasChanged = true;
        }

        this->globaltransport_beatTimeChanges->push(beattime);
        this->globaltransport_beatTimeChanges->push(time);
        return beatTimeHasChanged;
    }
}

number globaltransport_getBeatTimeAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getBeatTime(this->getEngine()->getCurrentTime() + msOffset);
}

array<number, 2> globaltransport_getTimeSignature(MillisecondTime time) {
    number i = 3;

    while (i < this->globaltransport_timeSignatureChanges->length && this->globaltransport_timeSignatureChanges[(Index)(i + 2)] <= time) {
        i += 3;
    }

    i -= 3;

    return {
        this->globaltransport_timeSignatureChanges[(Index)i],
        this->globaltransport_timeSignatureChanges[(Index)(i + 1)]
    };
}

array<number, 2> globaltransport_getTimeSignatureAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getTimeSignature(this->getEngine()->getCurrentTime() + msOffset);
}

bool globaltransport_setTimeSignature(MillisecondTime time, number numerator, number denominator, bool notify) {
    if ((bool)(notify)) {
        this->processTimeSignatureEvent(time, (int)(numerator), (int)(denominator));
        this->globaltransport_notify = true;
    } else {
        array<number, 2> currentSig = this->globaltransport_getTimeSignature(time);

        if (currentSig[0] != numerator || currentSig[1] != denominator) {
            this->globaltransport_timeSignatureChanges->push(numerator);
            this->globaltransport_timeSignatureChanges->push(denominator);
            this->globaltransport_timeSignatureChanges->push(time);
            return true;
        }
    }

    return false;
}

void globaltransport_advance() {
    if ((bool)(this->globaltransport_tempoNeedsReset)) {
        fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
        this->globaltransport_tempoNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTempoEvent(this->globaltransport_lastTempo);
        }
    }

    if ((bool)(this->globaltransport_stateNeedsReset)) {
        fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
        this->globaltransport_stateNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTransportEvent(TransportState(this->globaltransport_lastState));
        }
    }

    if (this->globaltransport_beatTimeChanges->length > 2) {
        this->globaltransport_beatTimeChanges[0] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 2)];
        this->globaltransport_beatTimeChanges[1] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 1)];
        this->globaltransport_beatTimeChanges->length = 2;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendBeatTimeEvent(this->globaltransport_beatTimeChanges[0]);
        }
    }

    if (this->globaltransport_timeSignatureChanges->length > 3) {
        this->globaltransport_timeSignatureChanges[0] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 3)];
        this->globaltransport_timeSignatureChanges[1] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 2)];
        this->globaltransport_timeSignatureChanges[2] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 1)];
        this->globaltransport_timeSignatureChanges->length = 3;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTimeSignatureEvent(
                (int)(this->globaltransport_timeSignatureChanges[0]),
                (int)(this->globaltransport_timeSignatureChanges[1])
            );
        }
    }

    this->globaltransport_notify = false;
}

void globaltransport_dspsetup(bool force) {
    if ((bool)(this->globaltransport_setupDone) && (bool)(!(bool)(force)))
        return;

    fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
    this->globaltransport_tempoNeedsReset = false;
    fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
    this->globaltransport_stateNeedsReset = false;
    this->globaltransport_setupDone = true;
}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    dspexpr_01_in1 = 0;
    dspexpr_01_in2 = 0.6;
    dspexpr_02_in1 = 0;
    dspexpr_02_in2 = 0.4;
    gen_01_in1 = 0;
    gen_01_in2 = 0;
    gen_01_modRate = 0;
    gen_01_modAmount = 0;
    gen_01_roomSize = 0;
    gen_01_deDiff1 = 0;
    gen_01_damping = 0;
    gen_01_bandwidth = 0;
    gen_01_decay = 0;
    gen_01_inDiff2 = 0;
    gen_01_predelay = 0;
    gen_01_sizeSpeed = 0;
    gen_01_inDiff1 = 0;
    dspexpr_03_in1 = 0;
    dspexpr_03_in2 = 0.6;
    dspexpr_04_in1 = 0;
    dspexpr_04_in2 = 0.4;
    param_01_value = 0.6;
    param_02_value = 0.269;
    param_03_value = 1024;
    param_04_value = 0.5;
    param_05_value = 0.0005;
    param_06_value = 0.7;
    param_07_value = 0.625;
    param_08_value = 0.75;
    param_09_value = 1;
    param_10_value = 0.9995;
    param_11_value = 30;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    signals[1] = nullptr;
    signals[2] = nullptr;
    signals[3] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    gen_01_history_16_value = 0;
    gen_01_history_15_value = 0;
    gen_01_history_14_value = 0;
    gen_01_delay_13__maxdelay = 0;
    gen_01_delay_13_sizemode = 0;
    gen_01_delay_13_wrap = 0;
    gen_01_delay_13_reader = 0;
    gen_01_delay_13_writer = 0;
    gen_01_delay_12__maxdelay = 0;
    gen_01_delay_12_sizemode = 0;
    gen_01_delay_12_wrap = 0;
    gen_01_delay_12_reader = 0;
    gen_01_delay_12_writer = 0;
    gen_01_delay_11__maxdelay = 0;
    gen_01_delay_11_sizemode = 0;
    gen_01_delay_11_wrap = 0;
    gen_01_delay_11_reader = 0;
    gen_01_delay_11_writer = 0;
    gen_01_delay_10__maxdelay = 0;
    gen_01_delay_10_sizemode = 0;
    gen_01_delay_10_wrap = 0;
    gen_01_delay_10_reader = 0;
    gen_01_delay_10_writer = 0;
    gen_01_delay_9__maxdelay = 0;
    gen_01_delay_9_sizemode = 0;
    gen_01_delay_9_wrap = 0;
    gen_01_delay_9_reader = 0;
    gen_01_delay_9_writer = 0;
    gen_01_delay_8__maxdelay = 0;
    gen_01_delay_8_sizemode = 0;
    gen_01_delay_8_wrap = 0;
    gen_01_delay_8_reader = 0;
    gen_01_delay_8_writer = 0;
    gen_01_delay_7__maxdelay = 0;
    gen_01_delay_7_sizemode = 0;
    gen_01_delay_7_wrap = 0;
    gen_01_delay_7_reader = 0;
    gen_01_delay_7_writer = 0;
    gen_01_delay_6__maxdelay = 0;
    gen_01_delay_6_sizemode = 0;
    gen_01_delay_6_wrap = 0;
    gen_01_delay_6_reader = 0;
    gen_01_delay_6_writer = 0;
    gen_01_delay_5__maxdelay = 0;
    gen_01_delay_5_sizemode = 0;
    gen_01_delay_5_wrap = 0;
    gen_01_delay_5_reader = 0;
    gen_01_delay_5_writer = 0;
    gen_01_delay_4__maxdelay = 0;
    gen_01_delay_4_sizemode = 0;
    gen_01_delay_4_wrap = 0;
    gen_01_delay_4_reader = 0;
    gen_01_delay_4_writer = 0;
    gen_01_delay_3__maxdelay = 0;
    gen_01_delay_3_sizemode = 0;
    gen_01_delay_3_wrap = 0;
    gen_01_delay_3_reader = 0;
    gen_01_delay_3_writer = 0;
    gen_01_delay_2__maxdelay = 0;
    gen_01_delay_2_sizemode = 0;
    gen_01_delay_2_wrap = 0;
    gen_01_delay_2_reader = 0;
    gen_01_delay_2_writer = 0;
    gen_01_delay_1__maxdelay = 0;
    gen_01_delay_1_sizemode = 0;
    gen_01_delay_1_wrap = 0;
    gen_01_delay_1_reader = 0;
    gen_01_delay_1_writer = 0;
    gen_01_slide_16_prev = 0;
    gen_01_slide_21_prev = 0;
    gen_01_slide_26_prev = 0;
    gen_01_slide_36_prev = 0;
    gen_01_slide_42_prev = 0;
    gen_01_slide_47_prev = 0;
    gen_01_slide_52_prev = 0;
    gen_01_slide_67_prev = 0;
    gen_01_slide_72_prev = 0;
    gen_01_slide_77_prev = 0;
    gen_01_slide_82_prev = 0;
    gen_01_slide_97_prev = 0;
    gen_01_slide_103_prev = 0;
    gen_01_slide_108_prev = 0;
    gen_01_slide_118_prev = 0;
    gen_01_slide_124_prev = 0;
    gen_01_slide_129_prev = 0;
    gen_01_slide_144_prev = 0;
    gen_01_slide_150_prev = 0;
    gen_01_slide_155_prev = 0;
    gen_01_cycle_187_ph_currentPhase = 0;
    gen_01_cycle_187_ph_conv = 0;
    gen_01_cycle_187_wrap = 0;
    gen_01_slide_202_prev = 0;
    gen_01_slide_210_prev = 0;
    gen_01_cycle_214_ph_currentPhase = 0;
    gen_01_cycle_214_ph_conv = 0;
    gen_01_cycle_214_wrap = 0;
    gen_01_slide_224_prev = 0;
    gen_01_slide_231_prev = 0;
    gen_01_slide_240_prev = 0;
    gen_01_slide_247_prev = 0;
    gen_01_setupDone = false;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    param_03_lastValue = 0;
    param_04_lastValue = 0;
    param_05_lastValue = 0;
    param_06_lastValue = 0;
    param_07_lastValue = 0;
    param_08_lastValue = 0;
    param_09_lastValue = 0;
    param_10_lastValue = 0;
    param_11_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_tempoNeedsReset = false;
    globaltransport_lastTempo = 120;
    globaltransport_state = nullptr;
    globaltransport_stateNeedsReset = false;
    globaltransport_lastState = 0;
    globaltransport_beatTimeChanges = { 0, 0 };
    globaltransport_timeSignatureChanges = { 4, 4, 0 };
    globaltransport_notify = false;
    globaltransport_setupDone = false;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number dspexpr_01_in1;
    number dspexpr_01_in2;
    number dspexpr_02_in1;
    number dspexpr_02_in2;
    number gen_01_in1;
    number gen_01_in2;
    number gen_01_modRate;
    number gen_01_modAmount;
    number gen_01_roomSize;
    number gen_01_deDiff1;
    number gen_01_damping;
    number gen_01_bandwidth;
    number gen_01_decay;
    number gen_01_inDiff2;
    number gen_01_predelay;
    number gen_01_sizeSpeed;
    number gen_01_inDiff1;
    number dspexpr_03_in1;
    number dspexpr_03_in2;
    number dspexpr_04_in1;
    number dspexpr_04_in2;
    number param_01_value;
    number param_02_value;
    number param_03_value;
    number param_04_value;
    number param_05_value;
    number param_06_value;
    number param_07_value;
    number param_08_value;
    number param_09_value;
    number param_10_value;
    number param_11_value;
    MillisecondTime _currentTime;
    SampleIndex audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[4];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    number gen_01_history_16_value;
    number gen_01_history_15_value;
    number gen_01_history_14_value;
    Float64BufferRef gen_01_delay_13_buffer;
    Index gen_01_delay_13__maxdelay;
    Int gen_01_delay_13_sizemode;
    Index gen_01_delay_13_wrap;
    Int gen_01_delay_13_reader;
    Int gen_01_delay_13_writer;
    Float64BufferRef gen_01_delay_12_buffer;
    Index gen_01_delay_12__maxdelay;
    Int gen_01_delay_12_sizemode;
    Index gen_01_delay_12_wrap;
    Int gen_01_delay_12_reader;
    Int gen_01_delay_12_writer;
    Float64BufferRef gen_01_delay_11_buffer;
    Index gen_01_delay_11__maxdelay;
    Int gen_01_delay_11_sizemode;
    Index gen_01_delay_11_wrap;
    Int gen_01_delay_11_reader;
    Int gen_01_delay_11_writer;
    Float64BufferRef gen_01_delay_10_buffer;
    Index gen_01_delay_10__maxdelay;
    Int gen_01_delay_10_sizemode;
    Index gen_01_delay_10_wrap;
    Int gen_01_delay_10_reader;
    Int gen_01_delay_10_writer;
    Float64BufferRef gen_01_delay_9_buffer;
    Index gen_01_delay_9__maxdelay;
    Int gen_01_delay_9_sizemode;
    Index gen_01_delay_9_wrap;
    Int gen_01_delay_9_reader;
    Int gen_01_delay_9_writer;
    Float64BufferRef gen_01_delay_8_buffer;
    Index gen_01_delay_8__maxdelay;
    Int gen_01_delay_8_sizemode;
    Index gen_01_delay_8_wrap;
    Int gen_01_delay_8_reader;
    Int gen_01_delay_8_writer;
    Float64BufferRef gen_01_delay_7_buffer;
    Index gen_01_delay_7__maxdelay;
    Int gen_01_delay_7_sizemode;
    Index gen_01_delay_7_wrap;
    Int gen_01_delay_7_reader;
    Int gen_01_delay_7_writer;
    Float64BufferRef gen_01_delay_6_buffer;
    Index gen_01_delay_6__maxdelay;
    Int gen_01_delay_6_sizemode;
    Index gen_01_delay_6_wrap;
    Int gen_01_delay_6_reader;
    Int gen_01_delay_6_writer;
    Float64BufferRef gen_01_delay_5_buffer;
    Index gen_01_delay_5__maxdelay;
    Int gen_01_delay_5_sizemode;
    Index gen_01_delay_5_wrap;
    Int gen_01_delay_5_reader;
    Int gen_01_delay_5_writer;
    Float64BufferRef gen_01_delay_4_buffer;
    Index gen_01_delay_4__maxdelay;
    Int gen_01_delay_4_sizemode;
    Index gen_01_delay_4_wrap;
    Int gen_01_delay_4_reader;
    Int gen_01_delay_4_writer;
    Float64BufferRef gen_01_delay_3_buffer;
    Index gen_01_delay_3__maxdelay;
    Int gen_01_delay_3_sizemode;
    Index gen_01_delay_3_wrap;
    Int gen_01_delay_3_reader;
    Int gen_01_delay_3_writer;
    Float64BufferRef gen_01_delay_2_buffer;
    Index gen_01_delay_2__maxdelay;
    Int gen_01_delay_2_sizemode;
    Index gen_01_delay_2_wrap;
    Int gen_01_delay_2_reader;
    Int gen_01_delay_2_writer;
    Float64BufferRef gen_01_delay_1_buffer;
    Index gen_01_delay_1__maxdelay;
    Int gen_01_delay_1_sizemode;
    Index gen_01_delay_1_wrap;
    Int gen_01_delay_1_reader;
    Int gen_01_delay_1_writer;
    number gen_01_slide_16_prev;
    number gen_01_slide_21_prev;
    number gen_01_slide_26_prev;
    number gen_01_slide_36_prev;
    number gen_01_slide_42_prev;
    number gen_01_slide_47_prev;
    number gen_01_slide_52_prev;
    number gen_01_slide_67_prev;
    number gen_01_slide_72_prev;
    number gen_01_slide_77_prev;
    number gen_01_slide_82_prev;
    number gen_01_slide_97_prev;
    number gen_01_slide_103_prev;
    number gen_01_slide_108_prev;
    number gen_01_slide_118_prev;
    number gen_01_slide_124_prev;
    number gen_01_slide_129_prev;
    number gen_01_slide_144_prev;
    number gen_01_slide_150_prev;
    number gen_01_slide_155_prev;
    number gen_01_cycle_187_ph_currentPhase;
    number gen_01_cycle_187_ph_conv;
    Float64BufferRef gen_01_cycle_187_buffer;
    long gen_01_cycle_187_wrap;
    uint32_t gen_01_cycle_187_phasei;
    SampleValue gen_01_cycle_187_f2i;
    number gen_01_slide_202_prev;
    number gen_01_slide_210_prev;
    number gen_01_cycle_214_ph_currentPhase;
    number gen_01_cycle_214_ph_conv;
    Float64BufferRef gen_01_cycle_214_buffer;
    long gen_01_cycle_214_wrap;
    uint32_t gen_01_cycle_214_phasei;
    SampleValue gen_01_cycle_214_f2i;
    number gen_01_slide_224_prev;
    number gen_01_slide_231_prev;
    number gen_01_slide_240_prev;
    number gen_01_slide_247_prev;
    bool gen_01_setupDone;
    number param_01_lastValue;
    number param_02_lastValue;
    number param_03_lastValue;
    number param_04_lastValue;
    number param_05_lastValue;
    number param_06_lastValue;
    number param_07_lastValue;
    number param_08_lastValue;
    number param_09_lastValue;
    number param_10_lastValue;
    number param_11_lastValue;
    signal globaltransport_tempo;
    bool globaltransport_tempoNeedsReset;
    number globaltransport_lastTempo;
    signal globaltransport_state;
    bool globaltransport_stateNeedsReset;
    number globaltransport_lastState;
    list globaltransport_beatTimeChanges;
    list globaltransport_timeSignatureChanges;
    bool globaltransport_notify;
    bool globaltransport_setupDone;
    number stackprotect_count;
    DataRef gen_01_delay_13_bufferobj;
    DataRef gen_01_delay_12_bufferobj;
    DataRef gen_01_delay_11_bufferobj;
    DataRef gen_01_delay_10_bufferobj;
    DataRef gen_01_delay_9_bufferobj;
    DataRef gen_01_delay_8_bufferobj;
    DataRef gen_01_delay_7_bufferobj;
    DataRef gen_01_delay_6_bufferobj;
    DataRef gen_01_delay_5_bufferobj;
    DataRef gen_01_delay_4_bufferobj;
    DataRef gen_01_delay_3_bufferobj;
    DataRef gen_01_delay_2_bufferobj;
    DataRef gen_01_delay_1_bufferobj;
    DataRef RNBODefaultSinus;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* creaternbomatic()
{
    return new rnbomatic();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return creaternbomatic;
}

} // end RNBO namespace

