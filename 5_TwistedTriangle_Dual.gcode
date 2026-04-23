; FILE: TWISTED TRIANGLE - DUAL SYRINGE
; Alternates between syringe 1 and 2 every layer
; Syringe 2 X offset: -24.0mm applied to all X coords
M106 S255 ; Fan on
G21 ; Millimeters
G90 ; Absolute positioning
M82 ; Absolute extrusion
G28 ; Home axes
G92 E0 ; Reset extruder

; Initial travel to start
G1 Z50.00 F2000 ; Safe Z for initial move only
G1 X125.40 Y100.00 F2000
G1 Z6.00 F2000 ; Descend to first layer

; === Layer 1 | Syringe 1 | Z=6.00mm | R=25.4mm | Rot=0.0deg ===
G1 Z6.00 F2000 ; Layer height
; Outer triangle
G1 E-2.000 F300 ; Retract
G1 X125.40 Y100.00 F2000 ; XY travel
G1 E0.000 F300 ; Prime
G1 X125.40 Y100.00 E0.000 F350
G1 X87.30 Y122.00 E2.420 F350
G1 X87.30 Y78.00 E4.839 F350
G1 X125.40 Y100.00 E7.259 F350
; Inner triangle
G1 E5.259 F300 ; Retract
G1 X122.90 Y100.00 F2000 ; XY travel
G1 E7.259 F300 ; Prime
G1 X122.90 Y100.00 E7.259 F350
G1 X88.55 Y119.83 E9.441 F350
G1 X88.55 Y80.17 E11.622 F350
G1 X122.90 Y100.00 E13.804 F350

; --- Syringe swap before layer 2 ---
G1 E11.804 F300 ; Retract
G1 Z50.00 F2000 ; Lift before syringe swap
G1 X10 Y10 F2000 ; Park away from print
; === SWITCH TO SYRINGE 2 ===
G92 E0 ; Reset extruder
G1 Z8.30 F2000 ; Return to layer height

; === Layer 2 | Syringe 2 | Z=8.30mm | R=22.9mm | Rot=3.0deg ===
G1 Z8.30 F2000 ; Layer height
; Outer triangle
G1 E-2.000 F300 ; Retract
G1 X98.87 Y101.20 F2000 ; XY travel
G1 E0.000 F300 ; Prime
G1 X98.87 Y101.20 E0.000 F350
G1 X63.53 Y119.21 E2.182 F350
G1 X65.60 Y79.60 E4.363 F350
G1 X98.87 Y101.20 E6.545 F350
; Inner triangle
G1 E4.545 F300 ; Retract
G1 X96.37 Y101.07 F2000 ; XY travel
G1 E6.545 F300 ; Prime
G1 X96.37 Y101.07 E6.545 F350
G1 X64.89 Y117.11 E8.488 F350
G1 X66.74 Y81.82 E10.431 F350
G1 X96.37 Y101.07 E12.375 F350

; --- Syringe swap before layer 3 ---
G1 E10.375 F300 ; Retract
G1 Z50.00 F2000 ; Lift before syringe swap
G1 X10 Y10 F2000 ; Park away from print
; === SWITCH TO SYRINGE 1 ===
G92 E0 ; Reset extruder
G1 Z10.60 F2000 ; Return to layer height

; === Layer 3 | Syringe 1 | Z=10.60mm | R=20.4mm | Rot=6.0deg ===
G1 Z10.60 F2000 ; Layer height
; Outer triangle
G1 E-2.000 F300 ; Retract
G1 X120.29 Y102.13 F2000 ; XY travel
G1 E0.000 F300 ; Prime
G1 X120.29 Y102.13 E0.000 F350
G1 X88.01 Y116.50 E1.943 F350
G1 X91.70 Y81.36 E3.887 F350
G1 X120.29 Y102.13 E5.830 F350
; Inner triangle
G1 E3.830 F300 ; Retract
G1 X117.80 Y101.87 F2000 ; XY travel
G1 E5.830 F300 ; Prime
G1 X117.80 Y101.87 E5.830 F350
G1 X89.48 Y114.48 E7.535 F350
G1 X92.72 Y83.65 E9.240 F350
G1 X117.80 Y101.87 E10.946 F350

; --- Syringe swap before layer 4 ---
G1 E8.946 F300 ; Retract
G1 Z50.00 F2000 ; Lift before syringe swap
G1 X10 Y10 F2000 ; Park away from print
; === SWITCH TO SYRINGE 2 ===
G92 E0 ; Reset extruder
G1 Z12.90 F2000 ; Return to layer height

; === Layer 4 | Syringe 2 | Z=12.90mm | R=17.9mm | Rot=9.0deg ===
G1 Z12.90 F2000 ; Layer height
; Outer triangle
G1 E-2.000 F300 ; Retract
G1 X93.68 Y102.80 F2000 ; XY travel
G1 E0.000 F300 ; Prime
G1 X93.68 Y102.80 E0.000 F350
G1 X64.74 Y113.91 E1.705 F350
G1 X69.59 Y83.29 E3.410 F350
G1 X93.68 Y102.80 E5.116 F350
; Inner triangle
G1 E3.116 F300 ; Retract
G1 X91.21 Y102.41 F2000 ; XY travel
G1 E5.116 F300 ; Prime
G1 X91.21 Y102.41 E5.116 F350
G1 X66.31 Y111.97 E6.583 F350
G1 X70.48 Y85.62 E8.050 F350
G1 X91.21 Y102.41 E9.517 F350

; --- Syringe swap before layer 5 ---
G1 E7.517 F300 ; Retract
G1 Z50.00 F2000 ; Lift before syringe swap
G1 X10 Y10 F2000 ; Park away from print
; === SWITCH TO SYRINGE 1 ===
G92 E0 ; Reset extruder
G1 Z15.20 F2000 ; Return to layer height

; === Layer 5 | Syringe 1 | Z=15.20mm | R=15.4mm | Rot=12.0deg ===
G1 Z15.20 F2000 ; Layer height
; Outer triangle
G1 E-2.000 F300 ; Retract
G1 X115.06 Y103.20 F2000 ; XY travel
G1 E0.000 F300 ; Prime
G1 X115.06 Y103.20 E0.000 F350
G1 X89.70 Y111.44 E1.467 F350
G1 X95.24 Y85.35 E2.934 F350
G1 X115.06 Y103.20 E4.401 F350
; Inner triangle
G1 E2.401 F300 ; Retract
G1 X112.62 Y102.68 F2000 ; XY travel
G1 E4.401 F300 ; Prime
G1 X112.62 Y102.68 E4.401 F350
G1 X91.37 Y109.59 E5.630 F350
G1 X96.01 Y87.73 E6.859 F350
G1 X112.62 Y102.68 E8.088 F350

; --- Syringe swap before layer 6 ---
G1 E6.088 F300 ; Retract
G1 Z50.00 F2000 ; Lift before syringe swap
G1 X10 Y10 F2000 ; Park away from print
; === SWITCH TO SYRINGE 2 ===
G92 E0 ; Reset extruder
G1 Z17.50 F2000 ; Return to layer height

; === Layer 6 | Syringe 2 | Z=17.50mm | R=12.9mm | Rot=15.0deg ===
G1 Z17.50 F2000 ; Layer height
; Outer triangle
G1 E-2.000 F300 ; Retract
G1 X88.46 Y103.34 F2000 ; XY travel
G1 E0.000 F300 ; Prime
G1 X88.46 Y103.34 E0.000 F350
G1 X66.88 Y109.12 E1.229 F350
G1 X72.66 Y87.54 E2.458 F350
G1 X88.46 Y103.34 E3.687 F350
; Inner triangle
G1 E1.687 F300 ; Retract
G1 X86.05 Y102.69 F2000 ; XY travel
G1 E3.687 F300 ; Prime
G1 X86.05 Y102.69 E3.687 F350
G1 X68.65 Y107.35 E4.677 F350
G1 X73.31 Y89.95 E5.668 F350
G1 X86.05 Y102.69 E6.659 F350

; --- Syringe swap before layer 7 ---
G1 E4.659 F300 ; Retract
G1 Z50.00 F2000 ; Lift before syringe swap
G1 X10 Y10 F2000 ; Park away from print
; === SWITCH TO SYRINGE 1 ===
G92 E0 ; Reset extruder
G1 Z19.80 F2000 ; Return to layer height

; === Layer 7 | Syringe 1 | Z=19.80mm | R=10.4mm | Rot=18.0deg ===
G1 Z19.80 F2000 ; Layer height
; Outer triangle
G1 E-2.000 F300 ; Retract
G1 X109.89 Y103.21 F2000 ; XY travel
G1 E0.000 F300 ; Prime
G1 X109.89 Y103.21 E0.000 F350
G1 X92.27 Y106.96 E0.991 F350
G1 X97.84 Y89.83 E1.981 F350
G1 X109.89 Y103.21 E2.972 F350
; Inner triangle
G1 E0.972 F300 ; Retract
G1 X107.51 Y102.44 F2000 ; XY travel
G1 E2.972 F300 ; Prime
G1 X107.51 Y102.44 E2.972 F350
G1 X94.13 Y105.29 E3.725 F350
G1 X98.36 Y92.27 E4.477 F350
G1 X107.51 Y102.44 E5.230 F350

; --- END ---
G1 E2.230 F300 ; Final retract
G1 Z50.00 F2000 ; Lift
G1 X0 Y0 F2000 ; Park at home
M84 ; Disable motors
M106 S0 ; Fan off