#include <array>
#include <vector> 
#include <fstream>
#include <cmath>

#include "Pythia8/Pythia.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

#include "../utils/ProgressBar.cc"

using namespace Pythia8;
using namespace std;

int main()
{
	Pythia pythia;
	
	pythia.readString("Beams:eCM = 13000.");
	pythia.readString("HardQCD:all = on");
	pythia.readString("Random:setSeed = on");

	pythia.readString("Next:numberShowEvent = 0");
	pythia.readString("Next:numberCount = 0");
 
	pythia.readString("PhaseSpace:pTHatMin = 40");
	pythia.readString("PhaseSpace:pTHatMax = 13000");
	
	pythia.readString("PhaseSpace:mHatMin = 0");
	pythia.readString("PhaseSpace:mHatMax = 13000");
	
	pythia.readString("ParticleDecays:limitTau0 = On");
	pythia.readString("ParticleDecays:tau0Max = 10.0");

	pythia.init();
	
	ofstream output_file("../data/Jets/pythia_pp13000.txt");
	
	array <float, 8> inclusive_jets_entries;
	array <float, 8> inclusive_jets_err;

	array <float, 8> pt_point = {110, 130, 150, 185, 235, 285, 355, 450};	

	array <float, 8> pt_range_low = {100, 120, 140, 160, 210, 260, 310, 400};
	array <float, 8> pt_range_high = {120, 140, 160, 210, 260, 310, 400, 500};

	unsigned const long	nEvent = 1000000;
	const float R_param = 0.4;
	const float rapidity_max = 0.3;
	const float rapidity_min = 0;
	const float jets_min_pt = 20;

	const double pi = 3.14159265359;
	
	std::vector <fastjet::PseudoJet> fjInputs;
	std::vector <fastjet::PseudoJet> sortedJets, inclusiveJets;
	
	fastjet::Strategy strategy = fastjet::Best;
	fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R_param);

	unsigned long njets = 0, accepted_njets = 0, nojets_events;

	float progress = 0;
	float progress_step = 0.01;

	for (unsigned long int iEvent = 0; iEvent < nEvent; ++iEvent)
	{
		if ((float) iEvent/nEvent >= progress)
		{
			ProgressBar(progress); 
			progress += progress_step;
		}

		fastjet::ClusterSequence cluster_seq(fjInputs, jet_def);

		if (!pythia.next()) continue;

		for (unsigned long int ipart = 0; ipart < pythia.event.size(); ipart++) {

			if (!pythia.event[ipart].isFinal()) continue;
			//if (!pythia.event[ipart].isCharged()) continue;
			//if (pythia.event[ipart].id() == 21) continue;
	
			// no neutrinos
			 
			if (pythia.event[ipart].idAbs() == 12 || pythia.event[ipart].idAbs() == 14 || pythia.event[ipart].idAbs() == 16 || pythia.event[ipart].idAbs() == 18) continue;
				 
			fjInputs.push_back( fastjet::PseudoJet(pythia.event[ipart].px(), pythia.event[ipart].py(), pythia.event[ipart].pz(), pythia.event[ipart].e()) );
		}
		
		inclusiveJets = cluster_seq.inclusive_jets(jets_min_pt);
		sortedJets = sorted_by_pt(inclusiveJets);	
		
		if(sortedJets.size() < 1)
		{
			nojets_events++;

			//cout << " No jets found in event " << iEvent << endl;
			continue;
		}
		
		for (int ijet = 0; ijet < sortedJets.size(); ijet++)
		{
			float pT = sortedJets[ijet].perp();
			
			njets++;

			double rap = sortedJets[ijet].rap();

			if (abs(rap) < rapidity_max && abs(rap) > rapidity_min)
			{
				for (int count = 0; count < inclusive_jets_entries.size(); count++)
				{
					if (pT > pt_range_low[count] && pT < pt_range_high[count])
					{	
						inclusive_jets_entries[count] += pythia.info.weight();
						inclusive_jets_err[count] = sqrt(pythia.info.weight()+pow(inclusive_jets_err[count],2));
						accepted_njets++;

						break;
					}
				}
			}
		}
		sortedJets.clear();
		fjInputs.clear();
	}
				
	ProgressBar(1); 
			
	const float sigma_pb = pythia.info.sigmaGen()*1.0E9;
	 
	cout << endl << "Sigma: " << sigma_pb << " pb" << endl << "Accepted jets:" << accepted_njets << " out of " << njets <<  endl;
	cout << "Events without any jets found: " << nojets_events << " out of " << nEvent << endl;
	 
	for (int count = 0; count < inclusive_jets_entries.size(); count++)
	{
		inclusive_jets_entries[count] *= sigma_pb/(njets*(rapidity_max-rapidity_min)*4*pi);
		inclusive_jets_err[count] *= sqrt(sigma_pb/(njets*(rapidity_max-rapidity_min)*4*pi));
		
		output_file << pt_point[count] << "	" << inclusive_jets_entries[count] << " " << inclusive_jets_err[count] << endl;
	}
	
	return 0;
}
