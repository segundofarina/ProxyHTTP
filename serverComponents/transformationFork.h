#ifndef _TRANSFORMATION_FORK_H_
#define _TRANSFORMATION_FORK_H_

/* Returns pid of the transformation process, or -1 on error */
int forkTransformation(int * readTransformFd, int * writeTransformFd);

#endif
