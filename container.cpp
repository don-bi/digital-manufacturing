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
const float SCREW_HEAD_WIDTH = 0.167;
const float PI = 3.1415926535f;

// --- Helper Functions ---

string screw_hole(float x, float y, float radius = SCREW_RADIUS) {
    return format(R"(<circle cx="{0}" cy="{1}" r="{2}" fill="none" stroke="white" stroke-width="1px" vector-effect="non-scaling-stroke"/>)", x, y, radius);
}

std::string t_slot(float x, float y, float angle_deg) {
    const float screw_r = SCREW_RADIUS; 
    const float nut_r = 0.1f; 
    
    const float y_top_screw = -0.26f;
    const float y_top_nut   = -0.16f;
    const float y_bot_nut   = -0.09f;
    const float y_bot_screw = 0.0f;
    
    string path_data = format(
        "M {0} {2} L {1} {2} L {1} {3} L {4} {3} L {4} {5} L {1} {5} L {1} {6} L {0} {6} L {0} {5} L {7} {5} L {7} {3} L {0} {3} Z",
        x - screw_r, x + screw_r, y + y_top_screw, y + y_top_nut, 
        x + nut_r, y + y_bot_nut, y + y_bot_screw, x - nut_r
    );

    return format("<g transform=\"rotate({2} {0} {1})\"><path d=\"{3}\" fill=\"none\" stroke=\"white\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" /></g>", x, y, angle_deg, path_data);
}

string generate_text(string text, float x, float y, float container_w, float container_h, float rotation_deg) {
    if (text.empty()) return "";
    bool is_vertical = (static_cast<int>(abs(rotation_deg)) % 180 == 90);
    float space_for_text_length = is_vertical ? container_h : container_w;
    float space_for_text_height = is_vertical ? container_w : container_h;
    float usable_length = space_for_text_length * 0.8f;
    float usable_height = space_for_text_height * 0.6f;
    float char_aspect = 0.6f; 
    float size_by_len = usable_length / (text.length() * char_aspect);
    float size_by_height = usable_height;
    float font_size = std::min(size_by_len, size_by_height);
    font_size = std::min(font_size, 1.25f); 
    font_size = std::max(font_size, 0.15f);

    return format(R"SVG(<text x="{0}" y="{1}" transform="rotate({4} {0} {1})" font-family="Arial, sans-serif" font-size="{2}" text-anchor="middle" dominant-baseline="middle" fill="blue" stroke="none">{3}</text>)SVG", x, y, font_size, text, rotation_deg);
}

void recursive_tree(float x, float y, float length, float angle_deg, int depth, string& svg_content) {
    if (depth == 0) return;
    float angle_rad = angle_deg * (PI / 180.0f);
    float x2 = x + length * cos(angle_rad);
    float y2 = y + length * sin(angle_rad);
    svg_content += format(R"(<line x1="{}" y1="{}" x2="{}" y2="{}" stroke="blue" stroke-width="0.02" />)", x, y, x2, y2);
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
    if (!infile.is_open()) return "";
    stringstream buffer;
    buffer << infile.rdbuf();
    string content = buffer.str();
    
    size_t xml_pos = content.find("<?xml");
    if (xml_pos != string::npos) content.erase(xml_pos, content.find("?>", xml_pos) - xml_pos + 2);
    size_t doc_pos = content.find("<!DOCTYPE");
    if (doc_pos != string::npos) content.erase(doc_pos, content.find(">", doc_pos) - doc_pos + 1);
    return content;
}

void replace_all(string& str, const string& from, const string& to) {
    if(from.empty()) return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

string generate_logo_face(float width, float height) {
    float target_w = width * 0.8f;
    float target_h = height * 0.5f;
    float dest_x = (width - target_w) / 2.0f;
    float dest_y = height * 0.15f;

    string raw_content = read_svg_file("columbia_engineering.svg");
    if (raw_content.empty()) return "";

    float vb_min_x = 0.0f, vb_min_y = 0.0f, vb_w = 100.0f, vb_h = 100.0f;
    size_t vb_pos = raw_content.find("viewBox=\"");
    if (vb_pos != string::npos) {
        size_t start = vb_pos + 9;
        size_t end = raw_content.find("\"", start);
        string vb_str = raw_content.substr(start, end - start);
        replace(vb_str.begin(), vb_str.end(), ',', ' ');
        stringstream ss(vb_str);
        ss >> vb_min_x >> vb_min_y >> vb_w >> vb_h;
    }

    float scale = std::min(target_w / vb_w, target_h / vb_h);
    float final_obj_w = vb_w * scale;
    float final_obj_h = vb_h * scale;
    float center_offset_x = (target_w - final_obj_w) / 2.0f;
    float center_offset_y = (target_h - final_obj_h) / 2.0f;

    size_t svg_start = raw_content.find("<svg");
    size_t content_start = (svg_start != string::npos) ? raw_content.find(">", svg_start) + 1 : 0;
    size_t content_end = raw_content.rfind("</svg>");
    string inner_svg = raw_content.substr(content_start, content_end - content_start);

    replace_all(inner_svg, "fill=\"#000000\"", "fill=\"blue\"");
    replace_all(inner_svg, "fill=\"black\"",    "fill=\"blue\"");
    replace_all(inner_svg, "stroke=\"#000000\"", "stroke=\"blue\"");
    replace_all(inner_svg, "stroke=\"black\"",   "stroke=\"blue\"");

    string content = format(R"SVG(<g transform="translate({0} {1}) scale({2}) translate({3} {4})">{5}</g>)SVG", 
        dest_x + center_offset_x, dest_y + center_offset_y, scale, -vb_min_x, -vb_min_y, inner_svg);

    string text = "Digital Manufacturing";
    content += format(R"(<text x="{0}" y="{1}" font-family="Arial, sans-serif" font-size="{2}" text-anchor="middle" fill="blue" stroke="none">{3}</text>)", 
        width/2.0f, height * 0.80f, min(0.4f, width/15.0f), text);

    return content;
}

struct Part {
    string name;
    string content;
    float w;
    float h;
    bool rotate;
};

int main(int argc, char** argv) {
    float width, length, height;

    while (true) {
        cout << "What should the width of the box be? (in)\n";
        if (cin >> width && width >= 2 && width <= 12) break;
        cout << "Error: Invalid width (min 2in, max 12in).\n"; cin.clear(); cin.ignore(1000, '\n');
    }
    while (true) {
        cout << "What should the length of the container be? (in)\n";
        if (cin >> length && length >= 2 && length <= 18) break;
        cout << "Error: Invalid length (min 2in, max 18in).\n"; cin.clear(); cin.ignore(1000, '\n');
    }
    while (true) {
        cout << "What should the height of the box be? (in)\n";
        if (cin >> height && height >= 2 && height <= 12) break;
        cout << "Error: Invalid height (min 2in, max 12in).\n"; cin.clear(); cin.ignore(1000, '\n');
    }
    height -= ACRYLIC_THICKNESS;

    const float available_space = length - ACRYLIC_THICKNESS*2;
    const float min_section_thickness = 0.5 + ACRYLIC_THICKNESS;
    int num_sections;

    while (true) {
        cout << "How many sections do you want?\n";
        if (cin >> num_sections && num_sections <= available_space / min_section_thickness) break;
        cout << "Error: Too many sections for this container.\n"; cin.clear(); cin.ignore(1000, '\n');
    }

    vector<float> section_lens;
    float remaining_space = available_space;
    bool continue_prompt = true;
    while(continue_prompt) {
        continue_prompt = false;
        cout << "Minimum section length is 0.5 inches.\n";
        for (int i = 0; i < num_sections-1; i ++) {
            if (remaining_space <= 0.5 + ACRYLIC_THICKNESS) {
                cout << "Error: Not enough space left. Resetting sections.\n";
                remaining_space = available_space; section_lens.clear(); continue_prompt = true; break;
            }
            float curr_sec_len;
            while (true) {
                cout << format("Length of section {} (in) [Remaining: {}]: ", i+1, remaining_space);
                if (cin >> curr_sec_len && curr_sec_len >= 0.5 && curr_sec_len + ACRYLIC_THICKNESS <= remaining_space) break;
                cout << "Invalid length.\n"; cin.clear(); cin.ignore(1000, '\n');
            }
            remaining_space -= (curr_sec_len + ACRYLIC_THICKNESS);
            section_lens.push_back(curr_sec_len);
        }
        if (!continue_prompt && remaining_space < 0.5) {
             cout << "Error: Last section too small. Resetting.\n";
             remaining_space = available_space; section_lens.clear(); continue_prompt = true;
        }
    }
    section_lens.push_back(remaining_space);
    cout << "Sections confirmed.\n";

    char do_lid, do_text_base, do_text_side;
    while (true) {
        cout << "Do you want a lid? (y/n)\n";
        if (cin >> do_lid && (do_lid == 'y' || do_lid == 'n')) break;
        cout << "Error: Invalid input.\n";
        cin.clear(); cin.ignore(1000, '\n');
    }

    while (true) {
        cout << "Do you want text to be engraved on the base? (y/n)\n";
        if (cin >> do_text_base && (do_text_base == 'y' || do_text_base == 'n')) break;
        cout << "Error: Invalid input.\n";
        cin.clear(); cin.ignore(1000, '\n');
    }
    string base_text = "";
    if (do_text_base == 'y') {
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
        if (cin >> do_text_side && (do_text_side == 'y' || do_text_side == 'n')) break;
        cout << "Error: Invalid input.\n";
        cin.clear(); cin.ignore(1000, '\n');
    }
    string length_text = "";
    if (do_text_side == 'y') {
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

    // 1. Base
    string base_inner = format("<rect x=\"0\" y=\"0\" width=\"{}\" height=\"{}\" fill=\"none\" stroke=\"green\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" />", width, length);
    base_inner += generate_text(base_text, width/2.0f, length/2.0f, width, length, 90);
    base_inner += screw_hole(HALF_ACRYLIC_THICKNESS, ACRYLIC_THICKNESS + 0.12);
    base_inner += screw_hole(HALF_ACRYLIC_THICKNESS, length - (ACRYLIC_THICKNESS + 0.12));
    base_inner += screw_hole(width - HALF_ACRYLIC_THICKNESS, ACRYLIC_THICKNESS + 0.12);
    base_inner += screw_hole(width - HALF_ACRYLIC_THICKNESS, length - (ACRYLIC_THICKNESS + 0.12));

    int num_screws_w = width / 3;
    float interval_inc_w = width / (num_screws_w + 1.0);
    for (int i = 0; i < num_screws_w; i++) {
        base_inner += screw_hole(interval_inc_w * (i+1), HALF_ACRYLIC_THICKNESS);
        base_inner += screw_hole(interval_inc_w * (i+1), length - HALF_ACRYLIC_THICKNESS);
    }
    int num_screws_l = length / 3;
    float interval_inc_l = length / (num_screws_l + 1.0);
    for (int i = 0; i < num_screws_l; i++) {
        base_inner += screw_hole(HALF_ACRYLIC_THICKNESS, interval_inc_l * (i+1));
        base_inner += screw_hole(width - HALF_ACRYLIC_THICKNESS, interval_inc_l * (i+1));
    }

    // 2. Length Wall
    string l_wall_inner = format("<rect x=\"0\" y=\"0\" width=\"{}\" height=\"{}\" fill=\"none\" stroke=\"green\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" />", length, height);
    l_wall_inner += generate_text(length_text, length/2.0f, height/2.0f, length, height, 0);
    l_wall_inner += t_slot(ACRYLIC_THICKNESS + 0.12, height, 0);
    l_wall_inner += t_slot(length - (ACRYLIC_THICKNESS + 0.12), height, 0);
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

    // 3. Width Walls
    float w_wall_width = width - ACRYLIC_THICKNESS*2;
    float lid_height = w_wall_width;
    float lid_length = length;

    string w_front_inner = format("<rect x=\"0\" y=\"0\" width=\"{}\" height=\"{}\" fill=\"none\" stroke=\"green\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" />", w_wall_width, height);
    w_front_inner += t_slot(0, height / 2, 90);
    w_front_inner += t_slot(w_wall_width, height / 2, 270);
    for (int i = 0; i < num_screws_w; i++) {
        w_front_inner += t_slot(interval_inc_w * (i+1) - ACRYLIC_THICKNESS, height, 0);
    }
    w_front_inner += generate_logo_face(w_wall_width, height);
    if (do_lid == 'y') {
        w_front_inner += format(R"(<rect x="{0}" y="0.1" width="{0}" height="{1}" fill="none" stroke="green" stroke-width="1px" vector-effect="non-scaling-stroke" />)", lid_height/3, ACRYLIC_THICKNESS+0.05);
    }

    string w_back_inner = format("<rect x=\"0\" y=\"0\" width=\"{}\" height=\"{}\" fill=\"none\" stroke=\"green\" stroke-width=\"1px\" vector-effect=\"non-scaling-stroke\" />", w_wall_width, height);
    w_back_inner += t_slot(0, height / 2, 90);
    w_back_inner += t_slot(w_wall_width, height / 2, 270);
    for (int i = 0; i < num_screws_w; i++) {
        w_back_inner += t_slot(interval_inc_w * (i+1) - ACRYLIC_THICKNESS, height, 0);
    }
    w_back_inner += generate_fractal_face(w_wall_width, height);

    // 4. Divider
    string d_wall_inner = format(R"(<polygon points="0,0 {0},0 {0},{1} {2},{1} {2},{3} 0.0375,{3}, 0.0375,{1} 0,{1}" fill="none" stroke="green" stroke-width="1px" vector-effect="non-scaling-stroke" />)", 
        w_wall_width, height-0.26f, w_wall_width-0.0375, height);
    d_wall_inner += t_slot(0, height / 2, 90);
    d_wall_inner += t_slot(w_wall_width, height / 2, 270);
    if (do_lid == 'y') {

        d_wall_inner += format(R"(<rect x="0" y="0" width="{0}" height="{1}" fill="none" stroke="green" stroke-width="1px" vector-effect="non-scaling-stroke" />)", lid_height, 0.1 + ACRYLIC_THICKNESS);
    }

    // 5. Lid (UPDATED)
    string lid_inner = "";
    if (do_lid == 'y') {
        lid_inner += format(R"(<rect x="0" y="0" width="{0}" height="{1}" fill="none" stroke="green" stroke-width="1px" vector-effect="non-scaling-stroke" />)", lid_length, lid_height);
        
        // Locking Tabs (Front)
        lid_inner += format(R"(<rect x="{0}" y="0" width="{1}" height="{2}" fill="none" stroke="green" stroke-width="1px" vector-effect="non-scaling-stroke" />)", 
            lid_length-ACRYLIC_THICKNESS*2, ACRYLIC_THICKNESS*2, lid_height/3);
        lid_inner += format(R"(<rect x="{0}" y="{1}" width="{2}" height="{3}" fill="none" stroke="green" stroke-width="1px" vector-effect="non-scaling-stroke" />)", 
            lid_length-ACRYLIC_THICKNESS*2, lid_height - lid_height/3, ACRYLIC_THICKNESS*2, lid_height/3);
            
        lid_inner += format(R"(<circle cx="{0}" cy="{1}" r="0.35" fill="none" stroke="green" stroke-width="1px" vector-effect="non-scaling-stroke" />)", 
            0.6, lid_height/2.0f);
    }

    // ==========================================
    // OUTPUT FILES
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
    if (do_lid == 'y') write_svg("lid.svg", lid_length, lid_height, lid_inner);

    // ==========================================
    // MASTER LAYOUT
    // ==========================================
    vector<Part> parts;
    parts.push_back({"Base", base_inner, width, length, false});
    parts.push_back({"LWall1", l_wall_inner, length, height, false});
    parts.push_back({"LWall2", l_wall_inner, length, height, false});
    parts.push_back({"WWallFront", w_front_inner, w_wall_width, height, false});
    parts.push_back({"WWallBack", w_back_inner, w_wall_width, height, false});
    for(int i=0; i<num_sections-1; i++) parts.push_back({"DWall", d_wall_inner, w_wall_width, height, false});
    if (do_lid == 'y') parts.push_back({"Lid", lid_inner, lid_length, lid_height, false});

    // Horizontal orientation + Sort by Height (Bin Packing)
    for(auto& p : parts) {
        if (p.h > p.w && p.h <= 18.0f) { p.rotate = true; swap(p.w, p.h); }
        else if (p.w > 18.0f && p.h <= 18.0f) { p.rotate = true; swap(p.w, p.h); }
    }
    sort(parts.begin(), parts.end(), [](const Part& a, const Part& b) { return a.h > b.h; });

    ofstream master("master.svg", ios::binary);
    master << R"(<?xml version="1.0" encoding="UTF-8" ?>
    <svg xmlns="http://www.w3.org/2000/svg" width="18in" height="12in" viewBox="0 0 18 12">
    <rect x="0" y="0" width="18" height="12" fill="none" stroke="red" stroke-width="0.001in" /> 
    )";

    float cur_x = 0.2f, cur_y = 0.2f, row_h = 0.0f, gap = 0.25f;
    for (const auto& p : parts) {
        if (cur_x + p.w > 17.8f) { cur_x = 0.2f; cur_y += row_h + gap; row_h = 0.0f; }
        if (p.h > row_h) row_h = p.h;
        if (cur_y + p.h > 11.8f) { cout << "WARNING: Part " << p.name << " did not fit!\n"; continue; }
        
        string transform = p.rotate ? format("translate({0}, {1}) rotate(90)", cur_x + p.w, cur_y) : format("translate({0}, {1})", cur_x, cur_y);
        master << "<g transform=\"" << transform << "\">\n" << p.content << "\n</g>\n";
        cur_x += p.w + gap;
    }
    master << "</svg>";
    master.close();

    cout << "Files generated successfully. Check master.svg for the full layout.\n";
    return 0;
}