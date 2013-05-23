#ifndef GLM_LIB_H_
#define GLM_LIB_H_

// Rules of thumb

// Do affine transformations in this order
// 1. Scale
// 2. Rotate
// 3. Translate

// Matrix multiplication is left to right. Ex:
// glm::vec4 v_out = projection * view * model * v_in;

// Specify a subset of the OpenGL math library to use
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#endif /* GLM_LIB_H_ */