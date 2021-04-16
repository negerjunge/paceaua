#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <chrono>
#include <SDL2/SDL_ttf.h>
#define __STDC_CONSTANT_MACROS
extern "C"
{
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/timestamp.h>
    #include <libavformat/avformat.h>
}
#include <cstdio>
#include <error.h>

//rawframes to render sdl
//clean lib deprecated 
//move global in main

static AVCodecContext *video_dec_ctx = NULL;
static uint8_t *video_dst_data[4] = { NULL };
static int video_dst_linesize[4];
static int video_dst_bufsize;
static int video_stream_idx = -1;
static AVFrame *frame = NULL;
static AVPacket pkt;

static int decode_packet(int *got_frame, int cached)
{
    int ret = 0;
    int decoded = pkt.size;
    *got_frame = 0;
    if (pkt.stream_index == video_stream_idx) {
        /* decode video frame */
        ret = avcodec_decode_video2(video_dec_ctx, frame, got_frame, &pkt);
        if (ret < 0) {
            fprintf(stderr, "Error decoding video frame \n");
            return ret;
        }
        if (*got_frame) {
            av_image_copy(video_dst_data, video_dst_linesize,
                          (const uint8_t **)(frame->data), frame->linesize,
                          video_dec_ctx->pix_fmt, video_dec_ctx->width, video_dec_ctx->height);
            // aici se intampla sexul
            // randat pe ecran
        }
    }
    if (*got_frame)
        av_frame_unref(frame);
    return decoded;
}
static int open_codec_context(int *stream_idx,
                              AVFormatContext *fmt_ctx, enum AVMediaType type, const char *src_filename)
{
    int ret;
    AVStream *st;
    AVCodecContext *dec_ctx = NULL;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(type), src_filename);
        return ret;
    } else {
        *stream_idx = ret;
        st = fmt_ctx->streams[*stream_idx];
        /* find decoder for the stream */
        dec_ctx = st->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }
        /* Init the decoders, with or without reference counting */
        av_dict_set(&opts, "refcounted_frames", "1", 0);
        if ((ret = avcodec_open2(dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
    }
    return 0;
}

static int get_format_from_sample_fmt(const char **fmt,
                                      enum AVSampleFormat sample_fmt)
{
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
        { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;
    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }
    fprintf(stderr,
            "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return -1;
}

bool apasareDeTaste ;


int bani()
{
    int chips = 50000;
    
    
    
    
    
    
    
    
    int multiplier[9] = {5, 10, 25, 50, 100, 500, 1000, 5000, 10000};
    int bet[5] = {250, 500, 1000, 2500, };
    return 0;
}










int main()
{
    
   static AVFormatContext *fmt_ctx = NULL;
    static AVStream *video_stream = NULL;
    static const char *src_filename = NULL;
    static const char *video_dst_filename = NULL;
    int ret = 0, got_frame;

    src_filename = "vid.mp4";
    video_dst_filename = "asdf";
    /* register all formats and codecs */
	
    /* open input file, and allocate format context */
    if (avformat_open_input(&fmt_ctx, src_filename, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", src_filename);
        exit(1);
    }
    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }
    if (open_codec_context(&video_stream_idx, fmt_ctx, AVMEDIA_TYPE_VIDEO, src_filename) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];
        video_dec_ctx = video_stream->codec;
        /* allocate image where the decoded image will be put */
        ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             video_dec_ctx->width, video_dec_ctx->height,
                             video_dec_ctx->pix_fmt, 1);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate raw video buffer\n");
            return 0;
        }
        video_dst_bufsize = ret;
    }
    av_dump_format(fmt_ctx, 0, src_filename, 0);
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        ret = AVERROR(ENOMEM);
        return 0;
    }
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        do {
            ret = decode_packet(&got_frame, 0);
            if (ret < 0)
                break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (pkt.size > 0);
        av_free_packet(&orig_pkt);
    }
    pkt.data = NULL;
    pkt.size = 0;
    do {
        decode_packet(&got_frame, 1);
    } while (got_frame);

end:
    avcodec_close(video_dec_ctx);
    avformat_close_input(&fmt_ctx);

    av_frame_free(&frame);
    av_free(video_dst_data[0]);
// myth thingy solve
//segault framesLenght
//olaru plange



    SDL_Texture *pizdeBune[9] ={};
    SDL_Window *pizdePng;
    SDL_Init(SDL_INIT_VIDEO);
    pizdePng = SDL_CreateWindow("Pacanea Cu Desfranate", 0, 0, 1920, 1080, SDL_WINDOW_MAXIMIZED);
    SDL_Renderer *randat = NULL;
    randat = SDL_CreateRenderer(pizdePng, -1, SDL_RENDERER_ACCELERATED);
    IMG_Init(IMG_INIT_PNG);
    for(int i = 1; i <= 9 ; i++)
    {
      std::string folderPoze = "/home/theasspounder/vsc/pacaneav2/paceaua/pozePacaneaDesfranata/";
      folderPoze = folderPoze + std::to_string(i) + ".png"; 
      SDL_Surface *lemonParty = IMG_Load(folderPoze.c_str());
      if(!lemonParty) std::cout << "pulaMea" << std::endl;
       pizdeBune[i - 1] = SDL_CreateTextureFromSurface(randat, lemonParty);
       SDL_FreeSurface(lemonParty);
    }
    SDL_Rect whores;
     TTF_Init();
     TTF_Font *fontChips;
     fontChips = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 15);
     SDL_Surface *textChips;
     textChips =  TTF_RenderText_Solid(fontChips, "Ai Atatea Belite: ", {223, 194, 123});
     SDL_Texture *texChips;
     texChips = SDL_CreateTextureFromSurface(randat, textChips);
     

while(1 == 1)
{
    SDL_Event pimp;
    SDL_PollEvent(& pimp);
    if(pimp.type == SDL_QUIT)    
{
    break;   
}
    if(pimp.type == SDL_KEYDOWN)
    {
    apasareDeTaste = false;
                if(pimp.type == SDL_KEYDOWN)
                 {
                     apasareDeTaste = true;
                 }
    }
    
    
    if(pimp.type == SDL_KEYUP)
    {
        (char)pimp.key.keysym.sym;
            if(apasareDeTaste == true)
            {
                 std::cout << (char)pimp.key.keysym.sym << std::endl;
                apasareDeTaste = false;
                 apasareDeTaste != true;
            }
    }

SDL_RenderClear(randat);
SDL_Rect whores;
whores.h = 200;
whores.w = 200;
whores.x = 760;
whores.y = 440;
SDL_Rect whoresText;
whoresText.h = 75;
whoresText.w = 500;
whoresText.x = 560;
whoresText.y = 240;
SDL_RenderCopy(randat, pizdeBune[2], NULL, &whores);
SDL_RenderCopy(randat, texChips, NULL, &whoresText);
SDL_RenderPresent(randat);

}
    for(int i = 0; i <= 8; i++)
    {
    SDL_DestroyTexture(pizdeBune[i]);
    }

    SDL_DestroyRenderer(randat);
    SDL_DestroyWindow(pizdePng);   
    SDL_Quit(); 
    return 0;
}