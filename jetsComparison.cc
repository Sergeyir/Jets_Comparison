#include "../lib/CoolGraphsFromTXT.h"

int jetsComparison()
{
	CoolGraphsFromTXT *jets_gr = new CoolGraphsFromTXT("jet p_{T} in p+p #sqrt{s_{NN}} = 7000 GeV", 0, 500, "p_{T}, GeV", "#frac{d^{2} #sigma}{d p_{T} d y}, #frac{pb}{GeV}");

	jets_gr->AddEntry("../data/Jets/pythia_pp7000.txt", "Pythia8", kRed, 20);
	jets_gr->AddEntry("../data/Jets/herwig_pp7000.txt", "Herwig7", kBlue, 22);

	jets_gr->WriteResults("Jets.png", 1);

	return 1;
}
