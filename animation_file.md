###
Each line consists of a frame number and a transformation.  The transformation will be incrementally applied
from the previous frame number to this frame number. 0 is implied to be the previous frame number for the first
transformation in the file.

### Model example animation
```
16   0 0 1    16    1     1 0 0
32   0 0 1   -16    1    -1 0 0
```
For more information on the transformation format look at the scene driver file. </br>
This transformation would incrementally rotate 16 degrees along the z axis, and move
1 unit in the x direction from frames 0 to 16.
Then from frame 16 to frame 32 the object would be rotated -16 degrees around the z axis, 
and moved -1 unit in the x direction reversing the original transformation.


### Sphere example animation
```
16     0 -5  0      2
```
From frame 0 to frame 16 the sphere's center is moved -5 units in the y direction. The radius of the 
sphere is scaled up by 2.

