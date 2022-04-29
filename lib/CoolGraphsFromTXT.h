#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TColor.h"
#include "TVirtualPad.h"

using namespace std;

class CoolGraphsFromTXT
{

private:

	double max_y{0}, min_y{0};
	TCanvas *canv;
	TLegend *legend;
	TH1F *hr;

	vector <TGraphErrors*> graphs;

	TFile *output_file;

	void SetGraphStyle(TGraphErrors* graph, Color_t color, const int marker_style) {

		graph->SetMarkerStyle(marker_style);
		graph->SetMarkerColor(color);
		graph->SetMarkerSize(2);

		graph->SetLineColor(color);
	}

	void CheckMaximumY(double y_val)
	{
		if (y_val > max_y) max_y = y_val;
	}

	void CheckMinimumY(double y_val)
	{
		if (y_val < min_y && y_val > 0) min_y = y_val;
	}

public:

	CoolGraphsFromTXT(string title, const int min_x, const int max_x, string XaxisTitle, string YaxisTitle)
	{
		canv = new TCanvas(title.c_str(), title.c_str(), 900, 900);
		
		hr = new TH1F("hr", "hr", 100, min_x, max_x); 
		
		gStyle->SetOptStat(0);

		hr->SetTitle(title.c_str());
		hr->GetXaxis()->SetLabelSize(0.025);
		hr->GetYaxis()->SetLabelSize(0.025);
		
		hr->GetXaxis()->SetTitle(XaxisTitle.c_str());
		hr->GetYaxis()->SetTitle(YaxisTitle.c_str());

		hr->GetXaxis()->SetTitleSize(0.025);
		hr->GetYaxis()->SetTitleSize(0.025);

		legend = new TLegend(0.7, 0.70, 0.93, 0.93);
	}

	/*
	~CoolGraphsFromTXT() {

		delete canv, hr, legend;

		while(!graphs.empty()) {

			delete graphs.back();
			graphs.pop_back();

		}

	}

	*/

	void AddEntry(string input_file_name, string legend_entry, Color_t color, const int marker_style)
	{
		ifstream input_file(input_file_name.c_str());
		
		if (!input_file.is_open())
		{
			printf("\033[31mError: ");
		
			cout << "File " << input_file_name << " not found";

			exit(1);
		}

		graphs.push_back(new TGraphErrors());

		float x_val, y_val, y_err;
	
		int count = 0;

		while (input_file >> x_val >> y_val >> y_err)
		{
			graphs.back()->AddPoint(x_val, y_val);
			graphs.back()->SetPointError(count, 0, y_err);
			count++;

			CheckMaximumY(y_val);
			CheckMinimumY(y_val);
		}

		SetGraphStyle(graphs.back(), color, marker_style);
		legend->AddEntry(graphs.back(), legend_entry.c_str(), "p");
	} 


	void AddEntryRCP(string input_file_name, string legend_entry, Color_t color, const int marker_style, const int do_calc = 0, float N_coll1 = 0, float N_coll2 = 0)
	{
		ifstream input_file(input_file_name.c_str());
		
		if (!input_file.is_open())
		{
			printf("\033[31mError: ");
		
			cout << "File " << input_file_name << " not found";

			exit(1);
		}

		graphs.push_back(new TGraphErrors());

		float x_val, x_val1, x_val2, y_err1, y_err2;
	
		int count = 0;

		while (input_file >> x_val >> x_val1 >> y_err1 >> x_val2 >> y_err2)
		{
			if (do_calc == 1)
			{

				x_val1 = x_val1/x_val2*N_coll2/N_coll1;
				y_err1 = sqrt(y_err1*y_err1 + y_err2*y_err2)/100*x_val1;
			
				cout << x_val << " " << x_val1 << " " << y_err1 << endl;
			}

			else
			{
				x_val1 = (x_val1 + x_val2)/2;
				y_err1 = (y_err1 + y_err2)/2;	
			}
			
			graphs.back()->AddPoint(x_val, x_val1);
			graphs.back()->SetPointError(count, 0, y_err1);
			count++;

			CheckMaximumY(x_val1);

		}

		SetGraphStyle(graphs.back(), color, marker_style);
		legend->AddEntry(graphs.back(), legend_entry.c_str(), "p");
	} 

	void WriteResults(string output_file_name, bool isLog)
	{

		canv->cd();

		hr->SetMaximum(max_y*1.1);
		if (isLog == 1) hr->SetMaximum(max_y*10);
		hr->SetMinimum(0.01);

		if (isLog == 1) gPad->SetLogy();
		hr->Draw();

		for (int count = 0; count < graphs.size(); count++)
		{
			graphs[count]->Draw("p");

		}

		legend->Draw();
		
		canv->SaveAs(output_file_name.c_str());

	}
};
