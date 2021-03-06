#ifndef MYTHV4L2M2MCONTEXT_H
#define MYTHV4L2M2MCONTEXT_H

// MythTV
#include "mythdrmprimecontext.h"

class MythV4L2M2MContext : public MythDRMPRIMEContext
{
  public:
    MythV4L2M2MContext(DecoderBase *Parent, MythCodecID CodecID);
   ~MythV4L2M2MContext() override = default;
    static MythCodecID GetSupportedCodec (AVCodecContext **Context,
                                          AVCodec       **Codec,
                                          const QString  &Decoder,
                                          AVStream       *Stream,
                                          uint            StreamType);
    void        InitVideoCodec           (AVCodecContext *Context, bool SelectedStream, bool &DirectRendering) override;
    bool        RetrieveFrame            (AVCodecContext *Context, VideoFrame *Frame, AVFrame *AvFrame) override;
    void        SetDecoderOptions        (AVCodecContext* Context, AVCodec* Codec) override;
    int         HwDecoderInit            (AVCodecContext *Context) override;
    bool        DecoderWillResetOnFlush  (void) override;
    static bool GetBuffer                (AVCodecContext *Context, VideoFrame *Frame, AVFrame *AvFrame, int/*Flags*/);
    static bool HaveV4L2Codecs           (AVCodecID Codec = AV_CODEC_ID_NONE);

    static enum AVPixelFormat GetV4L2RequestFormat(AVCodecContext *Context, const AVPixelFormat *PixFmt);
    static int  InitialiseV4L2RequestContext(AVCodecContext *Context);
};

#endif // MYTHV4L2M2MCONTEXT_H
