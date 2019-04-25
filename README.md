## RayTracer

![A fisherman, outlined against the setting sun, reflections bouncing across the water](https://i.imgur.com/gERVmbB.png)
"__Catching Light__" - *Still frame from scene animation available [here](https://youtu.be/bL9ruEOcmrc)*

## How to use
* To run:
  * Clone the repository and type 'make' in the Raytracer directory
  * Copy all of the files from one of the Scene folders into the same directory
  * Run with ./raytracer &nbsp;driver_file.txt &nbsp;output_image.ppm
  * Consult scene_driver.md for details on how the scene driver works


## Stages of development
   The project started as a semester long project to build a render engine for my computer graphics class.  At the end of the class my raytracer could render still images of triangle meshes and spheres with realistic details such as reflection and refraction.
   Once the computer graphics class was finished I continued development for an independent study.  In this second stage of development I updated the whole raytracer program to support animated objects.  I also added a new kind of object, a 3d wave object that is the composite of multiple sine waves.  The finale of the independent study was a render of a sunset feature that prominently featured this wave surface.  The final render is available [here]( https://youtu.be/bL9ruEOcmrc)
   
## Development for Computer Graphics class:

* Read from the <A DRIVER FILE>.txt (format is explained in scene_driver.md):
  * Camera location, look and size
  * Image width and height
  * Lights position and color
  * Spheres location and color
  * Models position, rotation, and scale
 * Raytrace the scene. For every pixel:
   * Find ray-object intersections, keep the closest intersected object
   * Calculate pixel color based off of the colors of the intersected object and non-occluded lights
   * Recursively find reflections if the object has specularity
   * Recursively find refraction if the object has transparency
 * Save the image:
   * Scale each pixel's calculated color from 0-1 to 0-255
   * Write rgb triplets of the calculated color in ppm format
   * Save to output_image.ppm
 
 ## Additional Features that I added for the Computer Graphics class:
 * Oct-tree traversal 
   * Complex polygonal objects are meshes formed of hundreds if not thousands of triangle faces.  The naive algorithm checks a ray for intersection with every single triangle.
   * My improvement splits each mesh into a recursively reducing octtree bounding structure. Only the areas of the mesh that intersect the ray are checked decreaseing ray-triangle intersection from O(n) to O(log(n))
 * Anti-aliasing
   * Render images larger than eventual output then scale down to output size to smooth edges
 * Light grouping
   * Allow greater scene control by specifying which lights illuminate which objects
 
 
 ## Development for independent study
 * Animation system
   * Still images are now equivalent to animations with only one frame
   * All rendered images are saved to ./tmp_renders
   * The starting frame and number of frames in the animation are specified in the driver file
   * For model and spheres objects the filename of an animation file can be added in the driver file to make it an animated object.
   * For further details on the animated object format look at animation_spec.md
 * Wave Object
   * Random sine waves are generated based off of specified number of waves and the random seed
   * The resulting sine waves are combined into a parameterized function that takes (x, y, z) coordinates and time
   * Initially I tried to represent this function as a triangulated mesh, but the results were quite choppy
   * To get the smooth gentle look of actual waves I created a searching algorithm that takes linear steps along rays to detect intersection with the wave surface. The point of intersection is refined with binary searching.  
