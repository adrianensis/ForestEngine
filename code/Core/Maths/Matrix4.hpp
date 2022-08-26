#ifndef MATRIX4_HPP
#define MATRIX4_HPP

#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector4.hpp"
#include "Core/Pointers.hpp"





/*
 ROW-MAJOR ORDER:

 row 0 | a b c d |
 row 1 | e f g h |
 row 2 | i j k l |
 row 3 | m n o p |

 COLUMN-MAJOR ORDER:

 column 0 | a e i m |
 column 1 | b f j n |
 column 2 | c g k o |
 column 3 | d h l p |

 */

class Matrix4
{
    GENERATE_METADATA_STRUCT(Matrix4);
	
private:
    void setRows(const std::array<f32, 4> &row0, const std::array<f32, 4> &row1, const std::array<f32, 4> &row2, const std::array<f32, 4> &row3);
    void setRows(const f32 *row0, const f32 *row1, const f32 *row2, const f32 *row3);
    void setRows(f32 n);
    void setRows(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);

public:

    Matrix4();
    Matrix4(const Matrix4& other);
    static const Matrix4& getIdentity();
    void init(f32 n);
    void init(const Matrix4& other);
    void init(const std::array<f32, 16> &data);
    void init(const std::array<f32, 4> &row0, const std::array<f32, 4> &row1, const std::array<f32, 4> &row2, const std::array<f32, 4> &row3);
    void init(const f32 *data);
    void init(const f32 *row0, const f32 *row1, const f32 *row2, const f32 *row3);
    void init(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);

	const f32 *getData() const
	{
		return mData;
	}

    inline f32 get(u8 row, u8 col) const
    {
        return mData[row + (4 * col)];
    }

    inline void set(u8 row, u8 col, f32 value)
    {
        mData[row + (4 * col)] = value;
    }
    
    void transpose();
    void invert();
    void mul(const Matrix4& other);
    Vector4 mulVector(const Vector4& vector) const;
    void zeros();
    void identity();
    void translation(const Vector3& vector);
    void rotation(const Vector3& vector);
    void scale(const Vector3& vector);
    void ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
    void perspective(f32 near, f32 far, f32 aspect, f32 fov);

    Matrix4& operator=(const Matrix4& other)
	{
		init(other);
		return *this;
	}

public:
	const static u16 smColumnSize = 4;
	const static u16 smMatrixSize = smColumnSize * smColumnSize;
	
private:
	inline static bool smIdentityCreated = false;
	inline static OwnerPtr<Matrix4> smIdentity;
	f32 mData[smMatrixSize];
};

#endif