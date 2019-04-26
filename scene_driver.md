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
light  10 10 100 1 0.5 0.5 0.5

############## Scene Objects #################
#     corner1    side_length  side_length  dir1     dir2  res height  num_waves random_seed  material
wave   0 0 0     10           10           1 0 0   0 0 1  20   .1       4        300         wave.mtl

#       center  radius  ambient c  diffuse c  specular c  attenuation c  refract c  phong  eta  <light group> <animation>
sphere  1 0 0   2      .5 .5 .5    .8 .8 .8    0 0 0      .9 .9 .9       0 0 0     32      1.3  <integer> <file name>

#      Rotation axis    angle scale  translation     mesh file          <light group> <animation>
model  1.0 1.0 0.0      30    2      0.0 0.0 -15.0   cube_centered.obj  <integer>     <file name>
```
