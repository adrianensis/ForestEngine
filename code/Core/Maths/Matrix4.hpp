#pragma once

#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector4.hpp"

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

 Matrix4 is COLUMN-MAJOR ORDER with ROW-MAJOR ORDER accessors!

 */

 NS_BEGIN(Maths)
class Matrix4
{
public:

	inline static constexpr const Core::u16 smColumnSize = 4;
	inline static constexpr const Core::u16 smMatrixSize = smColumnSize * smColumnSize;

    Matrix4();
    Matrix4(const std::array<Core::f32, smColumnSize> &row0, const std::array<Core::f32, smColumnSize> &row1, const std::array<Core::f32, smColumnSize> &row2, const std::array<Core::f32, smColumnSize> &row3);
    Matrix4(const Matrix4& other);
    void init(Core::f32 n);
    void init(const Matrix4& other);
    void init(const std::array<Core::f32, 16> &data);
    void init(const std::array<Core::f32, smColumnSize> &row0, const std::array<Core::f32, smColumnSize> &row1, const std::array<Core::f32, smColumnSize> &row2, const std::array<Core::f32, smColumnSize> &row3);
    void init(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);

	const Core::f32 *getData() const
	{
		return mData;
	}

    inline Core::f32 get(Core::u8 row, Core::u8 col) const
    {
        return mData[row + (smColumnSize * col)];
    }

    inline void set(Core::u8 row, Core::u8 col, Core::f32 value)
    {
        mData[row + (smColumnSize * col)] = value;
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
    void ortho(Core::f32 left, Core::f32 right, Core::f32 bottom, Core::f32 top, Core::f32 near, Core::f32 far);
    void perspective(Core::f32 near, Core::f32 far, Core::f32 aspect, Core::f32 fovy);
    void view(const Vector3& worldPosition, const Vector3& localRotation);
    void view(const Vector3& worldPosition, const Matrix4& localRotationMatrix);
    void lookAt(const Vector3& worldPosition, const Vector3& targetPosition);

    static Matrix4 transform(const Matrix4& translation, const Matrix4& rotation, const Matrix4& scale);

    Matrix4 operator=(const Matrix4& other)
    {
        if (this != &other)
        {
            init(other);
        }
        return *this;
    }

public:
	static const Matrix4 smIdentity;
	
private:
	Core::f32 mData[smMatrixSize];
};

inline const Matrix4 Matrix4::smIdentity = Matrix4({1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1});
NS_END