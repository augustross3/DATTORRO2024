# DATTORRO2024

**Technical Overview**

(Read the original paper here https://ccrma.stanford.edu/~dattorro/EffectDesignPart1.pdf)

This recreation of the Dattorro Reverb in MaxMSP represents a meticulous and modern reinterpretation of the classic algorithm, faithfully adhering to the original specifications while introducing enhancements for contemporary use. The core of this reimagined reverb remains true to Dattorro's design, retaining the "magic numbers" that defined its distinctive sound character. However, it now supports modern sampling rates up to 96kHz, ensuring compatibility with current audio standards and offering improved sound quality.

In addition to maintaining the integrity of the original design, this MaxMSP version introduces a new 'room size' parameter. This addition allows for greater flexibility in shaping the reverb's spatial characteristics, providing users with the ability to tailor the reverb effect more precisely to their needs. The modulation control is another modern enhancement, offering dynamic manipulation of the reverb's texture and depth, thus expanding the creative possibilities.

The implementation in MaxMSP is executed within the Gen environment, ensuring high efficiency and performance. Every component of the algorithm, including the one-pole lowpass and allpass filters, has been meticulously recreated in Gen. This approach not only ensures fidelity to the original algorithm but also leverages the optimization and flexibility that Gen offers.

Furthermore, the development in Gen opens up exciting possibilities for future applications. The algorithm can be exported as a standalone Max object, facilitating ease of integration into different MaxMSP projects. Moreover, there is potential for further expansion into a VST format, making this reverb algorithm accessible in a wide range of digital audio workstations. This adaptability ensures that the Dattorro Reverb, in its new MaxMSP incarnation, will be a valuable tool for audio professionals and enthusiasts alike.

**Usage**
There are currently two available forms in this repo. The .gendsp file contains the raw system along with all parameters set in place with their respective min/max values. This version can dropped into MaxMSP projects for quick use or examined. The .mxf file is a standalone visual patcher that can be used to test out and explore the algorithm with your own files or the ones provided in MaxMSP. This version is pre-built and therefor cannot be examined.

**TODO**
- Max External Object
- C++ Source Code
- Web Demo
