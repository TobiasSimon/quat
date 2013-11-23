
/*
   quaternion library - interface

   Copyright (C) 2013 Tobias Simon
   most of the code was stolen from the Internet

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
*/


#ifndef __QUAT_H__
#define __QUAT_H__


/* generic 3d vector */
typedef union
{
   struct
   {
      float x;
      float y;
      float z;
   };
   float vec[3];
}
vec3_t;


/* quaternion */
typedef union
{
   struct
   {
      float q0;
      float q1;
      float q2;
      float q3;
   };
   struct
   {
      float w;
      float x;
      float y;
      float z;
   };
   float vec[4];
}
quat_t;


/* euler angle */
typedef union
{
   struct
   {
      float yaw;
      float pitch;
      float roll;
   };
   float vec[3];
}
euler_t;


/* copy vector vi to vo */
void vec3_copy(vec3_t *vo, vec3_t *vi);

/* init orientation quaternion from measurements */
void quaternion_init(quat_t *quat, const vec3_t *acc, const vec3_t *mag);

/* initialize quaternion from axis angle using floats */
void quat_init_axis(quat_t *q, float x, float y, float z, float a);

/* initialize quaternion from axis angle using a vector */
void quat_init_axis_v(quat_t *q, const vec3_t *v, float a);

/* extract axis and angle from a quaternion */
void quat_to_axis(const quat_t *q, float *x, float *y, float *z, float *a);

/* extract axis in vector form and angle from a quaternion */
void quat_to_axis_v(const quat_t *q, vec3_t *v, float *a);

/* rotate vector vi via unit quaternion q and put result into vector vo */
void quat_rot_vec(vec3_t *vo, const vec3_t *vi, const quat_t *q);

/* rotate vector v_in in-place via unit quaternion quat */
void quat_rot_vec_self(vec3_t *v, const quat_t *q);

/* returns len of quaternion */
float quat_len(const quat_t *q);

/* copy quaternion qi to qo */
void quat_copy(quat_t *qo, const quat_t *qi);

/* qo = qi * f */
void quat_scale(quat_t *qo, const quat_t *qi, float f);

/* qo *= f */
void quat_scale_self(quat_t *q, float f);

/* conjugate quaternion */
void quat_conj(quat_t *qo, const quat_t *qi);

/* o = q1 + 12 */
void quat_add(quat_t *qo, const quat_t *q1, const quat_t *q2);

/* o += q */
void quat_add_self(quat_t *o, const quat_t *q);

/* o = q1 * q2 */
void quat_mul(quat_t *o, const quat_t *q1, const quat_t *q2);

/* normalizes quaternion q and puts result into o */
void quat_normalize(quat_t *qo, const quat_t *qi);

/* normalize q in-place */
void quat_normalize_self(quat_t *q);

/* convert quaternion to euler angles */
void quat_to_euler(euler_t *e, const quat_t *q);

/* normalize angle */
float normalize_euler_0_2pi(float a);

/* Convert quaternion to right handed rotation matrix. m is a pointer
 * to 16 floats in column major order.
 */
void quat_to_rh_rot_matrix(const quat_t *q, float *m);

/* Convert quaternion to left handed rotation matrix. m is a pointer
 * to 16 floats in column major order.
 */
void quat_to_lh_rot_matrix(const quat_t *q, float *m);

/* initialize vector */
void vec3_init(vec3_t *vo, float x, float y, float z);

/* vo = v1 + v2 */
vec3_t *vec3_add(vec3_t *vo, const vec3_t *v1, const vec3_t *v2);

/* v1 = v1 + v2 */
vec3_t *vec3_add_self(vec3_t *v1, const vec3_t *v2);

/* v1 += [x, y, z] */
vec3_t *vec3_add_c_self(vec3_t *v1, float x, float y, float z);

/* vo = v1 - v2 */
vec3_t *vec3_sub(vec3_t *vo, const vec3_t *v1, const vec3_t *v2);

/* v1 = v1 - v2 */
vec3_t *vec3_sub_self(vec3_t *v1, const vec3_t *v2);

/* v1 -= [x, y, z] */
vec3_t *vec3_sub_c_self(vec3_t *v1, float x, float y, float z);

/* vo = vi * scalar */
vec3_t *vec3_mul(vec3_t *vo, const vec3_t *vi, float scalar);

/* vi *= scalar */
vec3_t *vec3_mul_self(vec3_t *vi, float scalar);

/* return dot product of v1 and v2 */
float vec3_dot(const vec3_t *v1, const vec3_t *v2);

/* vo = v1 x v2 (cross product) */ 
vec3_t *vec3_cross(vec3_t *vo, const vec3_t *v1, const vec3_t *v2);

/* returns square of the magnitude of v */
float vec3_len2(const vec3_t *v);

/* vo = normalized vi */
vec3_t *vec3_normalize(vec3_t *vo, const vec3_t *vi);

/* vec3 rotate by axis and angle */
vec3_t *vec3_rot_axis(vec3_t *vo, vec3_t *vi, float x, float y, float z, float angle);

/* vec3 rotate self by axis and angle */
vec3_t *vec3_rot_axis_self(vec3_t *vo, float x, float y, float z, float angle);

/* return distance between v1 and v2 */
double vec3_dist(const vec3_t *v1, const vec3_t *v2);

/* return distance between v1 and [x, y, z] */
double vec3_dist_c(const vec3_t *v1, float x, float y, float z);

/* identity quaternion */
extern const quat_t identity_quat;

/* see http://gamedev.stackexchange.com/questions/15070/orienting-a-model-to-face-a-target */
/* Calculate the quaternion to rotate from vector u to vector v */
void quat_from_u2v(quat_t *q, const vec3_t *u, const vec3_t *v, const vec3_t *up);

/* quaternion dot product q1 . q2 */
float quat_dot(const quat_t *q1, const quat_t *q2);

/* calculate normalized linear quaternion interpolation */
quat_t *quat_nlerp(quat_t *qo, const quat_t *qfrom, const quat_t *qto, float t);

/* calculate spherical quaternion interpolation */
quat_t *quat_slerp(quat_t *qo, const quat_t *qfrom, const quat_t *qto, float t);

/* Apply incremental yaw, pitch and roll relative to the quaternion.
 * For example, if the quaternion represents an orientation of a ship,
 * this will apply yaw/pitch/roll *in the ship's local coord system to the
 * orientation.
 */
quat_t *quat_apply_relative_yaw_pitch_roll(quat_t *q,
                                        double yaw, double pitch, double roll);

/* Apply incremental yaw and pitch relative to the quaternion.
 * Yaw is applied to world axis so no roll will accumulate */
quat_t *quat_apply_relative_yaw_pitch(quat_t *q, double yaw, double pitch);

#endif /* __QUAT_H__ */

