#include "Pythia8/Pythia.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

#include <array>
#include <vector> 
#include <fstream>
#include "cmath"

using namespace Pythia8;
using namespace std;

int main() {

	Pythia pythia;
	
	pythia.readString("Beams:eCM = 7000.");
	pythia.readString("HardQCD:all = on");
	pythia.readString("Random:setSeed = on");
 
	pythia.readString("PhaseSpace:pTHatMin = 40");
	pythia.readString("PhaseSpace:pTHatMax = 7000");
	
	pythia.readString("PhaseSpace:mHatMin = 0");
	pythia.readString("PhaseSpace:mHatMax = 7000");
	
	pythia.readString("ParticleDecays:limitTau0 = On");
	pythia.readString("ParticleDecays:tau0Max = 10.0");

	pythia.init();
	
	ofstream output_file("../data/Jets/pythia_pp7000.txt");
	
	array <float, 8> inclusive_jets_entries;

	array <float, 8> pt_point = {70, 95, 135, 185, 235, 285, 355, 450};

	array <float, 8> pt_range_low = {60, 80, 110, 160, 210, 260, 310, 400};
	array <float, 8> pt_range_high = {80, 110, 160, 210, 260, 310, 400, 500};
	
	const int	nEvent = 100000;
	const float R = 0.4;
	const float rapidity_max = 0.3;
	
	std::vector <fastjet::PseudoJet> fjInputs;
	std::vector <fastjet::PseudoJet> sortedJets, inclusiveJets;
	
	fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);

	unsigned long long int njets = 0, accepted_njets = 0;
	
	for (unsigned long int iEvent = 0; iEvent < nEvent; ++iEvent) {

		fastjet::ClusterSequence cluster_seq(fjInputs, jet_def);

		if (!pythia.next()) continue;

		for (unsigned long int ipart = 0; ipart < pythia.event.size(); ipart++) {

			if (!pythia.event[ipart].isFinal()) continue;
	
			// 3.1 < eta < 4

			if (pythia.event[ipart].eta() < 3.1 || pythia.event[ipart].eta() > 4) continue;
		 
			// no neutrinos
			 
			if (pythia.event[ipart].idAbs() == 12 || pythia.event[ipart].idAbs() == 14 || pythia.event[ipart].idAbs() == 16 || pythia.event[ipart].idAbs() == 18) continue;
				 
			fjInputs.push_back( fastjet::PseudoJet(pythia.event[ipart].px(), pythia.event[ipart].py(), pythia.event[ipart].pz(), pythia.event[ipart].e()) );
			 
		}
		
		inclusiveJets = cluster_seq.inclusive_jets(50.0);
		sortedJets = sorted_by_pt(inclusiveJets);	
		
		if(sortedJets.size() < 1) {

			cout << " No jets found in event " << iEvent << endl;
			continue;

		}
		
		for (int ijet = 0; ijet < sortedJets.size(); ijet++) {
			
			float pT = sortedJets[ijet].perp();
			
			njets++;

			if (abs(sortedJets[ijet].rap()) < rapidity_max) {
				
				for (int count = 0; count < inclusive_jets_entries.size(); count++) {

					if (pT > pt_range_low[count] && pT < pt_range_high[count]) {

						inclusive_jets_entries[count] += pythia.info.weight();
						accepted_njets++;

						break;

					}
				}
			}
		}
			
		sortedJets.clear();
		fjInputs.clear();

	}
				
	pythia.stat();
			
	const float sigma_pb = pythia.info.sigmaGen()*1.0E9;
	const float weights_sum = pythia.info.weightSum();
	 
	cout << endl << "Sigma: " << sigma_pb << " pb" << endl << "Accepted jets:" << accepted_njets << " out of " << njets << endl;
	 
	for (int count = 0; count < inclusive_jets_entries.size(); count++) {

		inclusive_jets_entries[count] /= sigma_pb*weights_sum*rapidity_max*4;
		
		output_file << pt_point[count] << inclusive_jets_entries[count];

	}
	
	return 0;
}
