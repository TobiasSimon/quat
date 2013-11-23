
/*
   quaternion library - implementation

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


#include <string.h>
#include <math.h>

#include "quat.h"

static const float ZERO_TOLERANCE = 0.000001f;

#ifndef FOR_N
#define FOR_N(v, m) for (int v = 0; v < m; ++v)
#endif /* FOR_N */


void vec3_copy(vec3_t *vo, vec3_t *vi)
{
   memcpy(vo, vi, sizeof(vec3_t));   
}


void quat_init(quat_t *q, const vec3_t *acc, const vec3_t *mag)
{
   float ax = acc->x;
   float ay = acc->y;
   float az = acc->z;
   float mx = mag->x;
   float my = mag->y;
   float mz = mag->z;


   float init_roll = atan2(-ay, -az);
   float init_pitch = atan2(ax, -az);

   float cos_roll = cosf(init_roll);
   float sin_roll = sinf(init_roll);
   float cos_pitch = cosf(init_pitch);
   float sin_pitch = sinf(init_pitch);

   float mag_x = mx * cos_pitch + my * sin_roll * sin_pitch + mz * cos_roll * sin_pitch;
   float mag_y = my * cos_roll - mz * sin_roll;

   float init_yaw = atan2(-mag_y, mag_x);

   cos_roll =  cosf(init_roll * 0.5f);
   sin_roll =  sinf(init_roll * 0.5f);

   cos_pitch = cosf(init_pitch * 0.5f );
   sin_pitch = sinf(init_pitch * 0.5f );

   float cosHeading = cosf(init_yaw * 0.5f);
   float sinHeading = sinf(init_yaw * 0.5f);

   q->q0 = cos_roll * cos_pitch * cosHeading + sin_roll * sin_pitch * sinHeading;
   q->q1 = sin_roll * cos_pitch * cosHeading - cos_roll * sin_pitch * sinHeading;
   q->q2 = cos_roll * sin_pitch * cosHeading + sin_roll * cos_pitch * sinHeading;
   q->q3 = cos_roll * cos_pitch * sinHeading - sin_roll * sin_pitch * cosHeading;
}


void quat_init_axis(quat_t *q, float x, float y, float z, float a)
{
   /* see: http://www.euclideanspace.com/maths/geometry/rotations
           /conversions/angleToQuaternion/index.htm */
   float a2 = a * 0.5f;
   float s = sin(a2);
   q->x = x * s;   
   q->y = y * s;   
   q->z = z * s;   
   q->w = cos(a2);   
}


void quat_init_axis_v(quat_t *q, const vec3_t *v, float a)
{
   quat_init_axis(q, v->x, v->y, v->z, a);
}


void quat_to_axis(const quat_t *q, float *x, float *y, float *z, float *a)
{
   /* see: http://www.euclideanspace.com/maths/geometry/rotations
           /conversions/quaternionToAngle/index.htm */
   float angle = 2 * acos(q->w);
   float s = sqrt(1.0 - q->w * q->w);
   if (s < ZERO_TOLERANCE) {
      /* if s close to zero then direction of axis not important */
      *a = 0;
      *x = 1;
      *y = 0;
      *z = 0;
   } else {
      *a = angle;
      *x = q->x / s; /* normalise axis */
      *y = q->y / s;
      *z = q->z / s;
   }
}


void quat_to_axis_v(const quat_t *q, vec3_t *v, float *a)
{
   quat_to_axis(q, &v->x, &v->y, &v->z, a);
}


void quat_rot_vec_self(vec3_t *v, const quat_t *q)
{
   vec3_t vo;
   quat_rot_vec(&vo, v, q);
   vec3_copy(v, &vo);
}


void quat_rot_vec(vec3_t *vo, const vec3_t *vi, const quat_t *q)
{
   /* see: https://github.com/qsnake/ase/blob/master/ase/quaternions.py */
   const float vx = vi->x, vy = vi->y, vz = vi->z;
   const float qw = q->w, qx = q->x, qy = q->y, qz = q->z;
   const float qww = qw * qw, qxx = qx * qx, qyy = qy * qy, qzz = qz * qz;
   const float qwx = qw * qx, qwy = qw * qy, qwz = qw * qz, qxy = qx * qy;
   const float qxz = qx * qz, qyz = qy * qz;
   vo->x = (qww + qxx - qyy - qzz) * vx + 2 * ((qxy - qwz) * vy + (qxz + qwy) * vz);
   vo->y = (qww - qxx + qyy - qzz) * vy + 2 * ((qxy + qwz) * vx + (qyz - qwx) * vz);
   vo->z = (qww - qxx - qyy + qzz) * vz + 2 * ((qxz - qwy) * vx + (qyz + qwx) * vy);
}


void quat_copy(quat_t *qo, const quat_t *qi)
{
   memcpy(qo, qi, sizeof(quat_t));   
}


float quat_len(const quat_t *q)
{
   float s = 0.0f;
   FOR_N(i, 4)
      s += q->vec[i] * q->vec[i];
   return sqrtf(s);
}


void quat_conj(quat_t *q_out, const quat_t *q_in)
{
   q_out->x = -q_in->x;
   q_out->y = -q_in->y;
   q_out->z = -q_in->z;
   q_out->w = q_in->w;
}


void quat_to_euler(euler_t *euler, const quat_t *quat)
{
   const float x = quat->x, y = quat->y, z = quat->z, w = quat->w;
   const float ww = w * w, xx = x * x, yy = y * y, zz = z * z;
   euler->yaw = normalize_euler_0_2pi(atan2f(2.f * (x * y + z * w), xx - yy - zz + ww));
   euler->pitch = asinf(-2.f * (x * z - y * w));
   euler->roll = atan2f(2.f * (y * z + x * w), -xx - yy + zz + ww);
}


void quat_mul(quat_t *o, const quat_t *q1, const quat_t *q2)
{
   /* see: http://www.euclideanspace.com/maths/algebra/
           realNormedAlgebra/quaternions/code/index.htm#mul */
   o->x =  q1->x * q2->w + q1->y * q2->z - q1->z * q2->y + q1->w * q2->x;
   o->y = -q1->x * q2->z + q1->y * q2->w + q1->z * q2->x + q1->w * q2->y;
   o->z =  q1->x * q2->y - q1->y * q2->x + q1->z * q2->w + q1->w * q2->z;
   o->w = -q1->x * q2->x - q1->y * q2->y - q1->z * q2->z + q1->w * q2->w;
}


void quat_add(quat_t *o, const quat_t *q1, const quat_t *q2)
{
   /* see: http://www.euclideanspace.com/maths/algebra/
           realNormedAlgebra/quaternions/code/index.htm#add */
   o->x = q1->x + q2->x;
   o->y = q1->y + q2->y;
   o->z = q1->z + q2->z;
   o->w = q1->w + q2->w;
}


void quat_add_to(quat_t *o, const quat_t *q)
{
   quat_t tmp;
   quat_add(&tmp, o, q);
   quat_copy(o, &tmp);
}


void quat_scale(quat_t *o, const quat_t *q, float f)
{
   /* see: http://www.euclideanspace.com/maths/algebra/
           realNormedAlgebra/quaternions/code/index.htm#scale*/
   FOR_N(i, 4)
      o->vec[i] = q->vec[i] * f;
}


void quat_scale_self(quat_t *q, float f)
{
   quat_scale(q, q, f);
}


void quat_normalize(quat_t *o, const quat_t *q)
{
   /* see: http://www.euclideanspace.com/maths/algebra/
           realNormedAlgebra/quaternions/code/index.htm#normalise */
   quat_scale(o, q, 1.0f / quat_len(q));
}


void quat_normalize_self(quat_t *q)
{
   quat_normalize(q, q);
}


float normalize_euler_0_2pi(float a)
{
   while (a < 0)
      a += (float)(2 * M_PI);
   return a;
}


/* m is pointer to array of 16 floats in column major order */
void quat_to_rh_rot_matrix(const quat_t *q, float *m)
{
   quat_t qn;
   float qw, qx, qy, qz;

   quat_normalize(&qn, q);

   qw = qn.w;
   qx = qn.x;
   qy = qn.y;
   qz = qn.z;

   m[0] = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
   m[1] = 2.0f * qx * qy + 2.0f * qz * qw;
   m[2] = 2.0f * qx * qz - 2.0f * qy * qw;
   m[3] = 0.0f;

   m[4] = 2.0f * qx * qy - 2.0f * qz * qw;
   m[5] = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
   m[6] = 2.0f * qy * qz + 2.0f * qx * qw;
   m[7] = 0.0f;

   m[8] = 2.0f * qx * qz + 2.0f * qy * qw;
   m[9] = 2.0f * qy * qz - 2.0f * qx * qw;
   m[10] = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;
   m[11] = 0.0f;

   m[12] = 0.0f;
   m[13] = 0.0f;
   m[14] = 0.0f;
   m[15] = 1.0f;
}


/* m is pointer to array of 16 floats in column major order */
void quat_to_lh_rot_matrix(const quat_t *q, float *m)
{
   quat_t qn;
   float qw, qx, qy, qz;

   quat_normalize(&qn, q);

   qw = qn.w;
   qx = qn.x;
   qy = qn.y;
   qz = qn.z;

   m[0] = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
   m[1] = 2.0f * qx * qy - 2.0f * qz * qw;
   m[2] = 2.0f * qx * qz + 2.0f * qy * qw;
   m[3] = 0.0f;

   m[4] = 2.0f * qx * qy + 2.0f * qz * qw;
   m[5] = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
   m[6] = 2.0f * qy * qz - 2.0f * qx * qw;
   m[7] = 0.0f;

   m[8] = 2.0f * qx * qz - 2.0f * qy * qw;
   m[9] = 2.0f * qy * qz + 2.0f * qx * qw;
   m[10] = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;
   m[11] = 0.0f;

   m[12] = 0.0f;
   m[13] = 0.0f;
   m[14] = 0.0f;
   m[15] = 1.0f;
}


void vec3_init(vec3_t *vo, float x, float y, float z)
{
   vo->x = x;
   vo->y = y;
   vo->z = z;
}


vec3_t *vec3_add(vec3_t *vo, const vec3_t *v1, const vec3_t *v2)
{
   vo->x = v1->x + v2->x;
   vo->y = v1->y + v2->y;
   vo->z = v1->z + v2->z;
   return vo;
}


vec3_t *vec3_add_self(vec3_t *v1, const vec3_t *v2)
{
   return vec3_add(v1, v1, v2);
}


vec3_t *vec3_add_c_self(vec3_t *v1, float x, float y, float z)
{
        v1->x += x;
        v1->y += y;
        v1->z += z;
        return v1;
}


vec3_t *vec3_sub(vec3_t *vo, const vec3_t *v1, const vec3_t *v2)
{
   vo->vec[0] = v1->vec[0] - v2->vec[0];
   vo->vec[1] = v1->vec[1] - v2->vec[1];
   vo->vec[2] = v1->vec[2] - v2->vec[2];
   return vo;
}


vec3_t *vec3_sub_self(vec3_t *v1, const vec3_t *v2)
{
   return vec3_sub(v1, v1, v2);
}


vec3_t *vec3_sub_c_self(vec3_t *v1, float x, float y, float z)
{
   v1->x -= x;
   v1->y -= y;
   v1->z -= z;
   return v1;
}


vec3_t *vec3_mul(vec3_t *vo, const vec3_t *vi, float scalar)
{
   vo->vec[0] = vi->vec[0] * scalar;
   vo->vec[1] = vi->vec[1] * scalar;
   vo->vec[2] = vi->vec[2] * scalar;
   return vo;
}


vec3_t *vec3_mul_self(vec3_t *vi, float scalar)
{
   return vec3_mul(vi, vi, scalar);
}


float vec3_dot(const vec3_t *v1, const vec3_t *v2)
{
   return v1->vec[0] * v2->vec[0] + v1->vec[1] * v2->vec[1] + v1->vec[2] * v2->vec[2];
}


vec3_t *vec3_cross(vec3_t *vo, const vec3_t *v1, const vec3_t *v2)
{
   vo->vec[0] = v1->vec[1]*v2->vec[2] - v1->vec[2]*v2->vec[1];
   vo->vec[1] = v1->vec[2]*v2->vec[0] - v1->vec[0]*v2->vec[2];
   vo->vec[2] = v1->vec[0]*v2->vec[1] - v1->vec[1]*v2->vec[0];
   return vo;
}


float vec3_len2(const vec3_t *v)
{
   return v->x * v->x + v->y * v->y + v->z * v->z;
}


vec3_t *vec3_normalize(vec3_t *vo, const vec3_t *vi)
{
   float len = sqrt(vec3_len2(vi));
   vo->x = vi->x / len;
   vo->y = vi->y / len;
   vo->z = vi->z / len;
   return vo;
}


vec3_t *vec3_rot_axis(vec3_t *vo, vec3_t *vi, float x, float y, float z, float angle)
{
   vec3_copy(vo, vi);
   return vec3_rot_axis_self(vo, x, y, z, angle);
}


vec3_t *vec3_rot_axis_self(vec3_t *vo, float x, float y, float z, float angle)
{
   quat_t rotate;
   quat_init_axis(&rotate, x, y, z, angle);
   quat_rot_vec_self(vo, &rotate);
   return vo;
}


double vec3_dist(const vec3_t *v1, const vec3_t *v2)
{
   return sqrt((v1->x - v2->x) * (v1->x - v2->x) +
               (v1->y - v2->y) * (v1->y - v2->y) +
               (v1->z - v2->z) * (v1->z - v2->z));
}


double vec3_dist_c(const vec3_t *v1, float x, float y, float z)
{
   return sqrt((v1->x - x) * (v1->x - x) +
               (v1->y - y) * (v1->y - y) +
               (v1->z - z) * (v1->z - z));
}

const quat_t identity_quat = { {1.0, 0.0, 0.0, 0.0} };
	
/* see http://gamedev.stackexchange.com/questions/15070/orienting-a-model-to-face-a-target */
/* Calculate the quaternion to rotate from vector u to vector v */
void quat_from_u2v(quat_t *q, const vec3_t *u, const vec3_t *v, const vec3_t *up)
{
   vec3_t un, vn, axis, axisn;
   float dot;
   float angle;

   vec3_normalize(&un, u);
   vec3_normalize(&vn, v);
   dot = vec3_dot(&un, &vn);
   if (fabs(dot - -1.0f) < ZERO_TOLERANCE) {
      /* vector a and b point exactly in the opposite direction
       * so it is a 180 degrees turn around the up-axis
       */
      vec3_t default_up = { { 0, 1, 0} };
      if (!up)
         up = &default_up;
      quat_init_axis(q, up->x, up->y, up->z, M_PI);
      return;
   }
   if (fabs(dot - 1.0f) < ZERO_TOLERANCE) {
      /* vector a and b point exactly in the same direction
       * so we return the identity quaternion
       */
      *q = identity_quat;
      return;
   }
   angle = acos(dot);
   vec3_cross(&axis, &un, &vn);
   vec3_normalize(&axisn, &axis);
   quat_init_axis(q, axisn.x, axisn.y, axisn.z, angle);
}


float quat_dot(const quat_t *q1, const quat_t *q2)
{
   return q1->vec[0] * q2->vec[0] + q1->vec[1] * q2->vec[1] +
          q1->vec[2] * q2->vec[2] + q1->vec[3] * q2->vec[3];
}


static quat_t *quat_lerp(quat_t *qo, const quat_t *qfrom, const quat_t *qto, float t)
{
   double cosom = quat_dot(qfrom, qto);

   /* qto = qfrom or qto = -qfrom so no rotation to slerp */
   if (cosom >= 1.0) {
      quat_copy(qo, qfrom);
      return qo;
   }

   /* adjust for shortest path */
   quat_t to1;
   if (cosom < 0.0) {
      to1.x = -qto->x;
      to1.y = -qto->y;
      to1.z = -qto->z;
      to1.w = -qto->w;
   } else {
      quat_copy(&to1, qto);
   }

   double scale0 = 1.0 - t;
   double scale1 = t;

   /* calculate final values */
   qo->x = scale0 * qfrom->x + scale1 * to1.x;
   qo->y = scale0 * qfrom->y + scale1 * to1.y;
   qo->z = scale0 * qfrom->z + scale1 * to1.z;
   qo->w = scale0 * qfrom->w + scale1 * to1.w;
   return qo;
}


quat_t *quat_nlerp(quat_t *qo, const quat_t *qfrom, const quat_t *qto, float t)
{
   quat_lerp(qo, qfrom, qto, t); 
   quat_normalize_self(qo);
   return qo; 
}


quat_t *quat_slerp(quat_t *qo, const quat_t *qfrom, const quat_t *qto, float t)
{
   /* calc cosine */
   double cosom = quat_dot(qfrom, qto);

   /* qto = qfrom or qto = -qfrom so no rotation to slerp */
   if (cosom >= 1.0) {
      quat_copy(qo, qfrom);
      return qo; 
   }   

   /* adjust for shortest path */
   quat_t to1;
   if (cosom < 0.0) {
      cosom = -cosom;
      to1.x = -qto->x;
      to1.y = -qto->y;
      to1.z = -qto->z;
      to1.w = -qto->w;
   } else {
      quat_copy(&to1, qto);
   }

   /* calculate coefficients */
   double scale0, scale1;
   if (cosom < 0.99995) {
      /* standard case (slerp) */
      double omega = acos(cosom);
      double sinom = sin(omega);
      scale0 = sin((1.0 - t) * omega) / sinom;
      scale1 = sin(t * omega) / sinom;
   } else {
      /* "from" and "to" quaternions are very close
       *  ... so we can do a linear interpolation
       */
      scale0 = 1.0 - t;
      scale1 = t;
   }

   /* calculate final values */
   qo->x = scale0 * qfrom->x + scale1 * to1.x;
   qo->y = scale0 * qfrom->y + scale1 * to1.y;
   qo->z = scale0 * qfrom->z + scale1 * to1.z;
   qo->w = scale0 * qfrom->w + scale1 * to1.w;
   return qo;
}


quat_t *quat_apply_relative_yaw_pitch_roll(quat_t *q,
                                        double yaw, double pitch, double roll)
{
        quat_t qyaw, qpitch, qroll, qrot, q1, q2, q3, q4;

        /* calculate amount of yaw to impart this iteration... */
        quat_init_axis(&qyaw, 0.0, 1.0, 0.0, yaw);
        /* Calculate amount of pitch to impart this iteration... */
        quat_init_axis(&qpitch, 0.0, 0.0, 1.0, pitch);
        /* Calculate amount of roll to impart this iteration... */
        quat_init_axis(&qroll, 1.0, 0.0, 0.0, roll);
        /* Combine pitch, roll and yaw */
        quat_mul(&q1, &qyaw, &qpitch);
        quat_mul(&qrot, &q1, &qroll);

        /* Convert rotation to local coordinate system */
        quat_mul(&q1, q, &qrot);
        quat_conj(&q2, q);
        quat_mul(&q3, &q1, &q2);
        /* Apply to local orientation */
        quat_mul(&q4, &q3, q);
        quat_normalize_self(&q4);
        *q = q4;
        return q;
}


quat_t *quat_apply_relative_yaw_pitch(quat_t *q, double yaw, double pitch)
{
        quat_t qyaw, qpitch, q1;

        /* calculate amount of yaw to impart this iteration... */
        quat_init_axis(&qyaw, 0.0, 1.0, 0.0, yaw);
        /* Calculate amount of pitch to impart this iteration... */
        quat_init_axis(&qpitch, 0.0, 0.0, 1.0, pitch);

        quat_mul(&q1, &qyaw, q);
        quat_mul(q, &q1, &qpitch);
        return q;
}

