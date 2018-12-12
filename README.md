## RayTracer

![A fisher, outlined against the setting sun, reflections bouncing across the water](https://i.imgur.com/uNnLggh.png)
"__Catching Light__" - *Scene created for final project and rendered with Raytracer in 4k*

### Raytracer started as a project for my computer graphics class.

## These were the original requirements:
* To run:
  * make
  * Ensure the necessary scene, object and material files are in the current directory
  * ./raytracer &nbsp;driver_file.txt &nbsp;output_image.ppm
* Read from the driver_file.txt (format is explained in scene_driver.md):
  * Camera specifications
  * Image specifications
  * Light position and color
  * Sphere location and color
  * Model position, rotation, and scale
 * Raytrace the scene. For every pixel:
   * Find ray-object intersections, keep the closest intersected object
   * Calculate pixel color based off of the colors of the intersected object and non-occluded lights
   * Recursively find reflections if the object has specularity
   * Recursively find refraction if the object has transparency
 * Save the image:
   * Scale each pixel's calculated color from 0-1 to 0-255
   * Write rgb triplets of the calculated color in ppm format
   * Save to output_image.ppm
 
 ## Additional Features that I've added:
 * Oct-tree traversal 
   * Split objects into an octtree to decrease ray-triangle intersection from O(n) to O(log(n))
 * Anti-aliasing
   * Render images larger than eventual output then scale down to output size to smooth edges
 * Focus blurring
   * Gradually blur rendered image as distance increases from focus point
 * Light grouping
   * Allow greater scene control by specifying which lights illuminate which objects
   
