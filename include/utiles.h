/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * utiles used throughout the code.
 **/
#ifndef TSUNAMI_LAB_UTILES_H
#define TSUNAMI_LAB_UTILES_H

#include <memory>

namespace tsunami_lab
{

    /**
     * Returns an aligned array zero initalized
     *
     * @tparam T type of the array
     * @param rawPtr pointer to the nonaligned array for deletion
     * @param size size of the array to allocated
     * @param alignment the alignment of the array
     * @return the aligned array or on fail a nullptr
    */
    template<typename T>
    T* aligned_alloc( T*& rawPtr, size_t size, size_t alignment = alignof( T ) )
    {
        // calculates size of array with overhead for alignment
        size_t alignedSize = size + ( alignment / sizeof( T ) ) - 1;

        // init the array
        void* data = new T[alignedSize]{ 0 };
        rawPtr = static_cast<T*>( data );

        // prepare for align and align the array
        alignedSize *= sizeof( T ); // std::align works with size in bytes
        std::align( alignment, sizeof( T ), data, alignedSize );

        // convert the result T* and check if the array is large enough
        T* result = static_cast<T*>( data );
        if( alignedSize < ( size * sizeof( T ) ) )
        {
            delete[] result;
            return nullptr;
        }
        return result;
    }
}

#endif