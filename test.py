import math

# =======================================================================
# PRINT PARAMETERS 
# =======================================================================

# Print Dimensions
CENTER_X = 100.0
CENTER_Y = 100.0

# Z-Height Adjustments
SAFE_Z = 50.0              # High clearance height to avoid crashing into the plate
START_Z_BASE = 7.5         
LAYER_HEIGHT = 1.5         
LINE_WIDTH = 2.5
JAM_Z_OFFSET = 3 * LAYER_HEIGHT   

# Tart Geometry
TART_RADIUS = 25.4         # 50.8mm diameter (Exactly 2.0 inches)
AMP = 2.5                  # Depth of the scallops
PETALS = 8                 
NUM_WALL_LAYERS = 3        
NUM_JAM_LAYERS = 1         

# Material Settings
FEED_RATE_CRUST = 500      
FEED_RATE_JAM = 700          # blueberry, unchanged
FEED_RATE_CREAM_CHEESE = 200      
FEED_RATE_TRAVEL = 2000

E_MULT_SHELL = 0.04        
E_MULT_JAM = 0.035           # blueberry, unchanged
E_MULT_CREAM_CHEESE = 0.025       

# Globals used during generation
current_e = 0.0            
current_x = CENTER_X
current_y = CENTER_Y

# =======================================================================
# HELPER FUNCTIONS
# =======================================================================

def reset_extruder(gcode):
    global current_e
    current_e = 0.0
    gcode.append("G92 E0 ; Reset Extruder position to 0 for new syringe")

def add_path_to_gcode(gcode, path, e_multiplier, feed_rate):
    global current_e, current_x, current_y
    for pt in path:
        dist = math.hypot(pt[0] - current_x, pt[1] - current_y)
        current_e += dist * e_multiplier
        current_x, current_y = pt[0], pt[1]
        gcode.append(f"G1 X{current_x:.2f} Y{current_y:.2f} E{current_e:.3f} F{feed_rate}")

# =======================================================================
# FILE 1: THE CRUST (Peanut Butter)
# =======================================================================
def generate_file_1_crust():
    global current_x, current_y, current_e
    gcode = []
    
    gcode.append("; FILE 1: CRUST (Peanut Butter)")
    gcode.append("; Diameter: Exactly 2.0 inches")
    gcode.append("M106 S255 ; Fan on")
    gcode.append("G21 ; Millimeters")
    gcode.append("G90 ; Absolute positioning")
    gcode.append("M82 ; Absolute extrusion")
    gcode.append("G28 ; Home axes")
    reset_extruder(gcode)
    
    # --- SAFE TRAVEL TO START ---
    gcode.append(f"\n; Move to start safely above the plate")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Lift to Safe Z")
    gcode.append(f"G1 X{CENTER_X} Y{CENTER_Y} F{FEED_RATE_TRAVEL} ; Travel X/Y while high up")
    
    # 1a. Print Solid Base 
    z = START_Z_BASE
    gcode.append(f"\n; Crust Base Layer (Z={z:.2f})")
    gcode.append(f"G1 Z{z:.2f} F{FEED_RATE_TRAVEL} ; Plunge straight down to print height")
    current_x, current_y = CENTER_X, CENTER_Y
    
    base_path = []
    revolutions = max(1, int(TART_RADIUS / LINE_WIDTH))
    steps_per_rev = 40 
    total_steps = revolutions * steps_per_rev
    
    for i in range(total_steps + 1):
        progress = i / total_steps
        r_base = progress * TART_RADIUS
        amp_curr = progress * AMP
        theta = (i / steps_per_rev) * 2 * math.pi
        r = r_base + amp_curr * math.cos(PETALS * theta)
        base_path.append((CENTER_X + r*math.cos(theta), CENTER_Y + r*math.sin(theta)))
        
    add_path_to_gcode(gcode, base_path, E_MULT_SHELL, FEED_RATE_CRUST)

    # 1b. Print Scalloped Walls
    for layer in range(1, NUM_WALL_LAYERS + 1):
        z = START_Z_BASE + (layer * LAYER_HEIGHT)
        gcode.append(f"\n; Crust Wall Layer {layer} (Z={z:.2f})")
        gcode.append(f"G1 Z{z:.2f} F{FEED_RATE_TRAVEL}")
        
        wall_path = []
        steps = 50
        for i in range(steps + 1):
            theta = (i / steps) * (2 * math.pi)
            r = TART_RADIUS + AMP * math.cos(PETALS * theta)
            wall_path.append((CENTER_X + r * math.cos(theta), CENTER_Y + r * math.sin(theta)))
        add_path_to_gcode(gcode, wall_path, E_MULT_SHELL, FEED_RATE_CRUST)

    # End Sequence for File 1
    gcode.append("\n; --- END OF FILE 1 ---")
    current_e -= 3.0
    gcode.append(f"G1 E{current_e:.3f} F300 ; Retract to prevent drool")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Lift to Safe Z for syringe swap")
    gcode.append(f"G1 X10 Y10 F{FEED_RATE_TRAVEL} ; Park in front left corner")
    gcode.append("; NOTICE: NO M84 COMMAND. Motors stay engaged to hold position.")

    with open("1_Crust.gcode", "w") as f:
        f.write("\n".join(gcode))
    print("Generated 1_Crust.gcode")

# =======================================================================
# FILE 2: BLUEBERRY JAM (Left Side)
# =======================================================================
def generate_file_2_blueberry():
    global current_x, current_y, current_e
    gcode = []
    
    gcode.append("; FILE 2: BLUEBERRY JAM (LEFT)")
    gcode.append("G21 ; Millimeters")
    gcode.append("G90 ; Absolute positioning")
    gcode.append("M82 ; Absolute extrusion")
    gcode.append("; NOTICE: NO G28 COMMAND. We do not want to crash into the crust!")
    reset_extruder(gcode)
    
    FILL_START_RADIUS = TART_RADIUS - LINE_WIDTH
    num_fill_lines = max(1, int(FILL_START_RADIUS / LINE_WIDTH))
    
    left_fill_path = []
    for i in range(num_fill_lines + 1):
        r_base = FILL_START_RADIUS - (i * LINE_WIDTH)
        if r_base < 0: r_base = 0
        amp_curr = AMP * (r_base / FILL_START_RADIUS) if FILL_START_RADIUS > 0 else 0
        
        steps = 20
        if i % 2 == 0:
            angles = [math.pi/2 + (math.pi * j / steps) for j in range(steps + 1)]
        else:
            angles = [3*math.pi/2 - (math.pi * j / steps) for j in range(steps + 1)]
            
        for theta in angles:
            r = r_base + amp_curr * math.cos(PETALS * theta)
            x = CENTER_X + r * math.cos(theta)
            y = CENTER_Y + r * math.sin(theta)
            left_fill_path.append((x, y))
    
    # --- SAFE TRAVEL TO START ---
    fill_z = START_Z_BASE + JAM_Z_OFFSET
    gcode.append(f"\n; Move safely over the tart before plunging")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Ensure Safe Z")
    gcode.append(f"G1 X{left_fill_path[0][0]:.2f} Y{left_fill_path[0][1]:.2f} F{FEED_RATE_TRAVEL} ; Travel X/Y while high up")
    
    gcode.append(f"\n; Blueberry Layer (Z={fill_z:.2f})")
    gcode.append(f"G1 Z{fill_z:.2f} F{FEED_RATE_TRAVEL} ; Plunge straight down to print height")
    current_x, current_y = left_fill_path[0][0], left_fill_path[0][1]
    
    add_path_to_gcode(gcode, left_fill_path, E_MULT_JAM, FEED_RATE_JAM)

    gcode.append("\n; --- END OF FILE 2 ---")
    current_e -= 3.0
    gcode.append(f"G1 E{current_e:.3f} F300 ; Retract")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Lift to Safe Z")
    gcode.append(f"G1 X10 Y10 F{FEED_RATE_TRAVEL} ; Park")

    with open("2_Blueberry.gcode", "w") as f:
        f.write("\n".join(gcode))
    print("Generated 2_Blueberry.gcode")

# =======================================================================
# FILE 3: STRAWBERRY JAM (Right Side)
# =======================================================================
def generate_file_3_strawberry():
    global current_x, current_y, current_e
    gcode = []
    
    gcode.append("; FILE 3: CREAM CHEESE (RIGHT)")
    gcode.append("G21 ; Millimeters")
    gcode.append("G90 ; Absolute positioning")
    gcode.append("M82 ; Absolute extrusion")
    gcode.append("; NOTICE: NO G28 COMMAND.")
    reset_extruder(gcode)
    
    FILL_START_RADIUS = TART_RADIUS - LINE_WIDTH
    num_fill_lines = max(1, int(FILL_START_RADIUS / LINE_WIDTH))
    
    right_fill_path = []
    for i in range(num_fill_lines + 1):
        r_base = FILL_START_RADIUS - (i * LINE_WIDTH)
        if r_base < 0: r_base = 0
        amp_curr = AMP * (r_base / FILL_START_RADIUS) if FILL_START_RADIUS > 0 else 0
        
        steps = 20
        if i % 2 == 0:
            angles = [-math.pi/2 + (math.pi * j / steps) for j in range(steps + 1)]
        else:
            angles = [math.pi/2 - (math.pi * j / steps) for j in range(steps + 1)]
            
        for theta in angles:
            r = r_base + amp_curr * math.cos(PETALS * theta)
            x = CENTER_X + r * math.cos(theta)
            y = CENTER_Y + r * math.sin(theta)
            right_fill_path.append((x, y))
            
    # --- SAFE TRAVEL TO START ---
    fill_z = START_Z_BASE + JAM_Z_OFFSET
    gcode.append(f"\n; Move safely over the tart before plunging")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Ensure Safe Z")
    gcode.append(f"G1 X{right_fill_path[0][0]:.2f} Y{right_fill_path[0][1]:.2f} F{FEED_RATE_TRAVEL} ; Travel X/Y while high up")

    gcode.append(f"\n; Strawberry Layer (Z={fill_z:.2f})")
    gcode.append(f"G1 Z{fill_z:.2f} F{FEED_RATE_TRAVEL} ; Plunge straight down to print height")
    current_x, current_y = right_fill_path[0][0], right_fill_path[0][1]

    add_path_to_gcode(gcode, right_fill_path, E_MULT_CREAM_CHEESE, FEED_RATE_CREAM_CHEESE)

    gcode.append("\n; --- END OF FILE 3 (FINAL) ---")
    current_e -= 3.0
    gcode.append(f"G1 E{current_e:.3f} F300 ; Retract")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Lift to Safe Z")
    gcode.append(f"G1 X0 Y0 F{FEED_RATE_TRAVEL} ; Park at home X/Y")
    gcode.append("M84 ; Disable motors (PRINT COMPLETE)")
    gcode.append("M106 S0 ; Fan off")

    with open("3_CreamCheese.gcode", "w") as f:
        f.write("\n".join(gcode))
    print("Generated 3_Strawberry.gcode")

# =======================================================================
# RUN EVERYTHING
# =======================================================================
if __name__ == "__main__":
    generate_file_1_crust()
    generate_file_2_blueberry()
    generate_file_3_strawberry()
    print("All 3 G-code files successfully generated with Safe Z plunging.")