#include "Maths/Matrix4.hpp"
#include "Maths/MathUtils.hpp"
#include "Maths/Vector4.hpp"
#include "Maths/Vector3.hpp"
#include "Maths/Quaternion.hpp"

// #include <immintrin.h>

namespace Maths
{
Matrix4::Matrix4()
{
	zeros();
}

Matrix4::Matrix4(const Matrix4& other)
{
	init(other);
}

Matrix4::Matrix4(const std::array<f32, smColumnSize> &row0, const std::array<f32, smColumnSize> &row1, const std::array<f32, smColumnSize> &row2, const std::array<f32, smColumnSize> &row3)
{
	init(row0, row1, row2, row3);
}

void Matrix4::init(f32 n)
{
	std::memset(mData, n, sizeof(mData));
}

void Matrix4::init(const Matrix4& other)
{
    std::memcpy(mData, other.mData, sizeof(mData));
}

void Matrix4::init(const std::array<f32, smMatrixSize> &data)
{
    std::memcpy(mData, data.data(), sizeof(mData));
}

void Matrix4::init(const std::array<f32, smColumnSize> &row0, const std::array<f32, smColumnSize> &row1, const std::array<f32, smColumnSize> &row2, const std::array<f32, smColumnSize> &row3)
{
	const std::array<const std::array<f32, smColumnSize>*, smColumnSize> rows = {&row0, &row1, &row2, &row3};

	for(Maths::u32 row = 0; row < smColumnSize; ++row)
	for(Maths::u32 col = 0; col < smColumnSize; ++col)
	set(row, col, (*(rows[row]))[col]);
}

void Matrix4::init(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
{
	const std::array<const Vector4*,smColumnSize> rows = {&row0, &row1, &row2, &row3};

	for(Maths::u32 row = 0; row < smColumnSize; ++row)
	for(Maths::u32 col = 0; col < smColumnSize; ++col)
	set(row, col, (*(rows[row]))[col]);
}

void Matrix4::transpose()
{
	Matrix4 copy;
	copy.init((*this));

	for(Maths::u32 row = 0; row < smColumnSize; ++row)
	for(Maths::u32 col = 0; col < smColumnSize; ++col)
	this->set(col, row, copy.get(row, col));
}

void Matrix4::invert()
{
    f32 n11 = (*this).get(0,0), n12 = (*this).get(1,0), n13 = (*this).get(2,0), n14 = (*this).get(3,0);
    f32 n21 = (*this).get(0,1), n22 = (*this).get(1,1), n23 = (*this).get(2,1), n24 = (*this).get(3,1);
    f32 n31 = (*this).get(0,2), n32 = (*this).get(1,2), n33 = (*this).get(2,2), n34 = (*this).get(3,2);
    f32 n41 = (*this).get(0,3), n42 = (*this).get(1,3), n43 = (*this).get(2,3), n44 = (*this).get(3,3);

    f32 t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    f32 t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    f32 t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    f32 t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    f32 det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    f32 idet = 1.0f / det;

    this->set(0, 0, t11 * idet);
    this->set(0, 1, (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet);
    this->set(0, 2, (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet);
    this->set(0, 3, (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet);

    this->set(1, 0, t12 * idet);
    this->set(1, 1, (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet);
    this->set(1, 2, (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet);
    this->set(1, 3, (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet);

    this->set(2, 0, t13 * idet);
    this->set(2, 1, (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet);
    this->set(2, 2, (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet);
    this->set(2, 3, (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet);

    this->set(3, 0, t14 * idet);
    this->set(3, 1, (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet);
    this->set(3, 2, (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet);
    this->set(3, 3, (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet);

	// f32 tmp_0 = this->get(2, 2) * this->get(3, 3);
	// f32 tmp_1 = this->get(3, 2) * this->get(2, 3);
	// f32 tmp_2 = this->get(1, 2) * this->get(3, 3);
	// f32 tmp_3 = this->get(3, 2) * this->get(1, 3);
	// f32 tmp_4 = this->get(1, 2) * this->get(2, 3);
	// f32 tmp_5 = this->get(2, 2) * this->get(1, 3);
	// f32 tmp_6 = this->get(0, 2) * this->get(3, 3);
	// f32 tmp_7 = this->get(3, 2) * this->get(0, 3);
	// f32 tmp_8 = this->get(0, 2) * this->get(2, 3);
	// f32 tmp_9 = this->get(2, 2) * this->get(0, 3);
	// f32 tmp_10 = this->get(0, 2) * this->get(1, 3);
	// f32 tmp_11 = this->get(1, 2) * this->get(0, 3);
	// f32 tmp_12 = this->get(2, 0) * this->get(3, 1);
	// f32 tmp_13 = this->get(3, 0) * this->get(2, 1);
	// f32 tmp_14 = this->get(1, 0) * this->get(3, 1);
	// f32 tmp_15 = this->get(3, 0) * this->get(1, 1);
	// f32 tmp_16 = this->get(1, 0) * this->get(2, 1);
	// f32 tmp_17 = this->get(2, 0) * this->get(1, 1);
	// f32 tmp_18 = this->get(0, 0) * this->get(3, 1);
	// f32 tmp_19 = this->get(3, 0) * this->get(0, 1);
	// f32 tmp_20 = this->get(0, 0) * this->get(2, 1);
	// f32 tmp_21 = this->get(2, 0) * this->get(0, 1);
	// f32 tmp_22 = this->get(0, 0) * this->get(1, 1);
	// f32 tmp_23 = this->get(1, 0) * this->get(0, 1);

	// f32 t0 = ((tmp_0 * this->get(1, 1) + tmp_3 * this->get(2, 1) + tmp_4 * this->get(3, 1)) - (tmp_1 * this->get(1, 1) + tmp_2 * this->get(2, 1) + tmp_5 * this->get(3, 1)));
	// f32 t1 = ((tmp_1 * this->get(0, 1) + tmp_6 * this->get(2, 1) + tmp_9 * this->get(3, 1)) - (tmp_0 * this->get(0, 1) + tmp_7 * this->get(2, 1) + tmp_8 * this->get(3, 1)));
	// f32 t2 = ((tmp_2 * this->get(0, 1) + tmp_7 * this->get(1, 1) + tmp_10 * this->get(3, 1)) - (tmp_3 * this->get(0, 1) + tmp_6 * this->get(1, 1) + tmp_11 * this->get(3, 1)));
	// f32 t3 = ((tmp_5 * this->get(0, 1) + tmp_8 * this->get(1, 1) + tmp_11 * this->get(2, 1)) - (tmp_4 * this->get(0, 1) + tmp_9 * this->get(1, 1) + tmp_10 * this->get(2, 1)));

	// f32 d = 1.0 / (this->get(0, 0) * t0 + this->get(1, 0) * t1 + this->get(2, 0) * t2 + this->get(3, 0) * t3);

	// f32 out_00 = d * t0;
	// f32 out_01 = d * t1;
	// f32 out_02 = d * t2;
	// f32 out_03 = d * t3;

	// f32 out_10 = d * ((tmp_1 * this->get(1, 0) + tmp_2 * this->get(2, 0) + tmp_5 * this->get(3, 0)) - (tmp_0 * this->get(1, 0) + tmp_3 * this->get(2, 0) + tmp_4 * this->get(3, 0)));
	// f32 out_11 = d * ((tmp_0 * this->get(0, 0) + tmp_7 * this->get(2, 0) + tmp_8 * this->get(3, 0)) - (tmp_1 * this->get(0, 0) + tmp_6 * this->get(2, 0) + tmp_9 * this->get(3, 0)));
	// f32 out_12 = d * ((tmp_3 * this->get(0, 0) + tmp_6 * this->get(1, 0) + tmp_11 * this->get(3, 0)) - (tmp_2 * this->get(0, 0) + tmp_7 * this->get(1, 0) + tmp_10 * this->get(3, 0)));
	// f32 out_13 = d * ((tmp_4 * this->get(0, 0) + tmp_9 * this->get(1, 0) + tmp_10 * this->get(2, 0)) - (tmp_5 * this->get(0, 0) + tmp_8 * this->get(1, 0) + tmp_11 * this->get(2, 0)));

	// f32 out_20 = d * ((tmp_12 * this->get(1, 3) + tmp_15 * this->get(2, 3) + tmp_16 * this->get(3, 3)) - (tmp_13 * this->get(1, 3) + tmp_14 * this->get(2, 3) + tmp_17 * this->get(3, 3)));
	// f32 out_21 = d * ((tmp_13 * this->get(0, 3) + tmp_18 * this->get(2, 3) + tmp_21 * this->get(3, 3)) - (tmp_12 * this->get(0, 3) + tmp_19 * this->get(2, 3) + tmp_20 * this->get(3, 3)));
	// f32 out_22 = d * ((tmp_14 * this->get(0, 3) + tmp_19 * this->get(1, 3) + tmp_22 * this->get(3, 3)) - (tmp_15 * this->get(0, 3) + tmp_18 * this->get(1, 3) + tmp_23 * this->get(3, 3)));
	// f32 out_23 = d * ((tmp_17 * this->get(0, 3) + tmp_20 * this->get(1, 3) + tmp_23 * this->get(2, 3)) - (tmp_16 * this->get(0, 3) + tmp_21 * this->get(1, 3) + tmp_22 * this->get(2, 3)));

	// f32 out_30 = d * ((tmp_14 * this->get(2, 2) + tmp_17 * this->get(3, 2) + tmp_13 * this->get(1, 2)) - (tmp_16 * this->get(3, 2) + tmp_12 * this->get(1, 2) + tmp_15 * this->get(2, 2)));
	// f32 out_31 = d * ((tmp_20 * this->get(3, 2) + tmp_12 * this->get(0, 2) + tmp_19 * this->get(2, 2)) - (tmp_18 * this->get(2, 2) + tmp_21 * this->get(3, 2) + tmp_13 * this->get(0, 2)));
	// f32 out_32 = d * ((tmp_18 * this->get(1, 2) + tmp_23 * this->get(3, 2) + tmp_15 * this->get(0, 2)) - (tmp_22 * this->get(3, 2) + tmp_14 * this->get(0, 2) + tmp_19 * this->get(1, 2)));
	// f32 out_33 = d * ((tmp_22 * this->get(2, 2) + tmp_16 * this->get(0, 2) + tmp_21 * this->get(1, 2)) - (tmp_20 * this->get(1, 2) + tmp_23 * this->get(2, 2) + tmp_17 * this->get(0, 2)));

	// set(0, 0, out_00);
	// set(0, 1, out_01);
	// set(0, 2, out_02);
	// set(0, 3, out_03);
	// set(1, 0, out_10);
	// set(1, 1, out_11);
	// set(1, 2, out_12);
	// set(1, 3, out_13);
	// set(2, 0, out_20);
	// set(2, 1, out_21);
	// set(2, 2, out_22);
	// set(2, 3, out_23);
	// set(3, 0, out_30);
	// set(3, 1, out_31);
	// set(3, 2, out_32);
	// set(3, 3, out_33);
}

void Matrix4::mul(const Matrix4& other)
{

	Matrix4 copy;
	copy.init((*this));

	this->init(0);

	for(Maths::u32 i = 0; i < smColumnSize; ++i)
	for(Maths::u32 j = 0; j < smColumnSize; ++j)
	for(Maths::u32 k = 0; k < smColumnSize; ++k)
	this->set(i, j, get(i, j) + copy.get(i, k) * other.get(k, j));

	// const float* A = copy.getData();
    // const float* B = other.getData();
    // float* C = this->getData(); // Result C

	// // Load the four columns of A into SSE registers.
    // // Since they are contiguous, we can use aligned loads.
    // __m128 colA0 = _mm_load_ps(&A[0]);
    // __m128 colA1 = _mm_load_ps(&A[4]);
    // __m128 colA2 = _mm_load_ps(&A[8]);
    // __m128 colA3 = _mm_load_ps(&A[12]);

    // // Calculate each column of C (C_j = A_0*B_0j + A_1*B_1j + A_2*B_2j + A_3*B_3j)
    // for (int j = 0; j < 4; ++j) 
    // {
    //     // Broadcast the scalar elements B_kj for the j-th column of C
    //     // Note: B is column-major. B[k * 4 + j] is the element B_jk.
    //     // We need B[j * 4 + k] for the element B_kj.
        
    //     // B_0j is B[j*4 + 0], B_1j is B[j*4 + 1], etc.
        
    //     // 1. Calculate A_0 * B_0j
    //     __m128 B0j = _mm_set1_ps(B[j * 4 + 0]);
    //     __m128 temp0 = _mm_mul_ps(colA0, B0j);

    //     // 2. Calculate A_1 * B_1j and add
    //     __m128 B1j = _mm_set1_ps(B[j * 4 + 1]);
    //     __m128 colCj = _mm_add_ps(temp0, _mm_mul_ps(colA1, B1j));

    //     // 3. Calculate A_2 * B_2j and add
    //     __m128 B2j = _mm_set1_ps(B[j * 4 + 2]);
    //     colCj = _mm_add_ps(colCj, _mm_mul_ps(colA2, B2j));

    //     // 4. Calculate A_3 * B_3j and add
    //     __m128 B3j = _mm_set1_ps(B[j * 4 + 3]);
    //     colCj = _mm_add_ps(colCj, _mm_mul_ps(colA3, B3j));

    //     // Store the resulting column of C back into memory
    //     _mm_store_ps(&C[j * 4], colCj);
    // }
}

Vector4 Matrix4::mulVector(const Vector4& vector) const
{
	Vector4 result(0, 0, 0, 0);

	for(Maths::u32 row = 0; row < smColumnSize; ++row)
	{
		const f32 a = get(row, 0) * vector[0];
        const f32 b = get(row, 1) * vector[1];
        const f32 c = get(row, 2) * vector[2];
        const f32 d = get(row, 3) * vector[3];
        result[row] = a + b + c + d;
	}

	return result;
}

void Matrix4::zeros()
{
	init(0);
}

void Matrix4::identity()
{
	init(smIdentity);
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
	q.toMatrix(*this);
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
	// INFO: Right-Handed (+Z going out screen), NDC Z Zero-To-One clamped

	this->identity();
	this->set(0, 0, 2.0f / (right - left));
	this->set(1, 1, 2.0f / (top - bottom));
	this->set(2, 2, -1.0f / (far - near));
	this->set(0, 3, -(right + left) / (right - left));
	this->set(1, 3, -( top + bottom) / ( top - bottom));
	this->set(2, 3, -(far + near) / (far - near));
}

void Matrix4::perspective(f32 near, f32 far, f32 aspect, f32 fovy)
{
	// INFO: Right-Handed (+Z going out screen), NDC Z Zero-To-One clamped

	f32 tanHalfFovy = tanf(MathUtils::rad(fovy) / 2.0f);

	this->zeros();
	this->set(0, 0, 1.0f/(aspect * tanHalfFovy));
	this->set(1, 1, 1.0f/tanHalfFovy);
	this->set(2, 2, far / (near - far));
    this->set(2, 3, -(far * near) / (far - near));
	this->set(3, 2,-1.0f);
}

void Matrix4::view(const Vector3& worldPosition, const Vector3& localRotation)
{
	Matrix4 rotationMatrix;
    rotationMatrix.rotation(localRotation);
    
    view(worldPosition, rotationMatrix);
}

void Matrix4::view(const Vector3& worldPosition, const Matrix4& localRotationMatrix)
{
	this->identity();

    Matrix4 viewTranslationMatrix;
	viewTranslationMatrix.translation(-worldPosition);

	this->mul(localRotationMatrix);
	this->mul(viewTranslationMatrix);
}

void Matrix4::lookAt(const Vector3& worldPosition, const Vector3& targetPosition)
{
	Vector3 target(targetPosition);

	Vector3 forward(target.sub(worldPosition).nor());
	Vector3 yAxis = Vector3::smUp;

	Vector3 right(Vector3(forward).cross(yAxis));
    right.nor();
    if(right.len() == 0.0f)
    {
        f32 dot = forward.dot(yAxis);
        if(dot > 0)
        {
            // parallel
            right = -Vector3::smRight;
        }
        else
        {
            // antiparallel
            right = Vector3::smRight;
        }
    }

	Vector3 up(Vector3(right).cross(forward));

	this->init(
        Vector4(right.x, right.y, right.z, 0),
        Vector4(up.x, up.y, up.z, 0),
		Vector4(forward.x, forward.y, forward.z, 0),
        Vector4(0, 0, 0, 1)
    );

	// Quaternion q;
	// q.fromMatrix(lookAtMatrix);
	// mLocalRotation = q.toEuler();
}

Matrix4 Matrix4::transform(const Matrix4& translation, const Matrix4& rotation, const Matrix4& scale)
{
    Matrix4 translationCopy = translation;
    Matrix4 rotationCopy = rotation;
    rotationCopy.mul(scale);
    translationCopy.mul(rotationCopy);
    return translationCopy;
}

};