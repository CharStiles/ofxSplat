=====================================

Introduction
-------------
This is a [nerf gaussian splat](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/) renderer modeled after [antimatter15's webgl](https://github.com/antimatter15/splat) implementation.
You can create a splat from a video following instructions froma well written writeup from Alex Carlier [here](https://www.reshot.ai/3d-gaussian-splatting). 
You can read more about the creative process [here](https://www.media.mit.edu/posts/splat/). 

License
-------
[MIT License](https://en.wikipedia.org/wiki/MIT_License)

Installation
------------
Download this and drop it into `openFrameworks/addons/` folder. Your point clouds go into the data folder in your project.

Use
------------

First you put your gaussian splat nerf PLY in the data folder, say its called pointcloud.ply then you call  ```splat.setup("pointcloud.ply")``` to prepare the splats

Then in your update loop call ```splat.udate()``` 

Then in your draw loop call ```splat.draw()``` 

Compatibility
------------
OF 10 and 11

Credit
------------
Thanks to Zach Lieberman for writing the meat of the renderer, from the vbo, fixing the shaders, to the sorting and beyond. 

Developed at [Future Sketches Group](https://www.media.mit.edu/groups/future-sketches/overview/) at the [MIT Media Lab.](https://www.media.mit.edu/)
