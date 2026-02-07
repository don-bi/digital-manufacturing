#include <iostream>
#include <fstream>
#include <format>
#include <string>

using namespace std;

const float ACRYLIC_THICKNESS = 0.125;
const float HALF_ACRYLIC_THICKNESS = 0.0625;
const float SCREW_RADIUS = 0.043;
const float SCREW_DIAMETER = 0.086;

string screw_hole(float x, float y) {
    // screw diameter is 0.086
    string hole_str = format(R"(
    <circle cx="{0}" cy="{1}" 
        r="0.043" 
        fill="none"
        stroke="black"
        stroke-width="1px" 
        vector-effect="non-scaling-stroke"
    />
    )", x, y);
    return hole_str;
}


// coordinates will be the center of the slot
string t_slot(float x, float y, float angle_deg) {
    string slot_str = format(" <g transform=\"rotate({2} {0} {1})\"> ", x, y, angle_deg);
    slot_str += format(R"(
    <rect x="{0}" y="{1}" width="{2}" height="{3}"
        fill="none" 
        stroke="black"
        stroke-width="1px" 
        vector-effect="non-scaling-stroke"
    />
    )", x-SCREW_RADIUS, y-0.25, SCREW_DIAMETER, 0.25);
    slot_str += format(R"(
    <rect x="{0}" y="{1}" width="{2}" height="{3}"
        fill="none" 
        stroke="black"
        stroke-width="1px" 
        vector-effect="non-scaling-stroke"
    />
    )", x-0.1, y-0.16, 0.2, 0.07);
    slot_str += "</g>";
    return slot_str;
}



int main(int argc, char** argv) {
    float width, length, height;

    while (true) {
        cout << "What should the width of the box be? (in)\n";
        if (cin >> width && width > 0 && width <= 100) break;
        cout << "Error: Invalid width.\n";
        cin.clear(); cin.ignore(1000, '\n');
    }
    while (true) {
        cout << "What should the length of the container be? (in)\n";
        if (cin >> length && length > 0 && length <= 100) break;
        cout << "Error: Invalid length.\n";
        cin.clear(); cin.ignore(1000, '\n');
    }
    while (true) {
        cout << "What should the height of the box be? (in)\n";
        if (cin >> height && height > 0 && height <= 100) break;
        cout << "Error: Invalid height.\n";
        cin.clear(); cin.ignore(1000, '\n'); // Clear buffer on bad input
    }

    // ==========================================
    // FILE 1: Base (Rectangle)
    // ==========================================
    ofstream base("base.svg", ios::binary);

    string base_svg = format(R"(<?xml version="1.0" encoding="UTF-8" ?>
    <svg xmlns="http://www.w3.org/2000/svg" version="1.1" 
        width="{0}in" height="{1}in" viewBox="0 0 {0} {1}">
        <rect x="0" y="0" width="{0}" height="{1}"
            fill="none" 
            stroke="black"
            stroke-width="1px" 
            vector-effect="non-scaling-stroke"
        />
    )", width, length);

    base_svg += screw_hole(HALF_ACRYLIC_THICKNESS, 0.15);
    base_svg += screw_hole(HALF_ACRYLIC_THICKNESS, length - 0.15);
    base_svg += screw_hole(width - HALF_ACRYLIC_THICKNESS, 0.15);
    base_svg += screw_hole(width - HALF_ACRYLIC_THICKNESS, length - 0.15);

    // for x
    int num_screws = width / 3;
    float interval_inc = width / (num_screws + 1.0);
    for (int i = 0; i < num_screws; i++) {
        base_svg += screw_hole(interval_inc * (i+1), HALF_ACRYLIC_THICKNESS);
        base_svg += screw_hole(interval_inc * (i+1), length - HALF_ACRYLIC_THICKNESS);
        base_svg += t_slot(interval_inc * (i+1), length, 0);
    }

    // for y
    num_screws = length / 3;
    interval_inc = length / (num_screws + 1.0);
    for (int i = 0; i < num_screws; i++) {
        base_svg += screw_hole(HALF_ACRYLIC_THICKNESS, interval_inc * (i+1));
        base_svg += screw_hole(width - HALF_ACRYLIC_THICKNESS, interval_inc * (i+1));
        base_svg += t_slot(0, interval_inc * (i+1), 90);
    }

    base_svg += "</svg>";

    base << base_svg;
    base.close();

    // ==========================================
    // FILE 2: Length Wall
    // ==========================================
    ofstream l_wall("l_wall.svg", ios::binary);

    string l_wall_svg = format(R"(<?xml version="1.0" encoding="UTF-8" ?>
    <svg xmlns="http://www.w3.org/2000/svg" version="1.1" 
        width="{0}in" height="{1}in" viewBox="0 0 {0} {1}">
        <rect x="0" y="0" width="{0}" height="{1}"
            fill="none" 
            stroke="black"
            stroke-width="1px" 
            vector-effect="non-scaling-stroke"
        />
    )", length, height);

    l_wall_svg += t_slot(0.15, height, 0);
    l_wall_svg += t_slot(length - 0.15, height, 0);
    l_wall_svg += screw_hole(HALF_ACRYLIC_THICKNESS, height / 2);
    l_wall_svg += screw_hole(length - HALF_ACRYLIC_THICKNESS, height / 2);

    num_screws = length / 3;
    interval_inc = length / (num_screws + 1.0);
    for (int i = 0; i < num_screws; i++) {
        l_wall_svg += t_slot(interval_inc * (i+1), height, 0);
    }
    l_wall_svg += "</svg>";

    l_wall << l_wall_svg;
    l_wall.close();

    // ==========================================
    // FILE 2: Width Wall
    // ==========================================
    ofstream w_wall("w_wall.svg", ios::binary);

    string w_wall_svg = format(R"(<?xml version="1.0" encoding="UTF-8" ?>
    <svg xmlns="http://www.w3.org/2000/svg" version="1.1" 
        width="{0}in" height="{1}in" viewBox="0 0 {0} {1}">
        <rect x="0" y="0" width="{0}" height="{1}"
            fill="none" 
            stroke="black"
            stroke-width="1px" 
            vector-effect="non-scaling-stroke"
        />
    )", width-ACRYLIC_THICKNESS*2, height);

    w_wall_svg += t_slot(0, height / 2, 90);
    w_wall_svg += t_slot(width-ACRYLIC_THICKNESS*2, height / 2, 270);
    
    // for x
    num_screws = width / 3;
    interval_inc = width / (num_screws + 1.0);
    for (int i = 0; i < num_screws; i++) {
        w_wall_svg += t_slot(interval_inc * (i+1) - ACRYLIC_THICKNESS, height, 0);
    }


    w_wall_svg += "</svg>";

    w_wall << w_wall_svg;
    w_wall.close();

    cout << "Files generated\n";
    return 0;
}