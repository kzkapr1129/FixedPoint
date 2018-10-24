/**
 * bitの足し算、桁上がりを利用してbitをカウントする
 */
uint8_t bit_count(uint32_t data) {
    // 1ビット単位でビットをカウントする -> 2ビットおきにカウントされたビット数が格納されている
    data = (data & 0x55555555) + ((data >> 1) & 0x55555555);
    
    // 2ビット単位でビットをカウントする -> 4ビットおきにカウントされたビット数が格納されている
    data = (data & 0x33333333) + ((data >> 2) & 0x33333333);
    
    // 4ビット単位でビットをカウントする -> 8ビットおきにカウントされたビット数が格納されている
    data = (data & 0x0F0F0F0F) + ((data >> 4) & 0x0F0F0F0F);
    
    // 8ビット単位でビットをカウントする -> 16ビットおきにカウントされたビット数が格納されている
    data = (data & 0x00FF00FF) + ((data >> 8) & 0x00FF00FF);
    
    // (最後)
    // 16ビット単位でビットをカウントする -> 32ビット起きにカウントされたビット数が格納されている
    data = (data & 0x0000FFFF) + ((data >>16) & 0x0000FFFF);
    return data;
}

/**
 * 二分探索で上位ビットの位置を求める
 */
unsigned int nlz(uint32_t data) {
    data = data & 0xFFFF0000 ? data & 0xFFFF0000 : data;
    data = data & 0xFF00FF00 ? data & 0xFF00FF00 : data;
    data = data & 0xF0F0F0F0 ? data & 0xF0F0F0F0 : data;
    data = data & 0xCCCCCCCC ? data & 0xCCCCCCCC : data;
    data = data & 0xAAAAAAAA ? data & 0xAAAAAAAA : data;
    
    return bit_count((data & (-data)) - 1);
}

/*
 * 符号・小数点の桁数・仮数から浮動小数点(float型)に変換する。
 * 以下のbit構成に従い変換を行う。
 *  31bit: 符号   (1bit)
 *  30-23bit: 指数部 (8bit)
 *  22- 0bit: 仮数部 (23bit)
 */
union Float {

    // 符号・小数点の桁数・仮数を設定しfloat型い変換する
    void setBits(int sign, int dec_point_pos/*下位ビット数*/, int kari_suu) {
        if (kari_suu == 0) {
            bits = 0; // 仮数が0の場合は0
            return;
        }
        
        // 符号ビットを設定する
        if (sign) {
            bits = 0x80000000;
        } else {
            bits = 0x00000000;
        }
        
        // 仮数の上位ビットから1が現れるまでのbit数を数える
        int n = 23 - nlz(kari_suu);
        
        // 仮数部を設定する
        bits |= ((kari_suu << n) & 0x007FFFFF);
        
        // 指数を求める
        int sisuu = 23 - n - dec_point_pos;
        
        // 新しい指数部のビットを計算
        int newBits = (sisuu + 127)<< 23;
        
        // 指数部の適用
        bits |= (newBits & 0x7F800000);
        
    }
    
    // ビット列
    int   bits;
    
    // 浮動小数点
    float value;
    
    // コンストラクタ
    Float() : value(0) {}
};


template<int SHIFT>
struct FixedPoint {
    uint32_t bits;
    
    inline FixedPoint(float data) {
        bits = round(data * (1 << SHIFT));
    }
    
    inline const FixedPoint<SHIFT>& operator += (int data) {
        bits += (data << SHIFT);
        return *this;
    }
    
    inline const FixedPoint<SHIFT>& operator -= (int data) {
        bits -= (data << SHIFT);
        return *this;
    }
    
    inline const FixedPoint<SHIFT>& operator += (const FixedPoint<SHIFT>& data) {
        bits += data.bits;
        return *this;
    }
    
    inline const FixedPoint<SHIFT>& operator -= (const FixedPoint<SHIFT>& data) {
        bits -= data.bits;
        return *this;
    }
    
    inline const FixedPoint<SHIFT>& operator *= (int data) {
        bits *= data;
        return *this;
    }
    
    inline const FixedPoint<SHIFT>& operator *= (const FixedPoint<SHIFT>& data) {
        bits *= data.bits;
        bits >>= SHIFT;
        return *this;
    }
};