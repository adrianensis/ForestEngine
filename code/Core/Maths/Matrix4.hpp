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
    void setRows(u32 n);
    void setRows(CR(Vector4) row0, CR(Vector4) row1, CR(Vector4) row2, CR(Vector4) row3);

public:

    static CR(Matrix4) getIdentity();
    void init(u32 n);
    void init(CR(Matrix4) other);
    void init(const std::array<f32, 16> &data);
    void init(const std::array<f32, 4> &row0, const std::array<f32, 4> &row1, const std::array<f32, 4> &row2, const std::array<f32, 4> &row3);
    void init(const f32 *data);
    void init(const f32 *row0, const f32 *row1, const f32 *row2, const f32 *row3);
    void init(CR(Vector4) row0, CR(Vector4) row1, CR(Vector4) row2, CR(Vector4) row3);

	const f32 *getData() const
	{
		return mData;
	}

    f32 get(u8 row, u8 col) const;
    void set(u8 row, u8 col, f32 value);
    void transpose();
    void invert();
    void mul(CR(Matrix4) other);
    Vector4 mulVector(CR(Vector4) vector) const;
    void zeros();
    void identity();
    void translation(CR(Vector3) vector);
    void rotation(CR(Vector3) vector);
    void scale(CR(Vector3) vector);
    void ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
    void perspective(f32 near, f32 far, f32 aspect, f32 fov);

public:
	const static u16 smColumnSize = 4;
	const static u16 smMatrixSize = smColumnSize * smColumnSize;
	
private:
	inline static bool smIdentityCreated = false;
	inline static OwnerPtr<Matrix4> smIdentity;
	f32 mData[smMatrixSize];
};

#endif