#include <iostream>

void ProgressBar(float progress)
{
	int barWidth = 95;

	std::cout << "[";
	int pos = barWidth * progress;

	for (int i = 0; i < barWidth; ++i)
	{
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] " << int(progress * 100.0);
	printf("%\r");
	std::cout.flush();
}
