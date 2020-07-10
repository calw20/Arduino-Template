/*
Cal.W 2020
Common / Nice to have c++ data types / structs / unions
Licensed under MIT, (Appended to the end of the file)
//https://github.com/calw20/Arduino-Template/
*/

#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

#include "Arduino.h"


//Nifty little union TEMPLATE!!! allowing for a float like value but happends to be two!
//Basicity it remembers the current and previous values.
template <typename T> 
union StoredData {
    struct {
        T current;
        T previous;
    };
    T val[2];
    
    
    //Direct access to the inbuilt array
    T& operator[](int i) {
        return val[i];
    }

    
    //Move the value to previous via an assignment
    StoredData& operator=(T i){
        previous = current;
        current = i;
        return *this;
    }

    //Have the option to override/copy another StoredData
    StoredData& operator=(StoredData sd){
        current = sd.current;
        previous = sd.previous;
        return *this;
    }
};

typedef StoredData<float> fStoreData;
typedef StoredData<int> iStoreData;


//2D Vectors
template <typename T>
union v2d {
    struct {
        T x;
        T y;
    };
    T var[2];

    T& operator[](int i) {
        return val[i];
    }

    v2d& operator=(v2d vec){
        x = vec.x;
        y = vec.y;
        return *this;
    }
};

typedef v2d<float> fv2d; 
typedef v2d<int> iv2d;


//3D Vectors
template <typename T>
union v3d {
    struct {
        T x;
        T y;
        T z;
    };
    T var[3];

    T& operator[](int i) {
        return val[i];
    }

    v2d& operator=(v2d vec){
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }
};

typedef v3d<float> fv3d; 
typedef v3d<int> iv3d;


/* LICENCE
MIT License

Copyright (c) 2020 Cal W

TL;DR: The legal babble is below (it's what applies), this is a quick note to you!
Do what you want with this project. I made it mostly to further my knowledge in
its respective felids, so it is most likely going to be a jumbled, barely
functional, glob of what I hope can be called code. Hence I can't promise it
wont break, cry, give you up or create a rift in time and space. Sorry.
So if you do end up using this, please let me know as I would love to see how its
used. Now if for some reason you feel like this has helped you in some useful way
and you manage to spot me out in the world, feel free to buy me a drink (coffee or
beer is always nice) but I highly doubt this will have been useful enough for that.
Enjoy and good luck!


============================== The Legal Babble ===============================

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#endif
