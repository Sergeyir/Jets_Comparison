#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"

#include "../utils/ProgressBar.cc"
#include "../lib/HepMCReader.h"

using namespace std;

int main()
{
	string input_file_name = "../data/Herwig_pp13000GeV.hepmc";
	ofstream output_file("../data/Jets/herwig_pp13000.txt");
	
	array <float, 8> inclusive_jets_entries, inclusive_jets_err;

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

	unsigned long njets = 0, accepted_njets = 0, nojets_events = 0;

	float progress = 0;
	float progress_step = 0.01;

	HepMCReader reader(input_file_name);

	unsigned long int iEvent = 0;

	double weights_sum_err = 0;

	for (iEvent = 0; iEvent < nEvent; ++iEvent)
	{
		if (reader.isEnd()) break;

		if ((float) iEvent/nEvent >= progress)
		{
			ProgressBar(progress); 
			progress += progress_step;
		}

		reader.ReadNextEvent();
		
		fastjet::ClusterSequence cluster_seq(fjInputs, jet_def);

		for (unsigned long int ipart = 0; ipart < reader.eventSize(); ipart++) {

			if (!reader.isStable(ipart)) continue;
			 
			// no neutrinos
			if (abs(reader.id(ipart)) == 12 || abs(reader.id(ipart)) == 14 || abs(reader.id(ipart)) == 16 || abs(reader.id(ipart)) == 18) continue;
				 
			fjInputs.push_back( fastjet::PseudoJet(reader.px(ipart), reader.py(ipart), reader.pz(ipart), reader.e(ipart)) );
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
						inclusive_jets_entries[count] += reader.weight();
						inclusive_jets_err[count] = sqrt(reader.weight()+pow(inclusive_jets_err[count],2));
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
				
	const double sigma_pb = reader.sigmaGen();
	 
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
