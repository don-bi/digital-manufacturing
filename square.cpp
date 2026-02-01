#include <format>

#include "square.h"

int main(int argc, char** argv) {
    using namespace std;
	int height, width;
	cout << "What should the height of the rectangle be?\n";
	cin >> height;
	cout << "What should the width of the rectangle be?\n";
    cin >> width;

	ofstream f {"square.svg", std::ios::binary};
	// f << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    f << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    f << std::format(
        "<rect x=\"3\" y=\"3\" width=\"{}mm\" height=\"{}mm\" "
        "style=\"stroke-width:1;stroke:rgb(0,0,0);fill:none\" />\n",
        width, height
    );
	f << "</svg>";
}
