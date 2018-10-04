# Design considerations for graphics engine #

How many shader types do we need?

 - Generic sprite (quad+tex+light). Is there an animation?
 - Text rendering. Is it the same?
 - Pretty sprite with normal maps.
 - Particle filters can get some separate shader for rendering single points
 - UI: quad + tex
 - Fonts: quad + tex?
 - Wireframe rendering. It is debug stuff

How many different vertex types do we need?

 - vertices for particle filter
 - vertices for the sprites
 - UI: 2d position, color
 - Font: is it the same as UI?
 
# Rendering structure #

We create and fill in a new RenderContext instance for each new frame. It should contain all the information necessary to render the frame.

RenderQueue is a queue of the following pairs: {transform, primitiveObject}

1. We iterate over all scene objects and add its internal primitives to a set of render queues. Each scene 'layer' has its own render queue. Objects that are not visible are not added to the render queue.

2. Iterate all over render queues from each layer and generate vertex batches that are sent to the GPU. We push the batch to GPU every time when we change render parameters, like uniforms or textures.

How many primitive objects do we have?
- Batch
- SpriteData. It's just a quad
- Font. It is complex. Or not complex?

