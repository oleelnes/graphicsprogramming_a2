# Assignment 2 Evaluation

I think this is a good work. It has all the features and I must say that you made it very appealing.

The rendering is not very efficient since it does make a lot of draw calls. This
could have been improved by using batch rendering (packing more geometry in a
single VBO), instancing and face culling. 

The implementation is fair, but the classes are flat (no hierarchies) and you
have memory leaks (check the times you issue `new` and the times you issue
`delete`). 


My evaluation of this work is: **B**.
