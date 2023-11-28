import cv2
import sys

# arg1 -- source
# arg2 -- destination
# arg3 -- array_name

image = cv2.imread(sys.argv[1])
im = cv2.cvtColor(image, cv2.COLOR_BGRA2RGBA)

pixels = []
for row in im:
    for col in row:
        for color in col:
            pixels.append(hex(color))

with open(sys.argv[2], 'w') as file_obj:
    print("#include \"plugin.h\"", file=file_obj)

    print("//extern plugin::Texture", sys.argv[3], ";", file=file_obj)

    print("static unsigned char", "{}_pixels".format(sys.argv[3]), "[] = {", file=file_obj)
    print(", ".join(pixels), file=file_obj)
    print("};", file=file_obj)

    print("plugin::Texture", sys.argv[3], "= plugin::Texture({}, {}, {});".format(image.shape[1], image.shape[0], "{}_pixels".format(sys.argv[3])), file=file_obj)