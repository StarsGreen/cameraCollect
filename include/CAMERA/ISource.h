#ifndef LIBCAMERA_ISOURCE_H
#define LIBCAMERA_ISOURCE_H

#include <stdint.h>
#include "IVAHelper.h"

typedef struct FormatDesc{
    int         format; //v4l2 format
    int         width;
    int         height;
    int         fps_num;
    int         fps_den;
} FormatDesc;

class ISource
{
public:
    class ISourceListener
    {
    public:
        virtual ~ISourceListener(){};

        /**
         * @brief after ISource output format decided (typically after function open successfully invoked),
         * onFormat notify listener ISource output buffer FormatDesc desc, which is shouldn't equal to NULL,
         * when listener was removed, onFormat will notify listener with NULL FormatDesc desc.
         * when ISource destructed, will remove all listeners
         *
         * @param desc ISource output buffer format description, it same with onFrameAvaible buffer
         * @param source which ISource the listener was add to
         */
        virtual void onFormat(FormatDesc *desc, ISource *source) = 0;

        /**
         * @brief after start function invoked, onFrameAvaible will be callback
         * after stop function invoked, onFrameAvaible will never be callback
         * onFrameAvaible notify listener ISource with new buffer,
         *
         * TAKE CARE: this buffer ALWAYS BELONG TO ISource, NEVER RELEASE OR CHANGE it
         *
         * @param buffer output buffer, listener should use buffer as soon as possible.
         * @param source which ISource this listener was add to
         */
        virtual void onFrameAvaible(BufferInfo *buffer, ISource *source) = 0;
    };

    static ISource *createInstance(char *url);

    virtual ~ISource(){}

    /**
     * @brief add listener, monitoring ISource decide format & produce buffer
     * @param listener who will accept callback
     * @return true if success
     */
    virtual bool addListener(ISourceListener *listener) = 0;
    virtual bool removeListener(ISourceListener *listener) = 0;

    /**
     * @brief open ISource, it will decide output buffer format
     * @return true if open success
     */
    virtual bool open() = 0;

    /**
     * @brief close ISource
     * @return true if open success
     */
    virtual bool close() = 0;

     /**
      * @brief start ISource work
      * @return true if success
      */
    virtual bool start() = 0;

    /**
     * @brief stop ISource work
     * @return true if success
     */
    virtual bool stop() = 0;

    /**
     * @brief Gets the parameter indicated by key.
     * @param key key indicates the parameter to be get.
     * @param value output value of the parameter get
     * @return true if success
     */
    virtual bool getParameter(uint32_t key, uint32_t * value){return false;}

    /**
     * @brief Sets the parameter indicated by key.
     * @param key key indicates the parameter to be set.
     * @param value value of the parameter to be set.
     * @return true if success
     */
    virtual bool setParameter(uint32_t key, uint32_t * value){return false;}
};

class ICameraSource
{
public:
    typedef enum
    {
        PARAM_BLACK_LEVEL = 0,
        PARAM_BRIGHTNESS,
        PARAM_CONTRAST,
        PARAM_RED_BALANCE,
        PARAM_BLUE_BALANCE,
    }ParameterKey;
};

class IVideoSource
{
public:
    virtual bool seek(int seek2) = 0;
    virtual bool step() = 0;
    virtual bool pause() = 0;
};

#endif // LIBCAMERA_ISOURCE_H
