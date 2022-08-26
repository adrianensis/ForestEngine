#include "Core/Maths/Matrix4.hpp"
#include "Core/Maths/MathUtils.hpp"
#include "Core/Maths/Vector4.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Quaternion.hpp"

Matrix4::Matrix4()
{
	zeros();
}

Matrix4::Matrix4(const Matrix4& other)
{
	init(other);
}

void Matrix4::setRows(const std::array<f32, 4> &row0, const std::array<f32, 4> &row1, const std::array<f32, 4> &row2, const std::array<f32, 4> &row3)
{
	const std::array<f32, 4> *rows[4] = {&row0, &row1, &row2, &row3};

	FOR_RANGE(row, 0, 4)
	FOR_RANGE(col, 0, 4)
	set(row, col, (*(rows[row]))[col]);
}

void Matrix4::setRows(const f32 *row0, const f32 *row1, const f32 *row2, const f32 *row3)
{
	const f32 *rows[4] = {row0, row1, row2, row3};

	FOR_RANGE(row, 0, 4)
	FOR_RANGE(col, 0, 4)
	set(row, col, rows[row][col]);
}

void Matrix4::setRows(f32 n)
{
	FOR_RANGE(i, 0, 4 * 4)
	mData[i] = n;
}

void Matrix4::setRows(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
{
	Vector4 rows[4] = {row0, row1, row2, row3};

	FOR_RANGE(row, 0, 4)
	FOR_RANGE(col, 0, 4)
	set(row, col, rows[row][col]);
}

const Matrix4& Matrix4::getIdentity()
{
	if (!smIdentity)
	{
		smIdentity = OwnerPtr<Matrix4>(NEW(Matrix4));
		smIdentity.get().identity();
	}

	return smIdentity.get();
}

void Matrix4::init(f32 n)
{
	setRows(n);
}

void Matrix4::init(const Matrix4& other)
{
	init(other.mData); //TRACE()
}

void Matrix4::init(const std::array<f32, 16> &data)
{
	init(data.data());
}

void Matrix4::init(const std::array<f32, 4> &row0, const std::array<f32, 4> &row1, const std::array<f32, 4> &row2, const std::array<f32, 4> &row3)
{
	setRows(row0, row1, row2, row3);
}

void Matrix4::init(const f32 *data)
{
	memcpy(mData, data, 16 * sizeof(f32));
}

void Matrix4::init(const f32 *row0, const f32 *row1, const f32 *row2, const f32 *row3)
{
	setRows(row0, row1, row2, row3);
}

void Matrix4::init(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
{
	setRows(row0, row1, row2, row3);
}

void Matrix4::transpose()
{
	Matrix4 copy;
	copy.init((*this));

	FOR_RANGE(row, 0, 4)
	FOR_RANGE(col, 0, 4)
	this->set(col, row, copy.get(row, col));
}

void Matrix4::invert()
{
	f32 tmp_0 = this->get(2, 2) * this->get(3, 3);
	f32 tmp_1 = this->get(3, 2) * this->get(2, 3);
	f32 tmp_2 = this->get(1, 2) * this->get(3, 3);
	f32 tmp_3 = this->get(3, 2) * this->get(1, 3);
	f32 tmp_4 = this->get(1, 2) * this->get(2, 3);
	f32 tmp_5 = this->get(2, 2) * this->get(1, 3);
	f32 tmp_6 = this->get(0, 2) * this->get(3, 3);
	f32 tmp_7 = this->get(3, 2) * this->get(0, 3);
	f32 tmp_8 = this->get(0, 2) * this->get(2, 3);
	f32 tmp_9 = this->get(2, 2) * this->get(0, 3);
	f32 tmp_10 = this->get(0, 2) * this->get(1, 3);
	f32 tmp_11 = this->get(1, 2) * this->get(0, 3);
	f32 tmp_12 = this->get(2, 0) * this->get(3, 1);
	f32 tmp_13 = this->get(3, 0) * this->get(2, 1);
	f32 tmp_14 = this->get(1, 0) * this->get(3, 1);
	f32 tmp_15 = this->get(3, 0) * this->get(1, 1);
	f32 tmp_16 = this->get(1, 0) * this->get(2, 1);
	f32 tmp_17 = this->get(2, 0) * this->get(1, 1);
	f32 tmp_18 = this->get(0, 0) * this->get(3, 1);
	f32 tmp_19 = this->get(3, 0) * this->get(0, 1);
	f32 tmp_20 = this->get(0, 0) * this->get(2, 1);
	f32 tmp_21 = this->get(2, 0) * this->get(0, 1);
	f32 tmp_22 = this->get(0, 0) * this->get(1, 1);
	f32 tmp_23 = this->get(1, 0) * this->get(0, 1);

	f32 t0 = ((tmp_0 * this->get(1, 1) + tmp_3 * this->get(2, 1) + tmp_4 * this->get(3, 1)) - (tmp_1 * this->get(1, 1) + tmp_2 * this->get(2, 1) + tmp_5 * this->get(3, 1)));
	f32 t1 = ((tmp_1 * this->get(0, 1) + tmp_6 * this->get(2, 1) + tmp_9 * this->get(3, 1)) - (tmp_0 * this->get(0, 1) + tmp_7 * this->get(2, 1) + tmp_8 * this->get(3, 1)));
	f32 t2 = ((tmp_2 * this->get(0, 1) + tmp_7 * this->get(1, 1) + tmp_10 * this->get(3, 1)) - (tmp_3 * this->get(0, 1) + tmp_6 * this->get(1, 1) + tmp_11 * this->get(3, 1)));
	f32 t3 = ((tmp_5 * this->get(0, 1) + tmp_8 * this->get(1, 1) + tmp_11 * this->get(2, 1)) - (tmp_4 * this->get(0, 1) + tmp_9 * this->get(1, 1) + tmp_10 * this->get(2, 1)));

	f32 d = 1.0 / (this->get(0, 0) * t0 + this->get(1, 0) * t1 + this->get(2, 0) * t2 + this->get(3, 0) * t3);

	f32 out_00 = d * t0;
	f32 out_01 = d * t1;
	f32 out_02 = d * t2;
	f32 out_03 = d * t3;

	f32 out_10 = d * ((tmp_1 * this->get(1, 0) + tmp_2 * this->get(2, 0) + tmp_5 * this->get(3, 0)) - (tmp_0 * this->get(1, 0) + tmp_3 * this->get(2, 0) + tmp_4 * this->get(3, 0)));
	f32 out_11 = d * ((tmp_0 * this->get(0, 0) + tmp_7 * this->get(2, 0) + tmp_8 * this->get(3, 0)) - (tmp_1 * this->get(0, 0) + tmp_6 * this->get(2, 0) + tmp_9 * this->get(3, 0)));
	f32 out_12 = d * ((tmp_3 * this->get(0, 0) + tmp_6 * this->get(1, 0) + tmp_11 * this->get(3, 0)) - (tmp_2 * this->get(0, 0) + tmp_7 * this->get(1, 0) + tmp_10 * this->get(3, 0)));
	f32 out_13 = d * ((tmp_4 * this->get(0, 0) + tmp_9 * this->get(1, 0) + tmp_10 * this->get(2, 0)) - (tmp_5 * this->get(0, 0) + tmp_8 * this->get(1, 0) + tmp_11 * this->get(2, 0)));

	f32 out_20 = d * ((tmp_12 * this->get(1, 3) + tmp_15 * this->get(2, 3) + tmp_16 * this->get(3, 3)) - (tmp_13 * this->get(1, 3) + tmp_14 * this->get(2, 3) + tmp_17 * this->get(3, 3)));
	f32 out_21 = d * ((tmp_13 * this->get(0, 3) + tmp_18 * this->get(2, 3) + tmp_21 * this->get(3, 3)) - (tmp_12 * this->get(0, 3) + tmp_19 * this->get(2, 3) + tmp_20 * this->get(3, 3)));
	f32 out_22 = d * ((tmp_14 * this->get(0, 3) + tmp_19 * this->get(1, 3) + tmp_22 * this->get(3, 3)) - (tmp_15 * this->get(0, 3) + tmp_18 * this->get(1, 3) + tmp_23 * this->get(3, 3)));
	f32 out_23 = d * ((tmp_17 * this->get(0, 3) + tmp_20 * this->get(1, 3) + tmp_23 * this->get(2, 3)) - (tmp_16 * this->get(0, 3) + tmp_21 * this->get(1, 3) + tmp_22 * this->get(2, 3)));

	f32 out_30 = d * ((tmp_14 * this->get(2, 2) + tmp_17 * this->get(3, 2) + tmp_13 * this->get(1, 2)) - (tmp_16 * this->get(3, 2) + tmp_12 * this->get(1, 2) + tmp_15 * this->get(2, 2)));
	f32 out_31 = d * ((tmp_20 * this->get(3, 2) + tmp_12 * this->get(0, 2) + tmp_19 * this->get(2, 2)) - (tmp_18 * this->get(2, 2) + tmp_21 * this->get(3, 2) + tmp_13 * this->get(0, 2)));
	f32 out_32 = d * ((tmp_18 * this->get(1, 2) + tmp_23 * this->get(3, 2) + tmp_15 * this->get(0, 2)) - (tmp_22 * this->get(3, 2) + tmp_14 * this->get(0, 2) + tmp_19 * this->get(1, 2)));
	f32 out_33 = d * ((tmp_22 * this->get(2, 2) + tmp_16 * this->get(0, 2) + tmp_21 * this->get(1, 2)) - (tmp_20 * this->get(1, 2) + tmp_23 * this->get(2, 2) + tmp_17 * this->get(0, 2)));

	set(0, 0, out_00);
	set(0, 1, out_01);
	set(0, 2, out_02);
	set(0, 3, out_03);
	set(1, 0, out_10);
	set(1, 1, out_11);
	set(1, 2, out_12);
	set(1, 3, out_13);
	set(2, 0, out_20);
	set(2, 1, out_21);
	set(2, 2, out_22);
	set(2, 3, out_23);
	set(3, 0, out_30);
	set(3, 1, out_31);
	set(3, 2, out_32);
	set(3, 3, out_33);
}

void Matrix4::mul(const Matrix4& other)
{
	Matrix4 copy;
	copy.init((*this));

	this->setRows(0);

	FOR_RANGE(i, 0, 4)
	FOR_RANGE(j, 0, 4)
	FOR_RANGE(k, 0, 4)
	this->set(i, j, this->get(i, j) + copy.get(i, k) * other.get(k, j));
}

Vector4 Matrix4::mulVector(const Vector4& vector) const
{
	Vector4 result(0, 0, 0, 0);

	FOR_RANGE(row, 0, 4)
	{
		FOR_RANGE(col, 0, 4)
		{
			result[row] += this->get(row, col) * vector[col];
		}
	}

	return result;
}

void Matrix4::zeros()
{
	memset(mData, 0, 16 * sizeof(f32));
}

void Matrix4::identity()
{
	this->zeros();
	this->set(0, 0, 1.0f);
	this->set(1, 1, 1.0f);
	this->set(2, 2, 1.0f);
	this->set(3, 3, 1.0f);
}

void Matrix4::translation(const Vector3& vector)
{
	this->identity();
	this->set(0, 3, vector.x);
	this->set(1, 3, vector.y);
	this->set(2, 3, vector.z);
}

void Matrix4::rotation(const Vector3& vector)
{
	Quaternion q(vector);
	q.toMatrix(this);

	/*this->identity();

	f32 radians, cos, sin;

	if (vector.x != 0){
		radians = MathUtils::rad(vector.x);
		sin = sinf(radians);
		cos = cosf(radians);
		this->set(1, 1, cos);
		this->set(1, 2, -sin);
		this->set(2, 1, sin);
		this->set(2, 2, cos);
	} else if (vector.y != 0){
		radians = MathUtils::rad(vector.y);
		sin = sinf(radians);
		cos = cosf(radians);
		this->set(0, 0, cos);
		this->set(0, 2, sin);
		this->set(2, 0, -sin);
		this->set(2, 2, cos);
	} else if (vector.z != 0){
		radians = MathUtils::rad(vector.z);
		sin = sinf(radians);
		cos = cosf(radians);
		this->set(0, 0, cos);
		this->set(0, 1, -sin);
		this->set(1, 0, sin);
		this->set(1, 1, cos);
	}*/
}

void Matrix4::scale(const Vector3& vector)
{
	this->identity();
	this->set(0, 0, vector.x);
	this->set(1, 1, vector.y);
	this->set(2, 2, vector.z);
}

void Matrix4::ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
	this->identity();
	this->set(0, 0, 2.0f / (right - left));
	this->set(0, 3, -((right + left) / (right - left)));
	this->set(1, 1, 2.0f / (top - bottom));
	this->set(1, 3, -((top + bottom) / (top - bottom)));
	this->set(2, 2, -2.0f / (far - near));
	this->set(2, 3, -((far + near) / (far - near)));
}

void Matrix4::perspective(f32 near, f32 far, f32 aspect, f32 fov)
{
	f32 zRange = far - near;
	f32 tanHalfFOV = tanf(MathUtils::rad(fov / 2.0f));

	this->zeros();
	this->set(0, 0, 1.0f / (tanHalfFOV * aspect));
	this->set(1, 1, 1.0f / (tanHalfFOV));
	this->set(2, 2, -((near + far) / zRange));
	this->set(2, 3, -1.0f);
	this->set(3, 2, (2.0f * far * near) / zRange);
}
