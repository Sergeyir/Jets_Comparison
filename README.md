# Overview

This is a simple project that allows to compare jets spectra \frac{d\sigma}{dp_T dy} for different event generators - Pythia8(https://pythia.org/), Herwig7 (https://herwig.hepforge.org/) and Powheg (https://powhegbox.mib.infn.it/) in pp collisions

# Required packages

- Root6       https://root.cern/ <br />
- Pythia8     https://pythia.org/ <br />
- Herwig7     https://herwig.hepforge.org/ <br />
- Powheg-Box  https://powhegbox.mib.infn.it/ <br />
- FastJet3    http://fastjet.fr/ <br />

# Installation

To download run

```sh
git clone https://github.com/Sergeyir/Jets_Comparison
```

Before running any macro create directories in directory you've downloaded this project

```sh
mkdir -p data/Jets
```

# Pythia

Paths in Makefile must be corrected for your configuration. Pythia will generate and process events and write jets spectra output as .txt file.

Before launch change setting to yours in pythia_jets.cc

To launch:

```sh
make pythia_jets
./pythia_jets
```

# Herwig

First you need activate Herwig and go to directory Hewig_gen. Then you can generate events in Herwig after you change event genetation parameters to yours in Herwig_gen.in, snippets/PPCollider.in and in snippets/HepMC.in. Herwig will write output as .HepMC file

```sh
Herwig read Herwig_gen.in
```

After generation is done do similar thing with herwig_jets.cc as with pythia_jets.cc

```sh
make herwig_jets
./herwig_jets
```

# Powheg

Not ready yet

# Comparing jets

After getting spectra for different programms set your parameters and output and input files paths in jetsComparison.cc and run it via root

```sh
root jetsComparison.cc
```

This macro will draw all your specified spectra as root graphs (TGraph) on signle canvas and write it as .png file.
