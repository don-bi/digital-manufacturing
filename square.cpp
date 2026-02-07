#include <iostream>
#include <fstream>
#include <format>
#include <string>
#include <vector>

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
        stroke="white"
        stroke-width="1px" 
        vector-effect="non-scaling-stroke"
    />
    )", x, y);
    return hole_str;
}

std::string t_slot(float x, float y, float angle_deg) {
    const float screw_r = SCREW_RADIUS; // screw half-width
    const float nut_r = 0.1f; // nut half-width (0.2 / 2)
    
    // Y-coordinates relative to y center
    const float y_top_screw = -0.25f;
    const float y_top_nut   = -0.16f;
    const float y_bot_nut   = -0.09f;
    const float y_bot_screw = 0.0f;
    
    std::string path_data = std::format(
        "M {0} {2} "  // Top-Left Screw (x-r, y_top)
        "L {1} {2} "  // Top-Right Screw (x+r, y_top)
        "L {1} {3} "  // to Nut Top edge
        "L {4} {3} "  // Nut Right edge
        "L {4} {5} "  // Nut Bottom edge
        "L {1} {5} "  // Screw edge
        "L {1} {6} "  // ottom Screw
        "L {0} {6} "  // Bottom-Left Screw
        "L {0} {5} "  // Nut Bottom edge
        "L {7} {5} "  // Nut Left edge
        "L {7} {3} "  // Nut Top edge
        "L {0} {3} "  // Screw edge
        "Z",          // Close path
        x - screw_r,  // {0} Left Screw X
        x + screw_r,  // {1} Right Screw X
        y + y_top_screw, // {2} Top Screw Y
        y + y_top_nut,   // {3} Top Nut Y
        x + nut_r,       // {4} Right Nut X
        y + y_bot_nut,   // {5} Bottom Nut Y
        y + y_bot_screw, // {6} Bottom Screw Y
        x - nut_r        // {7} Left Nut X
    );

    //rotation wrap
    std::string slot_str = std::format(
        "<g transform=\"rotate({2} {0} {1})\"><path d=\"{3}\" fill=\"none\" stroke=\"white\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" /></g>", 
        x, y, angle_deg, path_data
    );

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

    const float available_space = length - ACRYLIC_THICKNESS*2;
    const float min_section_thickness = 0.5 + ACRYLIC_THICKNESS; //0.5 + dividerlen
    int num_sections;

    while (true) {
        cout << "How many sections do you want?\n";
        if (cin >> num_sections && num_sections <= available_space / min_section_thickness) break;
        cout << "Error: Too many sections for this container.\n";
        cin.clear(); cin.ignore(1000, '\n'); // Clear buffer on bad input
    }

    vector<float> section_lens;
    bool continue_prompt = true;
    float remaining_space = available_space;
    while (continue_prompt) {
        cout << "You will choose the length of each section now. It will add a 0.125in divider after each of your section's provided length. The minimum section length is 0.5 inches\n";
        continue_prompt = false;
        for (int i = 0; i < num_sections-1; i ++) {
            if (remaining_space < min_section_thickness) {
                cout << "Error: Not enough space for minimum section length.";
                continue_prompt = true;
                break;
            }
            float curr_sec_len;
            while (true) {
                cout << format("What should the length of section {} be? (in) [Remaining Space: {}]\n", i+1, remaining_space);
                if (cin >> curr_sec_len && curr_sec_len + ACRYLIC_THICKNESS + 0.5 <= remaining_space) break; // the input is good
                cout << "Error: Selected section length is too large.";
                cin.clear(); cin.ignore(1000, '\n'); // Clear buffer on bad input
            }
            remaining_space -= curr_sec_len + ACRYLIC_THICKNESS;
            section_lens.push_back(curr_sec_len);
        }
    }
    section_lens.push_back(remaining_space);

    cout << "Successfully created section lengths of:\n";
    for (float& f : section_lens) {
        cout << f << "in ";
    }
    cout << "\n";

    // ==========================================
    // FILE 1: Base (Rectangle)
    // ==========================================
    ofstream base("base.svg", ios::binary);

    string base_svg = format(R"(<?xml version="1.0" encoding="UTF-8" ?>
    <svg xmlns="http://www.w3.org/2000/svg" version="1.1" 
        width="{0}in" height="{1}in" viewBox="0 0 {0} {1}">
        <rect x="0" y="0" width="{0}" height="{1}"
            fill="none" 
            stroke="white"
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
            stroke="white"
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
            stroke="white"
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