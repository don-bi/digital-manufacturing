import math

# =======================================================================
# PRINT PARAMETERS - TWISTED TRIANGLE (Nutella)
# =======================================================================

CENTER_X = 100.0
CENTER_Y = 100.0

SAFE_Z        = 50.0
START_Z       = 6.0
LAYER_HEIGHT  = 2.0
LINE_WIDTH    = 2.5

TRI_RADIUS_START   = 25.4
TRI_RADIUS_MIN     = 10.0
SHRINK_PER_LAYER   = 2.5
ROTATION_PER_LAYER = 3.0   # FIXED: was 15, too much to maintain layer adhesion

FEED_RATE_NUTELLA  = 450
FEED_RATE_TRAVEL   = 2000
E_MULT_NUTELLA     = 0.045

current_e = 0.0
current_x = CENTER_X
current_y = CENTER_Y

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

def move_xy(gcode, x, y):
    """Retract, travel XY at current Z, prime. No Z change."""
    global current_x, current_y
    retract(gcode)
    gcode.append(f"G1 X{x:.2f} Y{y:.2f} F{FEED_RATE_TRAVEL} ; XY travel")
    current_x, current_y = x, y
    unretract(gcode)

def add_path(gcode, path, e_mult, feed_rate):
    global current_e, current_x, current_y
    for pt in path:
        dist = math.hypot(pt[0] - current_x, pt[1] - current_y)
        current_e += dist * e_mult
        current_x, current_y = pt[0], pt[1]
        gcode.append(f"G1 X{current_x:.2f} Y{current_y:.2f} E{current_e:.3f} F{feed_rate}")

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

def generate_twisted_triangle():
    global current_x, current_y, current_e
    gcode = []

    gcode.append("; FILE: TWISTED TRIANGLE (Nutella)")
    gcode.append("M106 S255 ; Fan on")
    gcode.append("G21 ; Millimeters")
    gcode.append("G90 ; Absolute positioning")
    gcode.append("M82 ; Absolute extrusion")
    gcode.append("G28 ; Home axes")
    reset_extruder(gcode)

    # Move to first layer start position
    radius   = TRI_RADIUS_START
    rotation = 0.0
    layer    = 0
    first_pts = make_triangle(CENTER_X, CENTER_Y, radius, rotation)

    gcode.append(f"\n; Travel to start position")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Safe Z for initial move only")
    gcode.append(f"G1 X{first_pts[0][0]:.2f} Y{first_pts[0][1]:.2f} F{FEED_RATE_TRAVEL}")
    gcode.append(f"G1 Z{START_Z:.2f} F{FEED_RATE_TRAVEL} ; Descend to first layer")
    current_x, current_y = first_pts[0]

    while radius >= TRI_RADIUS_MIN:
        z = START_Z + layer * LAYER_HEIGHT

        outer_pts = make_triangle(CENTER_X, CENTER_Y, radius,              rotation)
        inner_pts = make_triangle(CENTER_X, CENTER_Y, radius - LINE_WIDTH, rotation)

        gcode.append(f"\n; === Layer {layer + 1} | Z={z:.2f}mm | "
                     f"R={radius:.1f}mm | Rot={rotation:.1f}deg ===")

        # Set Z for this layer
        gcode.append(f"G1 Z{z:.2f} F{FEED_RATE_TRAVEL} ; Raise to layer height")

        # Outer triangle: move XY to start then print
        gcode.append("; Outer triangle")
        move_xy(gcode, outer_pts[0][0], outer_pts[0][1])
        add_path(gcode, outer_pts, E_MULT_NUTELLA, FEED_RATE_NUTELLA)

        # Inner triangle: move XY to start then print
        gcode.append("; Inner triangle")
        move_xy(gcode, inner_pts[0][0], inner_pts[0][1])
        add_path(gcode, inner_pts, E_MULT_NUTELLA, FEED_RATE_NUTELLA)

        radius   -= SHRINK_PER_LAYER
        rotation += ROTATION_PER_LAYER
        layer    += 1

    # End
    gcode.append("\n; --- END ---")
    current_e -= 3.0
    gcode.append(f"G1 E{current_e:.3f} F300 ; Final retract")
    gcode.append(f"G1 Z{SAFE_Z:.2f} F{FEED_RATE_TRAVEL} ; Lift")
    gcode.append(f"G1 X0 Y0 F{FEED_RATE_TRAVEL} ; Park")
    gcode.append("M84 ; Disable motors")
    gcode.append("M106 S0 ; Fan off")

    with open("4_TwistedTriangle.gcode", "w") as f:
        f.write("\n".join(gcode))
    print(f"Generated 4_TwistedTriangle.gcode ({layer} layers)")

if __name__ == "__main__":
    generate_twisted_triangle()