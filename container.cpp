#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

const float ACRYLIC_THICKNESS = 0.125;
const float HALF_ACRYLIC_THICKNESS = 0.0625;
const float SCREW_RADIUS = 0.043;
const float SCREW_DIAMETER = 0.086;
const float PI = 3.1415926535f;

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

std::string generate_text(string text, float x, float y, float container_w, float container_h, float rotation_deg) {
    if (text.empty()) return "";

    // determine available space based on rotation
    bool is_vertical = (static_cast<int>(abs(rotation_deg)) % 180 == 90);

    float space_for_text_length = is_vertical ? container_h : container_w;
    float space_for_text_height = is_vertical ? container_w : container_h;

    // Margins
    float usable_length = space_for_text_length * 0.8f;
    float usable_height = space_for_text_height * 0.6f;
    
    //estiamte aspect ratio of characters (width / height)
    float char_aspect = 0.6f; 
    
    // 1. scale based on string length fitting into the usable length
    float size_by_len = usable_length / (text.length() * char_aspect);
    
    // 2. scale based on font height fitting into the usable height
    float size_by_height = usable_height;

    // pick the smaller constraint
    float font_size = std::min(size_by_len, size_by_height);

    // caps to prevent massive or microscopic text
    font_size = std::min(font_size, 1.25f); 
    font_size = std::max(font_size, 0.15f);

    string text_svg = format(R"SVG(
    <text x="{0}" y="{1}" 
            transform="rotate({4} {0} {1})"
            font-family="Arial, sans-serif" 
            font-size="{2}" 
            text-anchor="middle" 
            dominant-baseline="middle" 
            fill="blue" 
            stroke="none">
            {3}
        </text>
    )SVG", x, y, font_size, text, rotation_deg);


    return text_svg;
}

void recursive_tree(float x, float y, float length, float angle_deg, int depth, string& svg_content) {
    if (depth == 0) return;

    //convert to radians
    float angle_rad = angle_deg * (PI / 180.0f);

    float x2 = x + length * cos(angle_rad);
    float y2 = y + length * sin(angle_rad);

    svg_content += format(R"(<line x1="{}" y1="{}" x2="{}" y2="{}" stroke="blue" stroke-width="0.02" />)", x, y, x2, y2);

    //reduce length by 0.7, change angle by +/- 25 degrees
    recursive_tree(x2, y2, length * 0.7f, angle_deg - 25, depth - 1, svg_content);
    recursive_tree(x2, y2, length * 0.7f, angle_deg + 25, depth - 1, svg_content);
}

string generate_fractal_face(float width, float height) {
    string fractal_svg = "";
    recursive_tree(width / 2.0f, height * 0.9f, height * 0.25f, -90.0f, 9, fractal_svg);
    return fractal_svg;
}

string read_svg_file(const string& filename) {
    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "Warning: Could not open " << filename << ". Logo will be missing.\n";
        return "";
    }
    stringstream buffer;
    buffer << infile.rdbuf();
    string content = buffer.str();

    size_t xml_pos = content.find("<?xml");
    if (xml_pos != string::npos) {
        size_t end_pos = content.find("?>", xml_pos);
        if (end_pos != string::npos) {
            content.erase(xml_pos, end_pos - xml_pos + 2);
        }
    }
    
    size_t doc_pos = content.find("<!DOCTYPE");
    if (doc_pos != string::npos) {
        size_t end_pos = content.find(">", doc_pos);
        if (end_pos != string::npos) {
            content.erase(doc_pos, end_pos - doc_pos + 1);
        }
    }

    return content;
}

void replace_all(string& str, const string& from, const string& to) {
    if(from.empty()) return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

string generate_logo_face(float width, float height) {
    // calculate logo positioning
    float img_w = width * 0.7f;
    float img_h = height * 0.5f;
    float img_x = (width - img_w) / 2.0f;
    float img_y = height * 0.1f; 

    // read svg file content
    string logo_raw_content = read_svg_file("columbia_engineering.svg");

    // replace colors
    replace_all(logo_raw_content, "fill=\"#000000\"", "fill=\"blue\"");
    replace_all(logo_raw_content, "fill=\"black\"",    "fill=\"blue\"");
    replace_all(logo_raw_content, "stroke=\"#000000\"", "stroke=\"blue\"");
    replace_all(logo_raw_content, "stroke=\"black\"",   "stroke=\"blue\"");
    
    replace_all(logo_raw_content, ":#000000", ":blue");
    replace_all(logo_raw_content, ":black",   ":blue");
    
    // extract the actual viewBox
    string logo_viewbox = "0 0 100 100"; 
    size_t vb_pos = logo_raw_content.find("viewBox=\"");
    if (vb_pos != string::npos) {
        size_t start = vb_pos + 9; // Skip past viewBox="
        size_t end = logo_raw_content.find("\"", start);
        if (end != string::npos) {
            logo_viewbox = logo_raw_content.substr(start, end - start);
        }
    }

    // Wrapper SVG
    string content = format(R"(
        <svg x="{0}" y="{1}" width="{2}" height="{3}" viewBox="{4}" preserveAspectRatio="xMidYMid meet" fill="blue">
            {5}
        </svg>
    )", img_x, img_y, img_w, img_h, logo_viewbox, logo_raw_content);

    // add text
    string text = "Digital Manufacturing";
    float text_y = height * 0.75f;
    
    content += format(R"(
        <text x="{0}" y="{1}" font-family="Arial, sans-serif" font-size="{2}" text-anchor="middle" fill="blue" stroke="none">
            {3}
        </text>
    )", width/2.0f, text_y, min(0.4f, width/15.0f), text);

    return content;
}


int main(int argc, char** argv) {
    float width, length, height;

    // ==========================================
    // User Inputs
    // ==========================================

    while (true) {
        cout << "What should the width of the box be? (in)\n";
        if (cin >> width && width >= 2 && width <= 12) break;
        cout << "Error: Invalid width (min 2in, max 12in).\n";
        cin.clear(); cin.ignore(1000, '\n');
    }
    while (true) {
        cout << "What should the length of the container be? (in)\n";
        if (cin >> length && length >= 2 && length <= 18) break;
        cout << "Error: Invalid length (min 2in, max 18in).\n";
        cin.clear(); cin.ignore(1000, '\n');
    }
    while (true) {
        cout << "What should the height of the box be? (in)\n";
        if (cin >> height && height >= 2 && height <= 12) break;
        cout << "Error: Invalid height (min 2in, max 12in).\n";
        cin.clear(); cin.ignore(1000, '\n');
    }
    height -= ACRYLIC_THICKNESS;

    const float available_space = length - ACRYLIC_THICKNESS*2;
    const float min_section_thickness = 0.5 + ACRYLIC_THICKNESS; //0.5 + dividerlen
    int num_sections;

    while (true) {
        cout << "How many sections do you want?\n";
        if (cin >> num_sections && num_sections <= available_space / min_section_thickness) break;
        cout << "Error: Too many sections for this container.\n";
        cin.clear(); cin.ignore(1000, '\n');
    }

    vector<float> section_lens;
    bool continue_prompt = true;
    float remaining_space = available_space;
    while (continue_prompt) {
        cout << "You will choose the length of each section now. It will add a 0.125in divider after each of your section's provided length. The minimum section length is 0.5 inches\n";
        continue_prompt = false;
        for (int i = 0; i < num_sections-1; i ++) {
            if (remaining_space <= 0.5 + ACRYLIC_THICKNESS) {
                cout << "Error: Not enough space for minimum section length.\n";
                continue_prompt = true;
                cin.clear(); cin.ignore(1000, '\n'); // Clear buffer on bad input
                remaining_space = available_space;
                section_lens.clear();
                break;
            }
            float curr_sec_len;
            while (true) {
                cout << format("What should the length of section {} be? (in) [Remaining Space: {}]\n", i+1, remaining_space);
                if (cin >> curr_sec_len && curr_sec_len < 0.5) { // the input is good
                    cout << "Error: Selected section length is too small.\n";
                    cin.clear(); cin.ignore(1000, '\n');
                    continue;
                }

                if (curr_sec_len && curr_sec_len + ACRYLIC_THICKNESS <= remaining_space) break; // the input is good
                cout << "Error: Selected section length is too large.\n";
                cin.clear(); cin.ignore(1000, '\n'); // Clear buffer on bad input
            }
            remaining_space -= curr_sec_len + ACRYLIC_THICKNESS;
            section_lens.push_back(curr_sec_len);
        }
        if (remaining_space < 0.5) {
            cout << "Error: Not enough space for minimum section length (last section).\n";
            continue_prompt = true;
            cin.clear(); cin.ignore(1000, '\n'); // Clear buffer on bad input
            remaining_space = available_space;
            section_lens.clear();
        }
    }
    section_lens.push_back(remaining_space);

    cout << "Successfully created section lengths of:\n";
    for (float& f : section_lens) {
        cout << f << "in ";
    }
    cout << "\n";

    char do_text;
    while (true) {
        cout << "Do you want text to be engraved on the base? (y/n)\n";
        if (cin >> do_text && (do_text == 'y' || do_text == 'n')) break;
        cout << "Error: Invalid input.\n";
        cin.clear(); cin.ignore(1000, '\n');
    }
    string base_text = "";
    if (do_text == 'y') {
        cin.ignore(1000, '\n'); 
        
        while (true) {
            cout << "What should the text be? (max 20 characters)\n";
            getline(cin, base_text);
            
            if (!base_text.empty() && base_text.length() <= 20) {
                break;
            }
            cout << "Error: Text must be between 1 and 20 characters.\n";
        }
    }

    while (true) {
        cout << "Do you want text to be engraved on the sides (length-side wall)? (y/n)\n";
        if (cin >> do_text && (do_text == 'y' || do_text == 'n')) break;
        cout << "Error: Invalid input.\n";
        cin.clear(); cin.ignore(1000, '\n');
    }
    string length_text = "";
    if (do_text == 'y') {
        cin.ignore(1000, '\n'); 
        
        while (true) {
            cout << "What should the text be? (max 20 characters)\n";
            getline(cin, length_text);
            
            if (!length_text.empty() && length_text.length() <= 20) {
                break;
            }
            cout << "Error: Text must be between 1 and 20 characters.\n";
        }
    }

    // ==========================================
    // GENERATE GEOMETRY
    // ==========================================

    // --- 1. BASE CONTENT ---
    string base_inner = "";
    base_inner += "<rect x=\"0\" y=\"0\" width=\"" + format("{}", width) + "\" height=\"" + format("{}", length) + "\" fill=\"none\" stroke=\"green\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" />";
    base_inner += generate_text(base_text, width/2.0f, length/2.0f, width, length, 90);
    
    // corners
    base_inner += screw_hole(HALF_ACRYLIC_THICKNESS, 0.15);
    base_inner += screw_hole(HALF_ACRYLIC_THICKNESS, length - 0.15);
    base_inner += screw_hole(width - HALF_ACRYLIC_THICKNESS, 0.15);
    base_inner += screw_hole(width - HALF_ACRYLIC_THICKNESS, length - 0.15);

    // side screws (X)
    int num_screws_w = width / 3;
    float interval_inc_w = width / (num_screws_w + 1.0);
    for (int i = 0; i < num_screws_w; i++) {
        base_inner += screw_hole(interval_inc_w * (i+1), HALF_ACRYLIC_THICKNESS);
        base_inner += screw_hole(interval_inc_w * (i+1), length - HALF_ACRYLIC_THICKNESS);
    }
    // side screws (Y)
    int num_screws_l = length / 3;
    float interval_inc_l = length / (num_screws_l + 1.0);
    for (int i = 0; i < num_screws_l; i++) {
        base_inner += screw_hole(HALF_ACRYLIC_THICKNESS, interval_inc_l * (i+1));
        base_inner += screw_hole(width - HALF_ACRYLIC_THICKNESS, interval_inc_l * (i+1));
    }

    // --- 2. LENGTH WALL CONTENT ---
    string l_wall_inner = "";
    l_wall_inner += "<rect x=\"0\" y=\"0\" width=\"" + format("{}", length) + "\" height=\"" + format("{}", height) + "\" fill=\"none\" stroke=\"green\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" />";
    l_wall_inner += generate_text(length_text, length/2.0f, height/2.0f, length, height, 0);
    l_wall_inner += t_slot(0.15, height, 0);
    l_wall_inner += t_slot(length - 0.15, height, 0);
    l_wall_inner += screw_hole(HALF_ACRYLIC_THICKNESS, height / 2);
    l_wall_inner += screw_hole(length - HALF_ACRYLIC_THICKNESS, height / 2);

    float offset = ACRYLIC_THICKNESS;
    for (int i = 0; i < num_sections-1; i ++) {
        l_wall_inner += screw_hole(offset + section_lens[i] + HALF_ACRYLIC_THICKNESS, height / 2);
        offset += section_lens[i] + ACRYLIC_THICKNESS;
    }
    for (int i = 0; i < num_screws_l; i++) {
        l_wall_inner += t_slot(interval_inc_l * (i+1), height, 0);
    }

    // --- 3a. WIDTH WALL FRONT CONTENT ---
    float w_wall_width = width - ACRYLIC_THICKNESS*2;
    string w_front_inner = "";
    w_front_inner += "<rect x=\"0\" y=\"0\" width=\"" + format("{}", w_wall_width) + "\" height=\"" + format("{}", height) + "\" fill=\"none\" stroke=\"green\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" />";
    w_front_inner += t_slot(0, height / 2, 90);
    w_front_inner += t_slot(w_wall_width, height / 2, 270);
    for (int i = 0; i < num_screws_w; i++) {
        w_front_inner += t_slot(interval_inc_w * (i+1) - ACRYLIC_THICKNESS, height, 0);
    }
    w_front_inner += generate_logo_face(w_wall_width, height);

    // --- 3b. WIDTH WALL BACK CONTENT ---
    string w_back_inner = "";
    w_back_inner += "<rect x=\"0\" y=\"0\" width=\"" + format("{}", w_wall_width) + "\" height=\"" + format("{}", height) + "\" fill=\"none\" stroke=\"green\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" />";
    w_back_inner += t_slot(0, height / 2, 90);
    w_back_inner += t_slot(w_wall_width, height / 2, 270);
    for (int i = 0; i < num_screws_w; i++) {
        w_back_inner += t_slot(interval_inc_w * (i+1) - ACRYLIC_THICKNESS, height, 0);
    }
    w_back_inner += generate_fractal_face(w_wall_width, height);

    // --- 4. DIVIDER WALL CONTENT ---
    string d_wall_inner = "";
    d_wall_inner += "<rect x=\"0\" y=\"0\" width=\"" + format("{}", w_wall_width) + "\" height=\"" + format("{}", height) + "\" fill=\"none\" stroke=\"green\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" />";
    d_wall_inner += t_slot(0, height / 2, 90);
    d_wall_inner += t_slot(w_wall_width, height / 2, 270);

    // ==========================================
    // OUTPUT INDIVIDUAL FILES
    // ==========================================
    auto write_svg = [](string name, float w, float h, string content) {
        ofstream f(name, ios::binary);
        f << format(R"(<?xml version="1.0" encoding="UTF-8" ?><svg xmlns="http://www.w3.org/2000/svg" width="{0}in" height="{1}in" viewBox="0 0 {0} {1}">{2}</svg>)", w, h, content);
        f.close();
    };

    write_svg("base.svg", width, length, base_inner);
    write_svg("l_wall.svg", length, height, l_wall_inner);
    write_svg("w_wall_front.svg", w_wall_width, height, w_front_inner);
    write_svg("w_wall_back.svg", w_wall_width, height, w_back_inner);
    write_svg("d_wall.svg", w_wall_width, height, d_wall_inner);

    // ==========================================
    // MASTER SVG LAYOUT (18x12 Sheet)
    // ==========================================
    ofstream master("master.svg", ios::binary);
    
    // header for master file (18x12 inches)
    master << R"(<?xml version="1.0" encoding="UTF-8" ?>
    <svg xmlns="http://www.w3.org/2000/svg" width="18in" height="12in" viewBox="0 0 18 12">
    <rect x="0" y="0" width="18" height="12" fill="none" stroke="red" stroke-width="0.001in" /> 
    )"; //cut bounds

    float cur_x = 0.2f; // margin
    float cur_y = 0.2f;
    float row_h = 0.0f;
    float gap = 0.25f;  // space between parts

    // Lambda to place a part
    // returns true if placed, false if it doesn't fit in the sheet
    auto place_part = [&](string content, float part_w, float part_h, bool rotate) {
        float effective_w = rotate ? part_h : part_w;
        float effective_h = rotate ? part_w : part_h;

        // check if we need a new row
        if (cur_x + effective_w > 17.8f) {
            cur_x = 0.2f;
            cur_y += row_h + gap;
            row_h = 0.0f;
        }

        // check if we are out of vertical space
        if (cur_y + effective_h > 11.8f) {
            cout << "WARNING: Part does not fit on 18x12 sheet!\n";
            return;
        }

        // determine Transform String
        string transform;
        if (rotate) {
            transform = format("translate({0}, {1}) rotate(90) translate(0, -{2})", cur_x, cur_y, part_h); 
            transform = format("translate({0}, {1}) rotate(90)", cur_x + effective_w, cur_y);
        } else {
            transform = format("translate({0}, {1})", cur_x, cur_y);
        }

        master << "<g transform=\"" << transform << "\">\n" << content << "\n</g>\n";

        // advance Cursor
        cur_x += effective_w + gap;
        row_h = max(row_h, effective_h);
    };

    // 1. PLACE BASE
    bool rotate_base = (width > 17.5f) || (width > length && length < 11.5f);
    place_part(base_inner, width, length, rotate_base);

    // 2. PLACE WALLS
    place_part(l_wall_inner, length, height, false);
    place_part(l_wall_inner, length, height, false);

    // 1x Front W-Wall
    place_part(w_front_inner, w_wall_width, height, false);

    // 1x Back W-Wall
    place_part(w_back_inner, w_wall_width, height, false);

    // N-1 Divider Walls
    for(int i=0; i < num_sections-1; i++) {
        place_part(d_wall_inner, w_wall_width, height, false);
    }

    master << "</svg>";
    master.close();

    cout << "Files generated, including master.svg (18x12 layout)\n";
    return 0;
}