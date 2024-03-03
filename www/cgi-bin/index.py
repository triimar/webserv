#!/usr/bin/python3

import os
import colorsys

# Set the path to the directory you want to index
directory_path = '.'

# Function to calculate perceived brightness
def calculate_brightness(rgb):
    r, g, b = [x / 255.0 for x in rgb]
    return colorsys.rgb_to_hsv(r, g, b)[2]

# Print the HTTP header
print("Content-type: text/html\n")

# HTML start
print("<html><head><title>CGI Directory Index</title>")
print("<style>")
print("body { font-family: 'Arial', sans-serif; background-color: #f7f7f7; margin: 40px; }")
print("h1 { color: #333; text-align: center; }")
print("#container { max-width: 400px; margin: 0 auto; display: flex; flex-direction: column; }")
print(".section { flex-grow: 1; }")
print("ul { list-style-type: none; padding: 0; text-align: center; }")
print("li { margin: 5px; padding: 10px; border-radius: 5px; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); text-align: left; display: inline-block; box-sizing: border-box; }")
print("a { text-decoration: none; color: #000000; }")
print("a:hover { color: #FFFFFF; }")
print("</style>")
print("</head><body>")
print("<h1>CGI Directory Index</h1>")
print("<div id='container'>")

# List Folders
print("<div class='section'>")
print("<h2>Folders</h2>")
print("<ul>")
for filename in os.listdir(directory_path):
    file_path = os.path.join(directory_path, filename)
    # Check if it is a directory
    if os.path.isdir(file_path):
        hash_color = "#{:06x}".format(hash(filename) % 0xFFFFFF)
        rgb_color = tuple(int(hash_color[i:i + 2], 16) for i in (1, 3, 5))
        brightness = calculate_brightness(rgb_color)
        
        # Adjust brightness if too dark (you can customize the threshold)
        if brightness < 0.6:
            rgb_color = tuple(min(int(255 - c), 255) for c in rgb_color)
        
        adjusted_color = "#{:02x}{:02x}{:02x}".format(*rgb_color)
        
        print(f'<li style="background-color: {adjusted_color};">'
              f'<a href="{filename}" target="_blank">{filename}</a></li>')
print("</ul>")
print("</div>")

# List Files
print("<div class='section'>")
print("<h2>Files</h2>")
print("<ul>")
for filename in os.listdir(directory_path):
    file_path = os.path.join(directory_path, filename)
    # Check if it is a file
    if os.path.isfile(file_path):
        hash_color = "#{:06x}".format(hash(filename) % 0xFFFFFF)
        rgb_color = tuple(int(hash_color[i:i + 2], 16) for i in (1, 3, 5))
        brightness = calculate_brightness(rgb_color)
        
        # Adjust brightness if too dark (you can customize the threshold)
        if brightness < 0.6:
            rgb_color = tuple(min(int(255 - c), 255) for c in rgb_color)
        
        adjusted_color = "#{:02x}{:02x}{:02x}".format(*rgb_color)
        
        print(f'<li style="background-color: {adjusted_color};">'
              f'<a href="{filename}" target="_blank">{filename}</a></li>')
print("</ul>")
print("</div>")

# HTML end
print("</div></body></html>")
