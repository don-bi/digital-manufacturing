import math

# =======================================================================
# PRINT PARAMETERS - TWISTED TRIANGLE (Dual Syringe)
# =======================================================================

CENTER_X = 100.0
CENTER_Y = 100.0

SAFE_Z        = 50.0
START_Z       = 6.0
LAYER_HEIGHT  = 2.3
LINE_WIDTH    = 2.5

# Dual syringe offset
# Measure the physical X distance between your two syringe nozzles.
# Syringe 2 is to the RIGHT, so we subtract this from X coords when using it
# so both syringes print at the same physical location.
SYRINGE_2_OFFSET_X = 24.0   # <-- MEASURE AND SET THIS

# Triangle Geometry
TRI_RADIUS_START   = 25.4
TRI_RADIUS_MIN     = 10.0
SHRINK_PER_LAYER   = 2.5
ROTATION_PER_LAYER = 3.0

# Material 1: Nutella (odd layers)
FEED_RATE_1 = 350
E_MULT_1    = 0.055

# Material 2: Your second material (even layers)
# Adjust these to match your second material's viscosity
FEED_RATE_2 = 350
E_MULT_2    = 0.055

FEED_RATE_TRAVEL = 2000

# Globals
current_e      = 0.0
current_x      = CENTER_X
current_y      = CENTER_Y
current_syringe = 1   # 1 or 2

# =======================================================================
# HELPERS
# =======================================================================

def reset_extruder(gcode):
    global current_e
    current_e = 0.0
    gcode.append("G92 E0 ; Reset extruder")

def retract(gcode):
    global current_e
    current_e -= 2.0
    gcode.append(f"G1 E{current_e:.3f} F300 ; Retract")

def unretract(gcode):
    global current_e
    current_e += 2.0
    gcode.append(f"G1 E{current_e:.3f} F300 ; Prime")

def apply_offset(x):
    """Shift X coords left when using syringe 2 so it prints in the same spot."""
    if current_syringe == 2:
        return x - SYRINGE_2_OFFSET_X
    return x

def move_xy(gcode, x, y):
    """Retract, travel XY at current Z with offset applied, prime."""
    global current_x, current_y
    retract(gcode)
    ox = apply_offset(x)
    gcode.append(f"G1 X{ox:.2f} Y{y:.2f} F{FEED_RATE_TRAVEL} ; XY travel")
    current_x, current_y = ox, y
    unretract(gcode)

def add_path(gcode, path, e_mult, feed_rate):
    global current_e, current_x, current_y
    for pt in path:
        ox = apply_offset(pt[0])
        dist = math.hypot(ox - current_x, pt[1] - current_y)
        current_e += dist * e_mult
        current_x, current_y = ox, pt[1]
        gcode.append(f"G1 X{current_x:.2f} Y{current_y:.2f} E{current_e:.3f} F{feed_rate}")

def switch_syringe(gcode, to_syringe, z):
    """
    Park safely, retract, reset extruder, switch to new syringe.
    The operator swaps the active syringe manually if needed,
    or the machine drives the second head into position via offset.
    """
    global current_syringe
    retract(gcode)
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Lift before syringe swap")
    gcode.append(f"G1 X10 Y10 F{FEED_RATE_TRAVEL} ; Park away from print")
    gcode.append(f"; === SWITCH TO SYRINGE {to_syringe} ===")
    current_syringe = to_syringe
    reset_extruder(gcode)
    gcode.append(f"G1 Z{z:.2f} F{FEED_RATE_TRAVEL} ; Return to layer height")

def make_triangle(cx, cy, radius, rotation_deg):
    pts = []
    for i in range(4):
        angle = math.radians(rotation_deg + i * 120.0)
        pts.append((cx + radius * math.cos(angle),
                    cy + radius * math.sin(angle)))
    return pts

# =======================================================================
# MAIN GENERATOR
# =======================================================================

def generate_twisted_triangle_dual():
    global current_x, current_y, current_e, current_syringe
    gcode = []

    gcode.append("; FILE: TWISTED TRIANGLE - DUAL SYRINGE")
    gcode.append("; Alternates between syringe 1 and 2 every layer")
    gcode.append(f"; Syringe 2 X offset: -{SYRINGE_2_OFFSET_X}mm applied to all X coords")
    gcode.append("M106 S255 ; Fan on")
    gcode.append("G21 ; Millimeters")
    gcode.append("G90 ; Absolute positioning")
    gcode.append("M82 ; Absolute extrusion")
    gcode.append("G28 ; Home axes")
    reset_extruder(gcode)

    radius   = TRI_RADIUS_START
    rotation = 0.0
    layer    = 0
    current_syringe = 1

    first_pts = make_triangle(CENTER_X, CENTER_Y, radius, rotation)
    gcode.append(f"\n; Initial travel to start")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Safe Z for initial move only")
    gcode.append(f"G1 X{apply_offset(first_pts[0][0]):.2f} Y{first_pts[0][1]:.2f} F{FEED_RATE_TRAVEL}")
    gcode.append(f"G1 Z{START_Z:.2f} F{FEED_RATE_TRAVEL} ; Descend to first layer")
    current_x, current_y = apply_offset(first_pts[0][0]), first_pts[0][1]

    while radius >= TRI_RADIUS_MIN:
        z = START_Z + layer * LAYER_HEIGHT

        # Alternate syringe every layer
        target_syringe = 1 if layer % 2 == 0 else 2
        if target_syringe != current_syringe:
            gcode.append(f"\n; --- Syringe swap before layer {layer + 1} ---")
            switch_syringe(gcode, target_syringe, z)

        feed  = FEED_RATE_1 if current_syringe == 1 else FEED_RATE_2
        emult = E_MULT_1    if current_syringe == 1 else E_MULT_2

        outer_pts = make_triangle(CENTER_X, CENTER_Y, radius,              rotation)
        inner_pts = make_triangle(CENTER_X, CENTER_Y, radius - LINE_WIDTH, rotation)

        gcode.append(f"\n; === Layer {layer + 1} | Syringe {current_syringe} | "
                     f"Z={z:.2f}mm | R={radius:.1f}mm | Rot={rotation:.1f}deg ===")

        gcode.append(f"G1 Z{z:.2f} F{FEED_RATE_TRAVEL} ; Layer height")

        gcode.append("; Outer triangle")
        move_xy(gcode, outer_pts[0][0], outer_pts[0][1])
        add_path(gcode, outer_pts, emult, feed)

        gcode.append("; Inner triangle")
        move_xy(gcode, inner_pts[0][0], inner_pts[0][1])
        add_path(gcode, inner_pts, emult, feed)

        radius   -= SHRINK_PER_LAYER
        rotation += ROTATION_PER_LAYER
        layer    += 1

    # End
    gcode.append("\n; --- END ---")
    current_e -= 3.0
    gcode.append(f"G1 E{current_e:.3f} F300 ; Final retract")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Lift")
    gcode.append(f"G1 X0 Y0 F{FEED_RATE_TRAVEL} ; Park at home")
    gcode.append("M84 ; Disable motors")
    gcode.append("M106 S0 ; Fan off")

    with open("5_TwistedTriangle_Dual.gcode", "w") as f:
        f.write("\n".join(gcode))
    print(f"Generated 5_TwistedTriangle_Dual.gcode ({layer} layers)")

if __name__ == "__main__":
    generate_twisted_triangle_dual()