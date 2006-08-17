//  
// Copyright (C) 2006 SIPfoundry Inc. 
// Licensed by SIPfoundry under the LGPL license. 
//  
// Copyright (C) 2006 SIPez LLC. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//  
// $$ 
////////////////////////////////////////////////////////////////////////////// 

#ifndef _INCLUDED_MPBUF_H // [
#define _INCLUDED_MPBUF_H

/**
 *  @todo cache align - align pool begining, buffer sizes (must be multiple of
 *        the cache size) and data begining in MpArrayBuf.
 *  @todo error handling - return OsStatus may be?
 *  @todo MpBufPtr::requestWrite() - create new buffer if MpBufPtr::isWritable()
 *        return false. 
 *  @todo defines for most reused code - MpBufPtr parts, for example.
 */

// SYSTEM INCLUDES
#include <stdlib.h>
#include <assert.h>

// APPLICATION INCLUDES
#include "mp/MpBufPool.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS

/// Enum used for runtime type checks.
/**
*  This enum is used to determine the real type of the buffer. Also this helps
*  us to decide is it possible to convert from one buffer to other.
*  E.g. MpAudioBuf could be converted to MpDataBuf, but could not be converted
*  to MpArrayBuf. This check is done inside MpBufPtr's child classes.
*/
enum MP_BUFFERS_TREE {
    MP_BUF,               ///< Begin of the MpBuf type
      MP_BUF_ARRAY,       ///< Begin of the MpArrayBuf type
      MP_BUF_ARRAY_END,   ///< End of the MpArrayBuf type
      MP_BUF_DATA,        ///< Begin of the MpDataBuf type
        MP_BUF_AUDIO,     ///< Begin of the MpAudioBuf type
        MP_BUF_AUDIO_END, ///< End of the MpAudioBuf type
        MP_BUF_RTP,       ///< Begin of the MpRtpBuf type
        MP_BUF_RTP_END,   ///< End of the MpRtpBuf type
        MP_BUF_UDP,       ///< Begin of the MpUdpBuf type
        MP_BUF_UDP_END,   ///< End of the MpUdpBuf type
      MP_BUF_DATA_END,    ///< End of the MpDataBuf type
    MP_BUF_END            ///< End of the MpBuf type
};

// TYPEDEFS

///  Base class for all media buffers.
/**
*  This class designed to be used with memory pool MpBufPool and smart pointer
*  MpBufPoolPtr. It seems to be useless without them.
*  
*  This struct itself does not provide any data storage. It contain only
*  reference counter and pointer to parent pool. To store data in this pool
*  use MpArrayBuf. To store store data in the other pool use MpDataBuf or
*  one of its children.
*/
struct MpBuf {
    friend class MpBufPtr;
    friend class MpBufPool;

/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:
/* ============================ CREATORS ================================== */
///@name Creators
//@{


//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

    /// Increments reference counter.
    void incrementRefCounter();

    /// Decrements reference counter and free buffer if needed.
    void decrementRefCounter();

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

    /// Get buffer type.
    MP_BUFFERS_TREE getType() {return mType;};

    /// Get parent pool of this buffer.
    MpBufPool *getBufferPool() {return mpPool;};

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

    /// You should write to the buffer if and only if this function return true.
    /**
    * isWritable() check are you the only owner of this buffer.
    */
    bool isWritable() {return (mRefCounter == 1);};

//@}

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

    MP_BUFFERS_TREE mType;     ///< Buffer class type. Used for type safety.
    int mRefCounter;           ///< Reference counter for use with MpBufPtr.
    MpBufPool* mpPool;         ///< Parent memory pool.
    void (*mpDestroy)(MpBuf*); ///< Pointer to deinitialization method. Used as
                               ///<  virtual destructor.

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

    /// Disable copy (and other) constructor.
    MpBuf(const MpBuf &);
    /**<
    * This struct will be initialized by init() member.
    */

    /// Disable assignment operator.
    MpBuf &operator=(const MpBuf &);
    /**<
    * Buffers may be copied. But do we need this?
    */
};

///  Smart pointer to MpBuf.
/**
*  You should only use this smart pointer, not #MpBuf* itself.
*  The goal of this smart pointer is to care about reference counter and
*  buffer deallocation.
*/
class MpBufPtr {
    friend class MpBufPool;

/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{

    /// Default constructor - construct invalid pointer.
    MpBufPtr()
        : mpBuffer(NULL)
    {};

    /// This constructor owns MpBuf object.
    MpBufPtr(MpBuf *buffer)
        : mpBuffer(buffer)
    {
        if (mpBuffer != NULL) {
            mpBuffer->mType = MP_BUF;
            mpBuffer->mpDestroy = NULL;
            mpBuffer->incrementRefCounter();
        }
#ifdef _DEBUG
        else {
            osPrintf("mpBuffer == NULL!\n");
        }
#endif
    };

    /// Destructor. It decrements buffer's reference counter.
    ~MpBufPtr()
    {
        if (mpBuffer != NULL)
            mpBuffer->decrementRefCounter();
    };

    /// Copy buffer pointer and increment its reference counter.
    MpBufPtr(const MpBufPtr &buffer) 
        : mpBuffer(buffer.mpBuffer)
    {
        if (mpBuffer != NULL)
            mpBuffer->incrementRefCounter();
    }

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{

    /// Smart assignment.
    /**
    * Decrement reference counter of our old buffer and increment in new one.
    */
    MpBufPtr &operator=(const MpBufPtr &bufferPtr)
    {
        // Check for a=a case;
        if (&bufferPtr == this) {
            return *this;
        }

        if (mpBuffer != NULL)
            mpBuffer->decrementRefCounter();
        mpBuffer = bufferPtr.mpBuffer;
        if (mpBuffer != NULL)
            mpBuffer->incrementRefCounter();

        return *this;
    }

    /// Compare two smart pointers
    /**
    *  Two pointers assumed equal if they point to the same buffer.
    */
    bool operator==(const MpBufPtr &pBuffer)
    {
        return (mpBuffer == pBuffer.mpBuffer);
    }

    /// Compare two smart pointers
    /**
    *  Two pointers assumed equal if they point to the same buffer.
    */
    bool operator!=(const MpBufPtr &pBuffer)
    {
        return (mpBuffer != pBuffer.mpBuffer);
    }

    /// Release buffer we are pointing to.
    /**
    *  If no one else is pointing to this buffer it will be freed.
    */
    void release()
    {
        if (mpBuffer != NULL)
            mpBuffer->decrementRefCounter();
        mpBuffer = NULL;
    }

    /// Swap to buffers.
    /**
    *  This ponter will point to buffer pointed by pBuffer, and pBuffer will
    *  point to buffer, pointed by this pointer. Swap does not modify reference
    *  counters of buffers.
    */
    void swap(MpBufPtr &pBuffer)
    {
        MpBuf *temp = pBuffer.mpBuffer;
        pBuffer.mpBuffer = mpBuffer;
        mpBuffer = temp;
    }

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{

    /// Return number of the buffer in the pool. Use this for debug output.
    int getBufferNumber() const
    {
        if (mpBuffer == NULL)
            return -1;
        else
            return mpBuffer->mpPool->getBufferNumber(mpBuffer);
    };

    /// Return pointer to MpBuf.
    MpBuf *operator->() {assert(mpBuffer!=NULL); return mpBuffer;};

    /// Return readonly pointer to MpBuf.
    const MpBuf *operator->() const {assert(mpBuffer!=NULL); return mpBuffer;};

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

    /// @brief Can this pointer be dereferenced? Use this function instead of
    /// NULL comparison.
    bool isValid() const {return mpBuffer != NULL;};

//@}

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

    MpBuf *mpBuffer;  ///< Pointer to real buffer.

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

};

/// Default constructor - construct invalid pointer.
#define MPBUF_DEFAULT_CONSTRUCTOR(classname)                                \
    classname##Ptr() {};

#define MPBUF_FROM_BASE_CONSTRUCTOR_INIT(classname, buffer_type)            \
            classname *pBuffer = (classname*)mpBuffer;                      \
            assert(pBuffer->mpPool->getBlockSize() >= sizeof(classname));   \
            pBuffer->mType = buffer_type;                                   \
            pBuffer->init();                                                \

/// This constructor owns MpBuf object.
#define MPBUF_FROM_BASE_CONSTRUCTOR(classname, buffer_type, base_classname) \
    classname##Ptr(MpBuf *buffer)                                           \
        : base_classname##Ptr(buffer)                                       \
    {                                                                       \
        if (mpBuffer != NULL) {                                             \
            MPBUF_FROM_BASE_CONSTRUCTOR_INIT(classname, buffer_type)        \
        }                                                                   \
    };

/// Copy object from base type with type check.
#define MPBUF_TYPECHECKED_COPY(classname, buffer_type, base_classname)      \
    classname##Ptr(MpBufPtr &buffer)                                        \
        : base_classname##Ptr(buffer)                                       \
    {                                                                       \
        assert( (!buffer.isValid())                                         \
              || (  buffer->getType() >= buffer_type                        \
                 && buffer->getType() < buffer_type##_END));                \
    };

/// Return pointer to buffer.
#define MPBUF_MEMBER_ACCESS_OPERATOR(classname)                             \
   classname *operator->()                                                  \
      {assert(mpBuffer!=NULL); return (classname*)mpBuffer;};

/// Return readonly pointer to buffer.
#define MPBUF_CONST_MEMBER_ACCESS_OPERATOR(classname)                       \
   const classname *operator->() const                                      \
      {assert(mpBuffer!=NULL); return (classname*)mpBuffer;};

#endif // _INCLUDED_MPBUF_H  ]