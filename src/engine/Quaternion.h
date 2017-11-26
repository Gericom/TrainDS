#pragma once

class Quaternion
{
private:
	void Init(fx32 ax, fx32 ay, fx32 az, fx32 angle)
	{
		fx32 s = FX_SinIdx(FX_DEG_TO_IDX((angle + 1) >> 1));
		fx32 c = FX_CosIdx(FX_DEG_TO_IDX((angle + 1) >> 1));
		x = FX_Mul(ax, s);
		y = FX_Mul(ay, s);
		z = FX_Mul(az, s);
		w = c;
	}

public:
	Quaternion() { }

	Quaternion(VecFx32* axis, fx32 angle)
	{
		Init(axis->x, axis->y, axis->z, angle);
	}

	Quaternion(fx32 ax, fx32 ay, fx32 az, fx32 angle)
	{
		Init(ax, ay, az, angle);
	}	

	/*Quaternion(fx32 rx, fx32 ry, fx32 rz)
	{
        fx32 c1 = FX_CosIdx(FX_DEG_TO_IDX((rx + 1) >> 1));
        fx32 c2 = FX_CosIdx(FX_DEG_TO_IDX((ry + 1) >> 1));
        fx32 c3 = FX_CosIdx(FX_DEG_TO_IDX((rz + 1) >> 1));
        fx32 s1 = FX_SinIdx(FX_DEG_TO_IDX((rx + 1) >> 1));
        fx32 s2 = FX_SinIdx(FX_DEG_TO_IDX((ry + 1) >> 1));
        fx32 s3 = FX_SinIdx(FX_DEG_TO_IDX((rz + 1) >> 1));

        w = FX_Mul(FX_Mul(c1, c2), c3) - FX_Mul(FX_Mul(s1, s2), s3);
        x = FX_Mul(FX_Mul(s1, s2), c3) + FX_Mul(FX_Mul(c1, c2), s3);
        y = FX_Mul(FX_Mul(s1, c2), c3) + FX_Mul(FX_Mul(c1, s2), s3);
        z = FX_Mul(FX_Mul(c1, s2), c3) - FX_Mul(FX_Mul(s1, c2), s3);
	}*/

	fx32 x, y, z, w;

	//mult by vector:
	//aq - br - cs + i(bq + ar - ct) + j(cq + as + bt) + k(at - cr - bs)
	void MultiplyByVector(const VecFx32* vec, VecFx32* dst)
	{
		VecFx32 xyz = {x, y, z};
		VecFx32 t;
		VEC_CrossProduct(&xyz, vec, &t);
		t.x *= 2;
		t.y *= 2;
		t.z *= 2;
		VecFx32 tmp_dst = *vec;
		VEC_MultAdd(w, &t, &tmp_dst, &tmp_dst);
		VecFx32 cross;
		VEC_CrossProduct(&xyz, &t, &cross);
		VEC_Add(&tmp_dst, &cross, &tmp_dst);
		*dst = tmp_dst;
		/*fx32 xx = FX_Mul(vec->x, x) - FX_Mul(vec->y, y) - FX_Mul(vec->z, z);
		fx32 yy = FX_Mul(vec->x, y) + FX_Mul(vec->y, x) - FX_Mul(vec->z, w);
		fx32 zz = FX_Mul(vec->x, z) + FX_Mul(vec->y, w) + FX_Mul(vec->z, x);
		dst->x = xx;
		dst->y = yy;
		dst->z = zz;*/
	}

	void MultiplyByQuaternion(const Quaternion* quaternion, Quaternion* dst)
	{
		VecFx32 xyz;
		xyz.x = FX_Mul(quaternion->w, x);
		xyz.y = FX_Mul(quaternion->w, y);
		xyz.z = FX_Mul(quaternion->w, z);
		xyz.x += FX_Mul(w, quaternion->x);
		xyz.y += FX_Mul(w, quaternion->y);
		xyz.z += FX_Mul(w, quaternion->z);
		VecFx32 xyzl = {x,y,z};
		VecFx32 xyzr = {quaternion->x,quaternion->y,quaternion->z};
		VecFx32 cross;
		VEC_CrossProduct(&xyzl, &xyzr, &cross);
		xyz.x += cross.x;
		xyz.y += cross.y;
		xyz.z += cross.z;
		fx32 w_new = FX_Mul(w, quaternion->w) - VEC_DotProduct(&xyzl, &xyzr);
		dst->x = xyz.x;
		dst->y = xyz.y;
		dst->z = xyz.z;
		dst->w = w_new;
	}

	void Identity()
	{
		x = y = z = 0;
		w = FX32_ONE;
	}

	void Conjugate(Quaternion* dst)
	{
		dst->x = -x;
		dst->y = -y;
		dst->z = -z;
		dst->w = w;
	}
};