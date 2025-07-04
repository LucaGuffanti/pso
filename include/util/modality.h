#pragma once

#include <iostream>
// enum class Modality{
//     CPU,
//     GPU
// };

enum class Modality{CPU, GPU};

class CPUModality{
    public:
    int a;

    static void sayHello(){
        std::cout << "Culo" << std::endl; 
    }

    void ciao(){
        std::cout << "Ciao" << std::endl;
    }
};

class GPUModality{

    public:
    int a;

    static void sayHello(){
        std::cout << "Tette" << std::endl; 
    }

    void ciao(){
        std::cout << "Hello" << std::endl;
    }
};

