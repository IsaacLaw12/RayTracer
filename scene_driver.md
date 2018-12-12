## Specifications for a scene driver file

#### An example driver
```
# Camera
eye  0 0 100
look  0 0 0
up  0 1 0
d  30
bounds  -2 -2 2 2

# Image
res  512 512
anti_alias  1
focus_blur  50 30 10

# Lighting
recursionLevel  1
ambient  0.2 0.2 0.2
light  10 10 100 1 0.5 0.5 0.5

# Scene Objects
sphere  1 0 0  2  .5 .5 .5   .8 .8 .8   0 0 0  .9 .9 .9   0 0 0  32  1.3
model  1.0 1.0 0.0 30  2  0.0 0.0 -15.0 cube_centered.obj
```
