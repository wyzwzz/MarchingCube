//
// Created by wyz on 2021/5/13.
//

#ifndef MARCHINGCUBES_LOOKUP_TABLE_HPP
#define MARCHINGCUBES_LOOKUP_TABLE_HPP
#include<array>
#include<cstdint>
#include<bitset>
#include<unordered_set>
#include<functional>
constexpr uint32_t square_case_0_idx  = 0x00000000;
constexpr uint32_t square_case_1_idx  = 0x01000000;
constexpr uint32_t square_case_2_idx  = 0x00000001;
constexpr uint32_t square_case_3_idx  = 0x00000100;
constexpr uint32_t square_case_4_idx  = 0x00010000;
constexpr uint32_t square_case_5_idx  = 0x01000001;
constexpr uint32_t square_case_6_idx  = 0x01010000;
constexpr uint32_t square_case_7_idx  = 0x01000100;
constexpr uint32_t square_case_8_idx  = 0x01010101;
constexpr uint32_t square_case_9_idx  = 0x00010101;
constexpr uint32_t square_case_10_idx = 0x01010100;
constexpr uint32_t square_case_11_idx = 0x01010001;
constexpr uint32_t square_case_12_idx = 0x01000101;
constexpr uint32_t square_case_13_idx = 0x00010100;
constexpr uint32_t square_case_14_idx = 0x00000101;
constexpr uint32_t square_case_15_idx = 0x00010001;

constexpr uint8_t cube_case_0 =0b00000000;

constexpr uint8_t cube_case_1 =0b01000000;

constexpr uint8_t cube_case_2a=0b10100000;
constexpr uint8_t cube_case_2b=0b01100000;
constexpr uint8_t cube_case_2c=0b00100100;

constexpr uint8_t cube_case_3a=0b01110000;
constexpr uint8_t cube_case_3b=0b10011000;
constexpr uint8_t cube_case_3c=0b01101000;

constexpr uint8_t cube_case_4a=0b11110000;
constexpr uint8_t cube_case_4b=0b11101000;
constexpr uint8_t cube_case_4c=0b01100110;
constexpr uint8_t cube_case_4d=0b10101100;
constexpr uint8_t cube_case_4e=0b00101101;
constexpr uint8_t cube_case_4f=0b10010110;
constexpr uint8_t cube_case_4g=0b01011100;

constexpr uint8_t cube_case_5a=0b10001111;
constexpr uint8_t cube_case_5b=0b01100111;
constexpr uint8_t cube_case_5c=0b10010111;

constexpr uint8_t cube_case_6a=0b01011111;
constexpr uint8_t cube_case_6b=0b10011111;
constexpr uint8_t cube_case_6c=0b11011011;

constexpr uint8_t cube_case_7 =0b10111111;

constexpr uint8_t cube_case_8 =0b11111111;

namespace mc{

    std::unordered_set<uint8_t> cube_lookup_table={
            cube_case_0,
            cube_case_1,
            cube_case_2a,cube_case_2b,cube_case_2c,
            cube_case_3a,cube_case_3b,cube_case_3c,
            cube_case_4a,cube_case_4b,cube_case_4c,cube_case_4d,cube_case_4e,cube_case_4f,cube_case_4g,
            cube_case_5a,cube_case_5b,cube_case_5c,
            cube_case_6a,cube_case_6b,cube_case_6c,
            cube_case_7,
            cube_case_8
    };

    template<class DTy,class T>
    class CubeCode{
    private:
        /**
         * @brief clockwise rotate 90 degree with z-axis
         */
        void rotate();
        void upToUp();
        void frontToUp();
        void backToUp();
        void leftToUp();
        void rightToUp();
        void bottomToUp();

        uint8_t coding() const;
    public:
        CubeCode(DTy value,const std::array<T,8>& cube):value(value),cube(cube),o_cube(cube){};

        uint8_t getCode(std::array<T,8>& values);

        DTy value;
        std::array<T,8> cube;
        std::array<T,8> o_cube;
    };

    template<class DTy,class T>
    inline uint8_t CubeCode<DTy,T>::coding() const {
        std::bitset<8> code;
        for(int i=0;i<8;i++){
            code.set(i,cube[i].value>value);
        }
        return code.to_ulong();
    }

    template<class DTy,class T>
    inline uint8_t CubeCode<DTy,T>::getCode(std::array<T,8>& values) {
        //up
        auto up_to_up=[&]()->void{upToUp();};
        auto front_to_up=[&]()->void{frontToUp();};
        auto back_to_up=[&]()->void{backToUp();};
        auto left_to_up=[&]()->void{leftToUp();};
        auto right_to_up=[&]()->void{rightToUp();};
        auto bottom_to_up=[&]()->void{bottomToUp();};
        std::vector<std::function<void()>> actions={up_to_up,front_to_up,back_to_up,left_to_up,right_to_up,bottom_to_up};

        for(auto& f:actions){
            f();
            for(int i=0;i<4;i++){
                auto code=coding();
                if(cube_lookup_table.count(code)==1){
                    values=cube;
                    return code;
                }
                rotate();
            }
        }
        std::cout<<"can't find case"<<std::endl;
        throw std::runtime_error("can't find case");
    }

    template<class DTy,class T>
    inline void CubeCode<DTy,T>::rotate() {
        auto t=cube[0];
        cube[0]=cube[2];
        cube[2]=cube[3];
        cube[3]=cube[1];
        cube[1]=t;
        t=cube[4];
        cube[4]=cube[6];
        cube[6]=cube[7];
        cube[7]=cube[5];
        cube[5]=t;
    }

    template<class DTy,class T>
    inline void CubeCode<DTy,T>::frontToUp() {
        cube=o_cube;
        auto t=cube[0];
        cube[0]=cube[4];
        cube[4]=cube[6];
        cube[6]=cube[2];
        cube[2]=t;
        t=cube[1];
        cube[1]=cube[5];
        cube[5]=cube[7];
        cube[7]=cube[3];
        cube[3]=t;
    }

    template<class DTy,class T>
    inline void CubeCode<DTy,T>::backToUp() {
        cube=o_cube;
        auto t=cube[0];
        cube[0]=cube[2];
        cube[2]=cube[6];
        cube[6]=cube[4];
        cube[4]=t;
        t=cube[1];
        cube[1]=cube[3];
        cube[3]=cube[7];
        cube[7]=cube[5];
        cube[5]=t;
    }

    template<class DTy,class T>
    inline void CubeCode<DTy,T>::leftToUp() {
        cube=o_cube;
        auto t=cube[0];
        cube[0]=cube[1];
        cube[1]=cube[5];
        cube[5]=cube[4];
        cube[4]=t;
        t=cube[2];
        cube[2]=cube[3];
        cube[3]=cube[7];
        cube[7]=cube[6];
        cube[6]=t;
    }

    template<class DTy,class T>
    inline void CubeCode<DTy,T>::rightToUp() {
        cube=o_cube;
        auto t=cube[0];
        cube[0]=cube[4];
        cube[4]=cube[5];
        cube[5]=cube[1];
        cube[1]=t;
        t=cube[2];
        cube[2]=cube[6];
        cube[6]=cube[7];
        cube[7]=cube[3];
        cube[3]=t;
    }

    template<class DTy,class T>
    inline void CubeCode<DTy,T>::bottomToUp() {
        cube=o_cube;
        std::swap(cube[0],cube[6]);
        std::swap(cube[1],cube[7]);
        std::swap(cube[2],cube[4]);
        std::swap(cube[3],cube[5]);
    }

    template<class DTy,class T>
    inline void CubeCode<DTy,T>::upToUp() {
        cube=o_cube;
    }


}


#endif //MARCHINGCUBES_LOOKUP_TABLE_HPP
