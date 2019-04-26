## Specifications for a scene driver file

#### An example driver file
```
############## Camera #################
eye  0 0 100
look  0 0 0
up  0 1 0
d  30
bounds  -2 -2 2 2

############## Animation #################
frames 16
start_frame 0

############## Image #################
res  512 512
anti_alias  1

############## Lighting #################
recursionLevel  1
ambient  0.2 0.2 0.2
light  10 10 100 1 0.5 0.5 0.5 <light group integer>

############## Scene Objects #################
#     corner1    side_length  side_length  dir1     dir2  res height  num_waves random_seed  material
wave   0 0 0     10           10           1 0 0   0 0 1  20   .1       4        300         wave.mtl

#       center  radius  ambient c  diffuse c  specular c  attenuation c  refract c  phong  eta  <light group> <animation>
sphere  1 0 0   2      .5 .5 .5    .8 .8 .8    0 0 0      .9 .9 .9       0 0 0     32      1.3  <integer> <file name>

#      Rotation axis    angle scale  translation     mesh file          <light group> <animation>
model  1.0 1.0 0.0      30    2      0.0 0.0 -15.0   cube_centered.obj  <integer>     <file name>
```

### Camera
The camera can be thought of as being shaped like a pyramid.  The rectangular side of the pyramid is aimed at the scene, and the point of the pyramid is where someone is looking through the rectangle window out at the scene.  The closer the point is to the rectangle, the wider the angle of the camera is.

* eye:  The 3d location that the viewer's eye is located at
* look: The 3d location that the eye is looking towards
* up:   The 3d direction that is considered up in the scene
* d:    The distance between eye and the "rectangle of the pyramid", in technical terms the near image plane.
* bounds: x1 y1 x2 y2  Defines the corners of the near image plane.  Imagine a ray from the eye straight to look.  Move along this ray d units.  On the plane orthogonal to this ray the corners are defined by the offset of (x1, y1) and (x2, y2)

### Animation
* frames:  The number of frames to render for the animation
* start_frame:  Which frame to start on.  The accepted range is from 0 to frames.  This is useful for concurrently rendering an animation on two different systems.

### Image
* res:  width and height of output image. The result should have the same aspect ratio of the bounds element of camera.
* anti_alias:  Default is 0. Recommended amount is 1 or 2. Antialiasing is implemented as rendering an image several times larger than the actual image, then scaling down to the final output to smooth sharp edges. The image is rendered as original_dimensions + anti_alias * original_dimensions, then scaled down to original_dimensions

### Lighting
* recursionLevel:  For refraction and reflection recursive raytracing is performed. On initial impact of a ray another ray is shot off to find the light that would land there.  This controls how many times these rays bounce around the scene picking up reflections.
* ambient:   Background color in the scene, rgb values scaled from 0-1
* light: 3d location 1 rgb values scaled from 0-1

### Scene Objects
* wave: corner, side lengths and direction vectors define a 2d rectangle.  Direction vectors should be orthogonal unit vectors. The larger res is the finer the detail of linear ray-search intersection algorithm.  Height is the maximum amount that the waves will displace from the original 2d rectangle. Num waves is how many different random sine waves will be composed into the wave surface.  Random seed is fed into the random wave generator. It means that each time the program is run with the same seed the same waves will be created.

* sphere: center is the 3d position of the center of the sphere, radius is the radius of the sphere.  All of the elements that end in c are various colors of the sphere. Phong, probably best to look up the wikipedia article. Eta defines the speed of light in the object. A vacumn has an eta of 1.  Lighting group and animation file are optional. If lighting group is defined then only lights with the same lighting group will illuminate the object.  See the animation_file.md file for details on the animation file.
* model: All of the elements before the .obj file are defining a 3d transformation.  Rotation axis, amount of rotation in degrees, scale (default 1) then translation. These are used to transform the points and triangles defined in the .obj object into the scene. Lighting group and animation file are optional. If lighting group is defined then only lights with the same lighting group will illuminate the object.  See the animation_file.md file for details on the animation file.
