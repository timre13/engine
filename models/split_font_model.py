# This script splits the "font.obj" model to the characters and saves them to the "font/" folder

import os
import sys
import re

fonts = []

try:
    os.mkdir("./font")
except FileExistsError:
    print("Font files already exist, exiting")
    sys.exit()
except:
    print("Warning: "+str(sys.exc_info()[1]))

with open("font.obj") as file:
    lines = file.readlines()
    for line in lines:
        if line.startswith("o"):
            fonts.append("")
        elif fonts:
            fonts[-1] += line

print("Generating font")

vertexCountWritten = 0
uvCountWritten = 0
normalCountWritten = 0

for i, font in enumerate(fonts):
    print("Processing: [{}/{}]".format(i+1, len(fonts)), end="\r")

    currentVertexCount = 0
    currentUvCount = 0
    currentNormalCount = 0
    output = ""
    for line in font.splitlines():
        if line.startswith("v "):
            currentVertexCount += 1
            output += line + "\n"
        elif line.startswith("vt "):
            currentUvCount += 1
            output += line + "\n"
        elif line.startswith("vn "):
            currentNormalCount += 1
            output += line + "\n"
        elif line.startswith("f "):
            output += "f "
            r = re.compile("[ \t]+")
            for vertex in r.split(line[2:]):
                vertexValues = [int(x) for x in vertex.split("/")]
                vertexValues[0] -= vertexCountWritten
                vertexValues[1] -= uvCountWritten
                vertexValues[2] -= normalCountWritten
                assert(vertexValues[0] >= 1 and vertexValues[1] >= 1 and vertexValues[2] >= 1)
                output += "/".join([str(x) for x in vertexValues])+" "
            output += "\n"
    vertexCountWritten += currentVertexCount
    uvCountWritten += currentUvCount
    normalCountWritten += currentNormalCount

    with open("./font/{}.obj".format(33+i), "w+") as file:
        file.write(output)

print("\nDone")

