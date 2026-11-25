"""Draws the Vehicle Settings Copy icon: a truck whose settings sheet is copied onto a second sheet.

  python Tools/make_icon.py

Writes Docs/Images/icon-1080.png, icon-512.png and Resources/Icon128.png via Tools/Icons/iconkit.py.
"""
import sys

sys.path.insert(0, r"D:\SFMod\Tools\Icons")
import iconkit
from iconkit import s, box, ellipse

TRUCK = (250, 149, 73)
TRUCK_DARK = (196, 104, 40)
SHEET = (226, 230, 240)
SHEET_BACK = (120, 128, 150)
LINE = (120, 126, 142)
ARROW = (150, 110, 230)

image, draw = iconkit.canvas((38, 40, 58), (20, 21, 32))

# Back sheet (the copy) and front sheet (the original).
draw.rounded_rectangle(box(250, 120, 440, 330), radius=s(16), fill=SHEET_BACK)
draw.rounded_rectangle(box(90, 70, 300, 300), radius=s(16), fill=SHEET)
for i in range(4):
    y = 110 + i * 40
    draw.rounded_rectangle(box(115, y, 275 - (i % 2) * 40, y + 14), radius=s(6), fill=LINE)

# Truck below the sheets.
draw.rounded_rectangle(box(70, 340, 300, 420), radius=s(10), fill=TRUCK)
draw.rounded_rectangle(box(300, 360, 400, 420), radius=s(10), fill=TRUCK_DARK)
for cx in (130, 260, 350):
    ellipse(draw, cx, 424, 26, fill=(30, 30, 40))
    ellipse(draw, cx, 424, 12, fill=(150, 154, 166))

# Arrow from the front sheet to the back one.
draw.polygon([(s(300), s(180)), (s(340), s(180)), (s(340), s(160)), (s(380), s(200)), (s(340), s(240)), (s(340), s(220)), (s(300), s(220))], fill=ARROW)

iconkit.save(image, __file__)
