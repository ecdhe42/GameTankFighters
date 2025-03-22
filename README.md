# GT Fighters

A fighting game for the GameTank console.

Build it with the "make" command

## Images

Images are done using GIMP 3.0:

- Load the PNG
- Image / Scale Image down to 640 pixels wide
- Image / Mode / Grayscaled
- Image / Mode / Indexed... and select the GameTank GAMETANK_HICON.act palette
- Copy the layer (used for player 1). On that copy:
    - Colors / Color to Alpha...
    - Click on Color
    - Select color 52
    - Copy the desired images to the spritesheet
- Copy the layer (used for player 2). On that copy:
    - Layer / Transform / Flip Horizontally
    - Colors / Color to Alpha...
    - Click on Color
    - Select color 212
    - Copy the desired images to the spritesheet
