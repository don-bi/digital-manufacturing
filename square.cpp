#include <format>
#include <stdexcept>

#include "square.h"

int main(int argc, char** argv) {
    using namespace std;
	float height, width;
    while (1) {
        cout << "What should the height of the rectangle be? (mm)\n";
        cin >> height;
        if (height > 0 && height <= 50.8) break;
        cout << "Error: Height is out of bounds.";
    }
    while (1) {
        cout << "What should the width of the rectangle be? (mm)\n";
        cin >> width;
        if (width > 0 && width <= 50.8) break;
        cout << "Error: Width is out of bounds.";
    }

	ofstream f {"square.svg", std::ios::binary};
	f << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    f << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    f << std::format(
        "<rect x=\"3\" y=\"3\" width=\"{}mm\" height=\"{}mm\" "
        "style=\"stroke-width:1;stroke:rgb(0,0,0);fill:none\" />\n",
        width, height
    );
	f << "</svg>";
}
